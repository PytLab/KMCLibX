""" Module for the KMCSitesMap """

# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

from KMCLib.Backend import Backend
from KMCLib.Exceptions.Error import Error
from KMCLib.CoreComponents.KMCConfiguration import KMCConfiguration
from KMCLib.Utilities.ConversionUtilities import numpy2DArrayToStdVectorStdVectorDouble
from KMCLib.Utilities.ConversionUtilities import stringListToStdVectorString


class KMCSitesMap(KMCConfiguration):
    """
    Class for representing the sitesmap in a KMC simulation. The class is derived
    from KMCConfiguration class with some needless function overloaded.
    """

    def __init__(self,
                 lattice=None,
                 types=None,
                 possible_types=None,
                 default_type=None):
        """
        Constructor for the KMCSitesMap - the sitesmap object to use
        in the KMC simulations.

        :param lattice: The lattice of the sitesmap as a KMCLattice.

        :param types: The site types at the lattice points as a list, e.g. ['type1','type2',..],
                      ordered as [a,b,c,i] with i being the fastest and a the slowest index and
                      a, b and c refers to the cell repetitions and i
                      refers to the specific basis point in the cell. When using this format
                      one cannot specify a default type, and the number of elements in the
                      types list must match the number of grid points in the lattice. Alternatively
                      one can specify the types input as a list of tuples specifying the a,b,c,i for each type,
                      e.g. [(0,0,1,0,'a'), (0,0,1,1,'b'), ...]. If one uses the longer tuple format
                      a default type should be given, which then will be used for all points not
                      explicitly specified in the list.

        :param possible_types: A list of possible types. If not given this list will be set to the
                               types present in the types list by default.

        :param default_type: This input parameter can only be given if the types are
                             given in long format i.e. [(0,0,1,0,'a'), (0,0,1,1,'b'), ...]
                             The default type will then be used for lattice sites
                             not specified in the types list.
        """
        super(self.__class__, self).__init__(lattice, types,
                                             possible_types,
                                             default_type)

    def __checkAndSetTypes(self, types, default_type, possible_types):
        """
        Private helper function to check and set the site types input.
        """
        super(self.__class__, self).__checkAndSetTypes(types, default_type,
                                                       possible_types)

    def types(self):
        """
        Query function for the site types of the sitesmap.

        :returns: The stored types list.
        """
        # Update the types with what ever has been changed in the backend.
        mangled_name = self.__mangled_name("__types")
        setattr(self, mangled_name, list(self._backend().sites()))

        # Return the types.
        return getattr(self, mangled_name)

    def atomIDTypes(self):
        """
        Empty function to override KMCConfiguration atomIDTypes function.
        """
        pass

    def atomIDCoordinates(self):
        """
        Empty function to override KMCConfiguration atomIDCoordinates function.
        """
        pass

    def movedAtomIDs(self):
        """
        Empty function to override KMCConfiguration movedAtomIDs function.
        """
        pass

    def _backend(self):
        """
        Query function for the sitesmap c++ backend object.
        """
        backend_mangled_name = self.__mangled_name("__backend")

        if getattr(self, backend_mangled_name) is None:
            # Construct the c++ backend object.
            mangled_name = self.__mangled_name("__types")
            cpp_types = stringListToStdVectorString(getattr(self, mangled_name))

            mangled_name = self.__mangled_name("__lattice")
            cpp_coords = numpy2DArrayToStdVectorStdVectorDouble(
                getattr(self, mangled_name).sites())

            mangled_name = self.__mangled_name("__possible_types")
            cpp_possible_types = Backend.StdMapStringInt(getattr(self, mangled_name))

            # Send in the coordinates and types to construct the backend sitesmap.
            cpp_sitesmap = Backend.SitesMap(cpp_coords, cpp_types, cpp_possible_types)

            # Set attribute.
            setattr(self, backend_mangled_name, cpp_sitesmap)

        # Return the backend.
        return getattr(self, backend_mangled_name)

    def __mangled_name(self, private_varname):
        """
        Private helper function to get the mangled name of private variable.
        """
        # Get parent class name.
        parent_class_name = self.__class__.__base__.__name__

        # Return mangled variable name.
        return "_" + parent_class_name + private_varname

    def siteTypesMapping(self, site_types):
        """ 
        Helper function to map site types from string list to int list
        according to possible_site_types.

        :param site_types: A string list of site types.

        :returns: A corresponding integer site types list.
        """
        int_site_types = []
        possible_types = self.possibleTypes()

        for site_type in site_types:
            # Check site type validity.
            if site_type not in possible_types:
                msg = "Type '%s' is not in possible types." % site_type
                raise Error(msg)

            # Collect int site type.
            int_site_type = possible_types[site_type]
            int_site_types.append(int_site_type)

        return int_site_types

    def _script(self, variable_name="sitesmap"):
        """
        Generate a KMCLib Python script representation of this sitesmap.

        :param variable_name: A name to use as variable name for
                              the KMCSitesMap in the generated script.
        :type variable_name: str

        :returns: A KMCLib Python script, as a string,
                  that can generate this sitesmap.
        """
        # Get the lattice script.
        lattice_script = self.lattice()._script(variable_name="lattice")

        # Get the types string.
        types_string = "types = "
        indent = " "*9
        line = "["
        nT = len(self.types())
        for i, t in enumerate(self.types()):
            # Add the type.
            line += "'" + t + "'"
            if i == nT-1:
                # Stop if we reach the end.
                line += "]\n"
                types_string += line
                break
            else:
            # Add the separator.
                line += ","

            # Check if we should add a new line.
            if len(line) > 50:
                types_string += line + "\n" + indent
                line = ""

        # Generate the possible types string.
        possible_types_string = "possible_types = "
        indent = " "*18
        line = "["

        # Sort possible_types for compatibilty py2 & py3.
        sorted_possible_types = sorted(list(set(self.possibleTypes().keys())))
        possible_types = [t for t in sorted_possible_types if t != "*"]

        nT = len(possible_types)
        for i, t in enumerate(possible_types):
            # Add the type.
            line += "'" + t + "'"
            if i == nT - 1:
                # Stop if we reach the end.
                line += "]\n"
                possible_types_string += line
                break
            else:
            # Add the separator.
                line += ","

            # Check if we should add a new line.
            if len(line) > 50:
                possible_types_string += line + "\n" + indent
                line = ""

        # Setup the sitesmap string.
        sitesmap_string = (variable_name + " = KMCSitesMap(\n" +
                           "    lattice=lattice,\n" +
                           "    types=types,\n" +
                           "    possible_types=possible_types)\n")

        # Add the comment.
        comment_string = ("\n# ---------------------------------------------" +
                          "--------------------------------\n" +
                          "# SitesMap\n\n")

        # Return the script.
        return (lattice_script + comment_string + types_string + "\n" +
                possible_types_string + "\n" + sitesmap_string)

    def _atkScript(self, types_map):
        pass
