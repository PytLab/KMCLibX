""" Module for the KMCLatticeModel """


# Copyright (c)  2012-2015  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import logging

from KMCLib import mpi_master
from KMCLib.Backend import Backend
from KMCLib.CoreComponents.KMCConfiguration import KMCConfiguration
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap
from KMCLib.CoreComponents.KMCInteractions import KMCInteractions
from KMCLib.CoreComponents.KMCControlParameters import KMCControlParameters
from KMCLib.PluginInterfaces.KMCAnalysisPlugin import KMCAnalysisPlugin
from KMCLib.Exceptions.Error import Error
from KMCLib.Utilities.CheckUtilities import checkSequenceOf
from KMCLib.Utilities.CheckUtilities import checkSequenceOfPositiveIntegers
from KMCLib.Utilities.CheckUtilities import checkPositiveFloat
from KMCLib.Utilities.CheckUtilities import checkPositiveInteger
#from KMCLib.Utilities.PrintUtilities import prettyPrint
from KMCLib.Utilities.PrintUtilities import convert_time
from KMCLib.Utilities.Trajectory.LatticeTrajectory import LatticeTrajectory
from KMCLib.Utilities.Trajectory.XYZTrajectory import XYZTrajectory


class KMCLatticeModel(object):
    """
    Class for representing a lattice KMC model.
    """

    def __init__(self,
                 configuration=None,
                 sitesmap=None,
                 interactions=None):
        """
        The KMCLatticeModel class is the central object in the KMCLib framework
        for running a KMC simulation. Once a configuration and with a lattice is
        defined and a set of interactions are setup, the KMCLatticeModel object
        unites this information, checks that the given interactions match the
        configurations, and provides means for running a KMC Lattice simulation.

        :param configuration: The KMCConfiguration to run the simulation for.

        :param sitesmap: The KMCSitesMap the simulation runs on.

        :param interactions: The KMCInteractions that specify possible local
                             states and barriers to use in the simulation.

        """
        # {{{
        # Check the configuration.
        if not isinstance(configuration, KMCConfiguration):
            msg = ("The 'configuration' parameter to the KMCLatticeModel " +
                   "must be an instance of type KMCConfiguration.")
            raise Error(msg)

        # Store.
        self.__configuration = configuration

        # Check the sitesmap.
        if not isinstance(sitesmap, KMCSitesMap):
            msg = ("The 'sitesmap' parameter to the KMCLatticeModel " +
                   "must be an instance of type KMCSitesMap.")
            raise Error(msg)

        # Store.
        self.__sitesmap = sitesmap

        # Check the interactions.
        if not isinstance(interactions, KMCInteractions):
            msg = ("The 'interactions' parameter to the KMCLatticeModel " +
                   "must be an instance of type KMCInteractions.")
            raise Error(msg)

        # Store.
        self.__interactions = interactions

        # Set the backend to be generated at first query.
        self.__backend = None

        # Set the verbosity level of output to minimal.
        self.__verbosity_level = 0

        # Set logger.
        self.__logger = logging.getLogger("KMCLibX.KMCLatticeModel")
        # }}}

    def interactions(self):
        """
        Query function for interactions object in model.

        :returns: KMCInteractions object.
        """
        return self.__interactions

    def _backend(self, start_time):
        """
        Function for generating the C++ backend reperesentation of this object.

        :param start_time: The start time for kMC loop
        :type: float.

        :returns: The C++ LatticeModel based on the parameters given to this
                  class on construction.
        """
        # {{{
        if self.__backend is None:
            # Setup the C++ objects we need.
            cpp_config = self.__configuration._backend()
            cpp_sitesmap = self.__sitesmap._backend()
            cpp_lattice_map = self.__configuration._latticeMap()
            cpp_interactions = self.__interactions._backend(self.__configuration.possibleTypes(),
                                                            cpp_lattice_map.nBasis())

            # Construct a timer.
            self.__cpp_timer = Backend.SimulationTimer(start_time=start_time)

            # Construct the backend object.
            self.__backend = Backend.LatticeModel(cpp_config,
                                                  cpp_sitesmap,
                                                  self.__cpp_timer,
                                                  cpp_lattice_map,
                                                  cpp_interactions)
        # Return.
        return self.__backend
        # }}}

    def run(self,
            control_parameters=None,
            trajectory_filename=None,
            trajectory_type=None,
            analysis=None):
        """
        Run the KMC lattice model simulation with specified parameters.

        :param control_paramters: An instance of KMCControlParameters specifying
                                  number of steps to run etc.

        :param trajectory_filename: The filename of the trajectory. If not given
                                    no trajectory will be saved.

        :param trajectory_type: The type of trajectory to use. Either 'lattice' or 'xyz'.
                                The 'lattice' format shows the types at the latice points.
                                The 'xyz' format gives type and coordinate for each particle.
                                The default type is 'lattice'.

        :param analysis: A list of instantiated analysis objects that should be
                         used for on-the-fly analysis.
        """
        # {{{
        # Check the input.
        if not isinstance(control_parameters, KMCControlParameters):
            msg = ("The 'control_parameters' input to the KMCLatticeModel run funtion" +
                   "must be an instance of type KMCControlParameters.")
            raise Error(msg)

        # Check the trajectory filename.
        use_trajectory = True
        if trajectory_filename is None:
            use_trajectory = False
            msg = "No trajectory filename given -> no trajectory will be saved."
            #prettyPrint(msg)
            if mpi_master:
                self.__logger.warning(msg)

        elif not (isinstance(trajectory_filename, str) or
                  isinstance(trajectory_filename, unicode)):
            msg = ("The 'trajectory_filename' input to the KMCLattice model " +
                   "run function must be given as str or unicode.")
            raise Error(msg)

        # Check the analysis type.
        if trajectory_type is None:
            trajectory_type = 'lattice'

        if not isinstance(trajectory_type, str):
            raise Error("The 'trajectory_type' input must given as a string.")

        # Check the analysis.
        if analysis is None:
            analysis = []
        else:
            msg = ("Each element in the 'analysis' list must be an" +
                   "instance of KMCAnalysisPlugin.")
            analysis = checkSequenceOf(analysis, KMCAnalysisPlugin, msg)

        # Set and seed the backend random number generator.
        if not Backend.setRngType(control_parameters.rngType()):
            raise Error("DEVICE random number generator is not supported by your system, " +
                        "or the std::random_device in the standard C++ library you use " +
                        "is implemented using a pseudo random number generator (entropy=0).")

        Backend.seedRandom(control_parameters.timeSeed(),
                           control_parameters.seed())

        # Construct the C++ lattice model.
        #prettyPrint(" KMCLib: setting up the backend C++ object.")
        if mpi_master:
            self.__logger.info("")
            self.__logger.info("setting up the backend C++ object.")

        start_time = control_parameters.startTime()
        cpp_model = self._backend(start_time)

        # Print the initial matching information if above the verbosity threshold.
        if self.__verbosity_level > 9:
            self.__printMatchInfo(cpp_model)

        # Check that we have at least one available process to  run the KMC simulation.
        if cpp_model.interactions().totalAvailableSites() == 0:
            raise Error("No available processes. None of the processes " +
                        "defined as input match any position in the configuration. " +
                        "Change the initial configuration or processes to run KMC.")

        # Setup a trajectory object.
        if use_trajectory:
            if trajectory_type == 'lattice':
                trajectory = LatticeTrajectory(trajectory_filename=trajectory_filename,
                                               configuration=self.__configuration)
            elif trajectory_type == 'xyz':
                trajectory = XYZTrajectory(trajectory_filename=trajectory_filename,
                                           configuration=self.__configuration)
            else:
                raise Error("The 'trajectory_type' input must be either 'lattice' or 'xyz'.")

            # Add the first step.
            trajectory.append(simulation_time=self.__cpp_timer.simulationTime(),
                              step=0,
                              configuration=self.__configuration)

        # Setup the analysis objects.
        for ap in analysis:
            step = 0
            ap.setup(step=step,
                     time=self.__cpp_timer.simulationTime(),
                     configuration=self.__configuration,
                     interactions=self.__interactions)

        # Get the needed parameters.
        end_time = control_parameters.timeLimit()
        n_steps = control_parameters.numberOfSteps()
        n_dump = control_parameters.dumpInterval()
        analysis_interv = control_parameters.analysisInterval()
        extra_traj = control_parameters.extraTraj()

        # Re-distribution related parameters.
        do_redistribution = control_parameters.doRedistribution()
        if do_redistribution:
            redistribution_interval = control_parameters.redistributionInterval()
            fast_species = control_parameters.fastSpecies()
            nsplits = control_parameters.nsplits()

        # Check validity of analysis number.
        if type(analysis_interv) in (list, tuple):
            if len(analysis_interv) != len(analysis):
                msg = "analysis intervals number ({}) != analysis objects number ({})"
                msg = msg.format(len(analysis_interv), len(analysis))
                raise Error(msg)
        else:
            # Convert to list.
            analysis_interv = [analysis_interv]*len(analysis)

        #prettyPrint(" KMCLib: Runing for %i steps, starting from time: %f\n" %
        #            (n_steps, self.__cpp_timer.simulationTime()))
        if mpi_master:
            msg = "Runing for {:,d} steps, starting from time: {:f}\n"
            self.__logger.info(msg.format(n_steps, self.__cpp_timer.simulationTime()))

        # Run the KMC simulation.
        try:
            # Loop over the steps.
            step = 0
            current_time = 0.0

            # Flags for what operation is in process.
            redistribution = False
            kmcstep = False

            while(1):
                # Start from step 1.
                step += 1

                # Check if it is possible to take a step.
                nP = cpp_model.interactions().totalAvailableSites()
                if nP == 0:
                    raise Error("No more available processes.")

                # Get which operation would be done in this iteration.
                if do_redistribution and (step % redistribution_interval) == 0:
                    redistribution = True
                    step -= 1
                else:
                    kmcstep = True

                # Take a step.
                if kmcstep:
                    cpp_model.singleStep()

                    # Time increase.
                    current_time = self.__cpp_timer.simulationTime()

                    if (step % n_dump == 0):
                        #prettyPrint(" KMCLib: %i steps executed. time: %20.10e " %
                        #           (step, self.__cpp_timer.simulationTime()))
                        if mpi_master:
                            msg = ("[{:>3d}%] [{:>6.2f}%] {:,d} steps executed. " +
                                   "time: {:0>2d}:{:0>2d}:{:0>2d} ({:<.2e}) delta: {:<10.5e}")
                            step_percent = int(float(step)/n_steps*100)
                            time_percent = current_time/end_time*100
                            hour, minute, second = convert_time(current_time)
                            self.__logger.info(msg.format(step_percent, time_percent, step,
                                                          hour, minute, int(second), current_time,
                                                          self.__cpp_timer.deltaTime()))

                        # Perform IO using the trajectory object.
                        if use_trajectory:
                            trajectory.append(simulation_time=current_time,
                                              step=step,
                                              configuration=self.__configuration)

                    # Extra trajectorie output.
                    if extra_traj is not None:
                        start, end, interval = extra_traj
                        if step >= start and step <= end and (step % interval == 0):
                            trajectory.append(simulation_time=current_time,
                                              step=step,
                                              configuration=self.__configuration)

                    # Run all other python analysis.
                    for intv, ap in zip(analysis_interv, analysis):
                        # NOTE: intv(interval) can be int or list/tuple of int here.

                        if type(intv) is int and ((step % intv) == 0):
                            ap.registerStep(step=step,
                                            time=current_time,
                                            configuration=self.__configuration,
                                            interactions=self.__interactions)

                        elif type(intv) in (list, tuple):
                            start, end, interval = intv
                            if step >= start and step <= end and (step % interval == 0):
                                ap.registerStep(step=step,
                                                time=current_time,
                                                configuration=self.__configuration,
                                                interactions=self.__interactions)

                    # Check loop conditions.
                    if step >= n_steps:
                        if mpi_master:
                            self.__logger.info("Max kMC step reached, kMC iteration finish.")
                        break

                    if current_time > end_time:
                        if mpi_master:
                            self.__logger.info("Time limit reached, kMC iteration finish.")
                        break

                if redistribution:
                    # Re-distribute the configuration.
                    cpp_model.redistribute(fast_species, *nsplit)

                    # Time increase.
                    current_time = self.__cpp_timer.simulationTime()

                    if mpi_master:
                        msg = ("Configuration re-distribution is executed. " +
                               "time: {:0>2d}:{:0>2d}:{:0>2d} ({:<.2e})")
                        hour, minute, second = convert_time(current_time)
                        self.__logger.info(msg.format(hour, minute, int(second), current_time))

                    if use_trajectory:
                        trajectory.append(simulation_time=current_time,
                                          step=step,
                                          configuration=self.__configuration)

        finally:

            # Flush the trajectory buffers when done.
            if use_trajectory:
                trajectory.flush()

            # Perform the analysis post processing.
            for ap in analysis:
                ap.finalize()
        # }}}

    def _script(self, variable_name="model"):
        """
        Generate a script representation of an instance.

        :param variable_name: A name to use as variable name for
                              the KMCLatticeModel in the generated script.
        :type variable_name: str

        :returns: A script that can generate this kmc lattice model.
        """
        # {{{
        # Get the configuration and interactions scripts.
        configuration_script = self.__configuration._script(variable_name="configuration")
        interactions_script = self.__interactions._script(variable_name="interactions")
        sitesmap_script = self.__sitesmap._script(variable_name="sitesmap")

        # Setup the lattice model string.
        lattice_model_string = (variable_name +
                                " = KMCLatticeModel(\n" +
                                "    configuration=configuration,\n" +
                                "    sitesmap=sitesmap,\n" +
                                "    interactions=interactions)\n")

        # And a comment string.
        comment_string = "\n# " + "-"*77 + "\n# Lattice model\n\n"

        # Return the script.
        return (configuration_script + sitesmap_script + interactions_script +
                comment_string + lattice_model_string)
        # }}}

    def __printMatchInfo(self, cpp_model):
        """ """
        """
        Private routine for printing the initial matching information
        given a C++ lattice model object.

        :param cpp_model: The C++ lattice model to print the matching for.
        """
        cpp_processes = cpp_model.interactions().processes()

        #prettyPrint("")
        #prettyPrint(" Matching Information: ")
        if mpi_master:
            self.__logger.info("")
            self.__logger.info("Matching Informations: ")
        for i, p in enumerate(cpp_processes):
            print i, p.sites()

