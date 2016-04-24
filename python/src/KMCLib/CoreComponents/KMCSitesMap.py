""" Module for the KMCSitesMap """

# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

from ..Utilities.ConversionUtilities import stringListToStdVectorString
from ..Utilities.ConversionUtilities import numpy2DArrayToStdVectorStdVectorDouble
from ..Utilities.ConversionUtilities import stdVectorCoordinateToNumpy2DArray
from ..Backend import Backend
from .KMCConfiguration import KMCConfiguration


class KMCSitesMap(KMCConfiguration):
    """
    Class for representing the sitesmap in a KMC simulation. The class is derived
    from KMCConfiguration class with some needless function overloaded.
    """

    def __init__(self,
                 lattice=None,
                 types=None,
                 possible_types=None,
                 default_types=None):
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
        super(self.__class__, self).__init__(lattice, types, possible_types, default_types)

    def __checkAndSetTypes(self, types, default_type, possible_types):
        """
        Private helper function to check and set the site types input.
        """
        super(self.__class__, self).__checkAndSetTypes(types, default_type, possible_types)

    def types(self):
        """
        Query function for the site types of the sitesmap.

        :returns: The stored types list.
        """
        # Update the types with what ever has been changed in the backend.
        self.__types = list(self._backend().sites())

        # Return the types.
        return self.__types

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
            cpp_coords = numpy2DArrayToStdVectorStdVectorDouble(getattr(self, mangled_name).sites())

            mangled_name = self.__mangled_name("__possible_types")
            cpp_possible_types = Backend.StdMapStringInt(getattr(self, mangled_name))

            # Send in the coordinates and types to construct the backend configuration.
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

