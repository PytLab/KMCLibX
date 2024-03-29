""" Module for the KMCInteractions """


# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project(based on KMCLib) distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import inspect
from functools import wraps

from KMCLib.Backend import Backend
from KMCLib.Utilities.CheckUtilities import checkSequenceOf
from KMCLib.Exceptions.Error import Error
from KMCLib.CoreComponents.KMCProcess import KMCProcess
from KMCLib.PluginInterfaces.KMCRateCalculatorPlugin import KMCRateCalculatorPlugin
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap


class KMCInteractions(object):
    """
    Class for holding information about all possible interactions used in a
    Lattice KMC simulation.
    """

    def __init__(self,
                 processes=None,
                 sitesmap=None,
                 implicit_wildcards=None):
        """
        Constructor for the KMCInteractions.

        :param processes: A list of possible processes in the simulation.

        :param sitesmap: KMCSitesMap object on which all processes are performed.

        :param implicit_wildcards: A flag indicating if implicit wildcards should be used in
                                   the matching of processes with the configuration. The default
                                   is True, i.e. to use implicit wildcards.
        :type implicit_wildcards:  bool
        """
        # Check the processes input.
        self.__processes = checkSequenceOf(processes, KMCProcess,
                                           msg="The 'processes' input must be " +
                                               "a list of KMCProcess instances.")

        # Check that the sitesmap is of the correct type.
        if sitesmap and not isinstance(sitesmap, KMCSitesMap):
            msg = ("The sitesmap given to the KMCInteractions constructor " +
                   "must be of type KMCSitesMap.")
            raise Error(msg)

        # Check if process has site types.
        if not sitesmap:
            for i, process in enumerate(processes):
                if process.siteTypes():
                    msg = "Site types in process%d are set, sitesmap must be supplied." % i
                    raise Error(msg)

        self.__sitesmap = sitesmap

        # Check the implicit wildcard flag.
        if implicit_wildcards is None:
            implicit_wildcards = True
        if not isinstance(implicit_wildcards, bool):
            raise Error("The 'implicit_wildcard' flag to the KMCInteractions constructor" +
                        " must be given as either True or False")
        self.__implicit_wildcards = implicit_wildcards

        # Set the backend to be generated at first query.
        self.__backend = None

        # Set the rate calculator.
        self.__rate_calculator = None

    def sitesMap(self):
        """
        Query for SitesMap object stored.
        :returns: The stored sitesmap object.
        """
        return self.__sitesmap

    def rateCalculator(self):
        """
        Query for the rate calculator.
        :returns: The stored rate calculator.
        """
        return self.__rate_calculator

    def setRateCalculator(self,
                          rate_calculator=None):
        """
        Set the rate calculator of the class. The rate calculator must be
        set before the backend is generated to take effect.

        :param rate_calculator:    A class inheriting from the
                                   KMCRateCalculatorPlugin interface. If not given
                                   the rates specified for each process will be used unmodified.

        """
        # Check the rate calculator.
        if rate_calculator is not None:

            # Check if this is a class.
            if not inspect.isclass(rate_calculator):
                msg = ("\nThe 'rate_calculator' input to the KMCInteractions constructor " +
                       "must be a class (not instantiated) inheriting " +
                       "from the KMCRateCalculatorPlugin.")
                raise Error(msg)

            # Save the class name for use in scripting.
            self.__rate_calculator_str = str(rate_calculator).replace("'>", "").split('.')[-1]
            # Instantiate.
            rate_calculator = rate_calculator()
            if not isinstance(rate_calculator, KMCRateCalculatorPlugin):
                msg = ("\nThe 'rate_calculator' input to the KMCInteractions constructor " +
                       "must be a class inheriting from the KMCRateCalculatorPlugin.")
                raise Error(msg)
            elif rate_calculator.__class__ == KMCRateCalculatorPlugin().__class__:
                msg = ("\nThe 'rate_calculator' input to the KMCInteractions constructor " +
                       "must be inheriting from the KMCRateCalculatorPlugin class. " +
                       "It may not be the KMCRateCalculatorPlugin class itself.")
                raise Error(msg)

        # All tests passed. Save the instantiated rate calculator on the class.
        self.__rate_calculator = rate_calculator

    def implicitWildcards(self):
        """
        Query for the implicit wildcard flag.

        :returns: The implicit wildcard flag stored.
        """
        return self.__implicit_wildcards

    def check_backend(func):
        """
        Decorator for c++ backend object checking.
        """
        @wraps(func)
        def wrapper(self, *args):
            if self.__backend is None:
                msg = "No cpp interactions backend in {}.".format(self.__class__.__name__)
                raise AttributeError(msg)
            return func(self, *args)

        return wrapper

    @check_backend
    def pickedIndex(self):
        """
        Query function for the index of process which was picked in last step.

        :returns: The number of index.
        """
        return self.__backend.pickedIndex()

    @check_backend
    def processAvailableSites(self):
        """
        Query function for list of available site number for all processes.

        :returns: A tuple of available site number.
        """
        return self.__backend.processAvailableSites()

    @check_backend
    def processRates(self):
        """
        Query function for list of rates for all processes.

        :returns: A list of process rates.
        """
        cpp_processes = self.__backend.processes()
        rates = []
        for process in cpp_processes:
            rates.append(process.rateConstant())

        return rates

    def _backend(self, possible_types, n_basis):
        """
        Query for the interactions backend object.

        :param possible_types: A dict with the global mapping of type strings
                               to integers.

        :param n_basis: The size of the configuration basis is.
        :type n_basis: int

        :returns: The interactions object in C++
        """
        # {{{
        if self.__backend is None:

            # Check the possible_types against the types in the processes.
            for process_number, process in enumerate(self.__processes):
                all_elements = list(set(process.elementsBefore() + process.elementsAfter()))
                if (not all([(e in possible_types) for e in all_elements])):
                    msg = (("Process %i contains elements not present in the list of " +
                            "possible types of the configuration.") % process_number)
                    raise Error(msg)

            # Setup the correct type of backend process objects
            # depending on the presence of a rate calculator.

            if self.__rate_calculator is not None:
                cpp_processes = Backend.StdVectorCustomRateProcess()
            else:
                cpp_processes = Backend.StdVectorProcess()

            # For each interaction.
            for process_number, process in enumerate(self.__processes):

                # Get the corresponding C++ objects.
                cpp_config1 = process.localConfigurations()[0]._backend(possible_types)
                cpp_config2 = process.localConfigurations()[1]._backend(possible_types)
                rate_constant = process.rateConstant()

                basis_list = range(n_basis)
                if process.basisSites() is not None:
                    # Make sure this basis list does not contain positions
                    # that are not in the configuration.
                    basis_list = []
                    for b in process.basisSites():
                        if b < n_basis:
                            basis_list.append(b)

                # And construct the C++ entry.
                cpp_basis = Backend.StdVectorInt(basis_list)

                # Setup the move vectors representation in C++.
                move_origins = [int(v[0]) for v in process.moveVectors()]
                cpp_move_origins = Backend.StdVectorInt(move_origins)
                cpp_move_vectors = Backend.StdVectorCoordinate()
                for v in process.moveVectors():
                    cpp_move_vectors.push_back(Backend.Coordinate(v[1][0], v[1][1], v[1][2]))

                # Setup the site types.
                if self.__sitesmap:
                    int_site_types = self.__sitesmap.siteTypesMapping(process.siteTypes())
                    cpp_site_types = Backend.StdVectorInt(int_site_types)
                else:
                    cpp_site_types = Backend.StdVectorInt()

                # Setup fast process related flag.
                fast = process.fast()
                redist = process.redist()
                redist_species = process.redistSpecies()

                # Construct and store the C++ process.
                if self.__rate_calculator is not None:
                    # Set the cutoff correctly.
                    cutoff = self.__rate_calculator.cutoff()
                    if cutoff is None:
                        cutoff = 1.0

                    cpp_processes.push_back(Backend.CustomRateProcess(cpp_config1,
                                                                      cpp_config2,
                                                                      rate_constant,
                                                                      cpp_basis,
                                                                      cutoff,
                                                                      cpp_move_origins,
                                                                      cpp_move_vectors,
                                                                      process_number,
                                                                      cpp_site_types,
                                                                      fast,
                                                                      redist,
                                                                      redist_species))
                else:
                    cpp_processes.push_back(Backend.Process(cpp_config1,
                                                            cpp_config2,
                                                            rate_constant,
                                                            cpp_basis,
                                                            cpp_move_origins,
                                                            cpp_move_vectors,
                                                            process_number,
                                                            cpp_site_types,
                                                            fast,
                                                            redist,
                                                            redist_species))

            # Construct the C++ interactions object.
            if self.__rate_calculator is not None:
                self.__backend = Backend.Interactions(cpp_processes,
                                                      self.__implicit_wildcards,
                                                      self.__rate_calculator)
            else:
                self.__backend = Backend.Interactions(cpp_processes,
                                                      self.__implicit_wildcards)

        # Return the stored backend.
        return self.__backend
        # }}}

    def _script(self, variable_name="interactions"):
        """
        Generate a script representation of an instance.

        :param variable_name: A name to use as variable name for
                              the KMCInteractions in the generated script.
        :type variable_name: str

        :returns: A script that can generate this interactions object.
        """
        # {{{
        # Loop through the list of interactions and for each one set up the
        # script for the pair of local configurations that goes together in a tuple.
        processes_script = ""
        processes_string = "processes = ["

        for i, process in enumerate(self.__processes):

            var_name = "process_%i" % (i)
            processes_script += process._script(var_name)

            if i == 0:
                indent = ""
            else:
                indent = " "*13

            processes_string += indent + var_name

            # If this is the last process, close.
            if i == len(self.__processes) - 1:
                processes_string += "]\n"
            else:
                processes_string += ",\n"

        # Get sitesmap construction string.
        if self.__sitesmap:
            sitesmap_script = self.__sitesmap._script("sitesmap")
        else:
            sitesmap_script = ""

        # Add a comment line.
        comment_string = "\n# " + "-"*77 + "\n# Interactions\n\n"

        # implicit wildcard string.
        if self.__implicit_wildcards:
            implicit = "True"
        else:
            implicit = "False"

        # Sitesmap.
        if self.__sitesmap:
            sitesmap = "sitesmap"
        else:
            sitesmap = "None"

        kmc_interactions_string = (
            variable_name +
            " = KMCInteractions(\n" +
            "    processes=processes,\n" +
            "    sitesmap=%s,\n"
            "    implicit_wildcards=%s)\n") % (sitesmap, implicit)

        # Return the script.
        return (comment_string + processes_script + processes_string +
                sitesmap_script + "\n" + kmc_interactions_string)
        # }}}

