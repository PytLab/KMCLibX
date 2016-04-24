"""" Module for testing KMCConfiguration """

# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest
import numpy

from KMCLib.Exceptions.Error import Error
from KMCLib.CoreComponents.KMCUnitCell import KMCUnitCell
from KMCLib.CoreComponents.KMCLattice import KMCLattice
from KMCLib.Backend import Backend

# Import the module to test.
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap


# Implementing the tests.
class KMCSitesMapTest(unittest.TestCase):
    """ Class for testing the KMCConfiguration class """

    def testConstructionShortFormat(self):
        """ Test that the KMCSitesMap class can be constructed. """

        # Setup a valid KMCUnitCell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8, 0.0, 0.0],
                                                          [0.0, 3.2, 0.0],
                                                          [0.0, 0.5, 3.0]]),
                                basis_points=[[0.0, 0.0, 0.0],
                                              [0.5, 0.5, 0.5],
                                              [0.25, 0.25, 0.75]])

        # Setup the lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4, 4, 1),
                             periodic=(True, True, False))

        types = ['a', 'a', 'a', 'a', 'b', 'b',
                 'a', 'a', 'a', 'b', 'b', 'b',
                 'b', 'b', 'a', 'a', 'b', 'a',
                 'b', 'b', 'b', 'a', 'b', 'a',
                 'b', 'a', 'a', 'a', 'b', 'b',
                 'b', 'b', 'b', 'b', 'b', 'b',
                 'a', 'a', 'a', 'a', 'b', 'b',
                 'b', 'b', 'a', 'b', 'b', 'a']

        # Setup the configuration.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a', 'c', 'b'])

        # Check that the types are what we expect.
        self.assertEqual(types, sitesmap._KMCConfiguration__types)
        # Check query function.
        self.assertEqual(types, sitesmap.types())

        # Check that the possible types are what we expect.
        self.assertEqual(set(['a', 'c', 'b', '*']),
                         set(sitesmap._KMCConfiguration__possible_types.keys()))
        # Check query function.
        self.assertEqual(set(['a', 'c', 'b', '*']),
                         set(sitesmap.possibleTypes().keys()))

        # Check that the number of lattice sites corresponds to the lattice.
        self.assertEqual(sitesmap._KMCConfiguration__n_lattice_sites,
                         len(lattice.sites()))

        # Check that the lattice site can be returned from the configuration.
        self.assertAlmostEqual(numpy.linalg.norm(numpy.array(sitesmap.sites()) -
                               numpy.array(lattice.sites())), 0.0, 10)

        # Check the empty oveloaded functions.
        self.assertIs(sitesmap.atomIDTypes(), None)
        self.assertIs(sitesmap.atomIDCoordinates(), None)
        self.assertIs(sitesmap.movedAtomIDs(), None)

        # Construct without possible types and check that the list is set correctly
        # from the given types.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types)
        self.assertEqual(set(['a', 'b', '*']),
                         set(sitesmap._KMCConfiguration__possible_types.keys()))

    def testConstructionLongFormat(self):
        """ Test that the KMCSitesMap class can be constructed with the long types format. """

        # Setup a valid KMCUnitCell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8, 0.0, 0.0],
                                                          [0.0, 3.2, 0.0],
                                                          [0.0, 0.5, 3.0]]),
                                basis_points=[[0.0, 0.0, 0.0],
                                              [0.5, 0.5, 0.5],
                                              [0.25, 0.25, 0.75]])

        # Setup the lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4, 3, 2),
                             periodic=(True, True, False))

        types = [(0, 0, 0, 0, 'g'), (3, 2, 1, 2, 'h')]
        default_type = 'a'

        # Setup the configuration.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               default_type=default_type)

        ref_types = ['g', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                     'a', 'a', 'a', 'a', 'a', 'a', 'a', 'h']

        # Check that they are what we inserted.
        self.assertEqual(ref_types, sitesmap._KMCConfiguration__types)
        # Check query function.
        self.assertEqual(ref_types, sitesmap.types())

        # Check that the possible types are what we expect.
        self.assertEqual(set(['a','g','h','*']),
                         set(sitesmap._KMCConfiguration__possible_types))
        # Check query function.
        self.assertEqual(set(['a','g','h','*']),
                         set(sitesmap.possibleTypes()))

        # Check that the number of lattice sites corresponds
        # to the lattice.
        self.assertEqual(sitesmap._KMCConfiguration__n_lattice_sites,
                         len(lattice.sites()))
        # Check query function.
        self.assertEqual(len(sitesmap.sites()), len(lattice.sites()))

        # Construct again, now with a list of possible types.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               default_type=default_type,
                               possible_types=['aa', 'a', 'h', 'g'])

        # Check that the possible types are what we expect.
        self.assertEqual(set(['aa', 'a', 'g', 'h', '*']),
                         set(sitesmap._KMCConfiguration__possible_types))
        # Check query function.
        self.assertEqual(set(['aa', 'a', 'g', 'h', '*']),
                         set(sitesmap.possibleTypes()))

if __name__ == '__main__':
    unittest.main()
