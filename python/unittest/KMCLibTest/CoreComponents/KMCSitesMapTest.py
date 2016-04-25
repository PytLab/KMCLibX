"""" Module for testing KMCSitesMap """

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

        # {{{
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

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a', 'c', 'b'])

        # Check that the types are what we expect.
        self.assertListEqual(types, sitesmap._KMCConfiguration__types)
        # Check query function.
        self.assertListEqual(types, sitesmap.types())

        # Check that the possible types are what we expect.
        self.assertSetEqual(set(['a', 'c', 'b', '*']),
                            set(sitesmap._KMCConfiguration__possible_types.keys()))
        # Check query function.
        self.assertSetEqual(set(['a', 'c', 'b', '*']),
                            set(sitesmap.possibleTypes().keys()))

        # Check that the number of lattice sites corresponds to the lattice.
        self.assertEqual(sitesmap._KMCConfiguration__n_lattice_sites,
                         len(lattice.sites()))

        # Check that the lattice site can be returned from the sitesmap.
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
        self.assertSetEqual(set(['a', 'b', '*']),
                            set(sitesmap._KMCConfiguration__possible_types.keys()))
        # }}}

    def testConstructionLongFormat(self):
        """ Test that the KMCSitesMap class can be constructed with the long types format. """
        # {{{
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

        # Setup the sitesmap.
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
        self.assertListEqual(ref_types, sitesmap._KMCConfiguration__types)
        # Check query function.
        self.assertListEqual(ref_types, sitesmap.types())

        # Check that the possible types are what we expect.
        self.assertSetEqual(set(['a','g','h','*']),
                            set(sitesmap._KMCConfiguration__possible_types))
        # Check query function.
        self.assertSetEqual(set(['a','g','h','*']),
                            set(sitesmap.possibleTypes()))

        # Check that the number of lattice sites corresponds
        # to the lattice.
        self.assertEqual(sitesmap._KMCConfiguration__n_lattice_sites,
                         len(lattice.sites()))
        # Check query function.
        self.assertEqual(len(sitesmap.sites()), len(lattice.sites()))

        # Check the empty oveloaded functions.
        self.assertIs(sitesmap.atomIDTypes(), None)
        self.assertIs(sitesmap.atomIDCoordinates(), None)
        self.assertIs(sitesmap.movedAtomIDs(), None)


        # Construct again, now with a list of possible types.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               default_type=default_type,
                               possible_types=['aa', 'a', 'h', 'g'])

        # Check that the possible types are what we expect.
        self.assertSetEqual(set(['aa', 'a', 'g', 'h', '*']),
                            set(sitesmap._KMCConfiguration__possible_types))
        # Check query function.
        self.assertSetEqual(set(['aa', 'a', 'g', 'h', '*']),
                            set(sitesmap.possibleTypes()))

        # }}}

    def testConstructionFailShort(self):
        """ Test every thing that should fail when using the short format. """

        # {{{
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

        types = ['g', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'h']

        default_type = 'a'


        # This fails because the lattice is of wrong type.
        self.assertRaises(Error, KMCSitesMap, lattice=unit_cell, types=types)


        # This fails because there is a default type given.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types,
                          default_type=default_type)

        # This fails becaulse of wrong length of the types.
        types_1 = ['g', 'a', 'a']
        self.assertRaises(Error, KMCSitesMap, lattice=lattice, types=types_1)

        types_1 = ['g', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 3, 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                   'a', 'a', 'a', 'a', 'a', 'a', 'a', 1]

        # This fails because of the wrong type of the types information.
        self.assertRaises(Error, KMCSitesMap, lattice=lattice, types=types_1)

        types_1 = "ABCDE"
        # And this also.
        self.assertRaises(Error, KMCSitesMap, lattice=lattice, types=types_1)

        # Use an incompatible possible types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types,
                          possible_types=['a'])

        # Use an incompatible possible types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types,
                          possible_types="ABCD")

        # Use a possible types list with a wildcard.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types,
                          possible_types=['g','a','h','*'])
        # }}}

    def testConstructionFailLongFormat(self):
        """ Test everything that can go wrong when constructing with the long fomat. """

        # {{{
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

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                             types=types,
                             default_type=default_type)


        # This fails because the lattice is of wrong type.
        self.assertRaises(Error, KMCSitesMap, 
                          lattice=unit_cell,
                          types=types,
                          default_type=default_type)

        # This fails because no default type is given.
        self.assertRaises(Error, KMCSitesMap, lattice=lattice, types=types)

        # This fails because no default type is not a string.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types,
                          default_type=['a'])

        # This fails because the possible types are not set correctly.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice, types=types,
                          default_type=default_type,
                          possible_types="agh")

        # This fails because there are types missing in the possible types.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice, types=types,
                          default_type=default_type,
                          possible_types=['a','h'])

        # This fails because there is a wildcard character in the possible types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice, types=types,
                          default_type=default_type,
                          possible_types=['g','a','*','h'])

        types_1 = [123, 345]
        # This fails because of wrong type in the types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice, types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 0, 'a'), (0, 0, 0, 'a')]
        # This fails because of wrong type in the types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 0, 'a'), (0, 0, 0, 1, 'b', 3)]
        # This fails because of wrong type in the types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [[0, 0, 0, 0, 'a'], [0, 0, 0, 1, 'b']]
        # This fails because of wrong type in the types list.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 0, 'a'), (-1, 0, 0, 0, 'b')]
        # This fails because of a negative first index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(100, 0, 0, 0, 'a'), (0, 0, 0, 0, 'b')]
        # This fails because of a too large firs index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 0, 'a'), (0, -1, 0, 0, 'b')]
        # This fails because of a negative second index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 100, 0, 0, 'a'), (0, 0, 0, 1, 'b')]
        # This fails because of a too large second index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type) 
        types_1 = [(0, 0, 0, 0, 'a'), (0, 0, -1, 0, 'b')]
        # This fails because of a negative third index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 100, 0, 'a'), (0, 0, 0, 0, 'b')]
        # This fails because of a too large third index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 0, 'a'), (0, 0, 0, -1, 'b')]
        # This fails because of a negative fourth index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)

        types_1 = [(0, 0, 0, 100, 'a'), (0, 0, 0, 1, 'b')]
        # This fails because of a too large fourth index.
        self.assertRaises(Error, KMCSitesMap,
                          lattice=lattice,
                          types=types_1,
                          default_type=default_type)
        # }}}

    def testLatticeMap(self):
        """ Make sure the lattice map we get correspond to the lattice we give. """
        # {{{
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

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a', 'c', 'b'])
                                                            
                                                            
        # Get the lattice map.                              
        cpp_lattice_map = sitesmap._latticeMap()              
                                                            
        # Get the map from the lattice.                     
        cpp_lattice_map_ref = lattice._map()                

        # Check that these two are references to the same underlying object.
        self.assertTrue(cpp_lattice_map == cpp_lattice_map_ref)

        # }}}

    def testBackend(self):
        """ Make sure the C++ backend is what we expect. """
        # {{{
        # Setup a valid KMCUnitCell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8,0.0,0.0],
                                                          [0.0,3.2,0.0],
                                                          [0.0,0.5,3.0]]),
                                basis_points=[[0.0,0.0,0.0],
                                              [0.5,0.5,0.5],
                                              [0.25,0.25,0.75]])

        # Setup the lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4,4,1),
                             periodic=(True,True,False))

        types = ['a','a','a','a','b','b',
                 'a','a','a','b','b','b',
                 'b','b','a','a','b','a',
                 'b','b','b','a','b','a',
                 'b','a','a','a','b','b',
                 'b','b','b','b','b','b',
                 'a','a','a','a','b','b',
                 'b','b','a','b','b','a']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a','c','b'])

        # Make sure that the backend stored on the sitesmap is None.
        self.assertTrue(sitesmap._KMCConfiguration__backend is None)

        # Query for the backend.
        cpp_backend = sitesmap._backend()

        # Check that the backend on the class is returned.
        self.assertTrue(sitesmap._KMCConfiguration__backend == cpp_backend)

        # Check the type of the cpp backend.
        self.assertTrue(isinstance(cpp_backend, Backend.SitesMap))
        # }}}

    def testScript(self):
        """ Test that we can generate a valid script. """
        # [{{
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
                 'a', 'a', 'a', 'b', 'b',  'b',
                 'b', 'b', 'a', 'a', 'b',  'a',
                 'b', 'b', 'b', 'a', 'b',  'a',
                 'b', 'a', 'a', 'a', 'b',  'b',
                 'b', 'b', 'b', 'b', 'b',  'b',
                 'a', 'a', 'a', 'a', 'b',  'b',
                 'b', 'b', 'a', 'b', 'b',  'a']
                                           
        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                                  types=types,
                                  possible_types=['a', 'c', 'b'])

        # Get the script.
        script = sitesmap._script()

        ref_script = """
# -----------------------------------------------------------------------------
# Unit cell

cell_vectors = [[   2.800000e+00,   0.000000e+00,   0.000000e+00],
                [   0.000000e+00,   3.200000e+00,   0.000000e+00],
                [   0.000000e+00,   5.000000e-01,   3.000000e+00]]

basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
                [   5.000000e-01,   5.000000e-01,   5.000000e-01],
                [   2.500000e-01,   2.500000e-01,   7.500000e-01]]

unit_cell = KMCUnitCell(
    cell_vectors=cell_vectors,
    basis_points=basis_points)

# -----------------------------------------------------------------------------
# Lattice

lattice = KMCLattice(
    unit_cell=unit_cell,
    repetitions=(4,4,1),
    periodic=(True, True, False))

# -----------------------------------------------------------------------------
# SitesMap

types = ['a','a','a','a','b','b','a','a','a','b','b','b','b',
         'b','a','a','b','a','b','b','b','a','b','a','b','a',
         'a','a','b','b','b','b','b','b','b','b','a','a','a',
         'a','b','b','b','b','a','b','b','a']

possible_types = ['a','c','b']

sitesmap = KMCSitesMap(
    lattice=lattice,
    types=types,
    possible_types=possible_types)
"""
        self.assertEqual(script, ref_script)

        # Try another one.

        # Setup a valid KMCUnitCell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8, 0.0, 0.0],
                                                          [0.0, 3.2, 0.0],
                                                          [0.0, 0.5, 3.0]]),
                                basis_points=[[0.0, 0.0, 0.0]])

        # Setup the lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(1, 1, 1),
                             periodic=(False, False, False))

        types = ['ThisIsTheTypeOfMyOnlySiteInThisUnrealisticallyShortStructure']

        possible_types = ['ThisIsTheTypeOfMyOnlySiteInThisUnrealisticallyShortStructure', "A", "B", "CDEFGHI"]

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                                  types=types,
                                  possible_types=possible_types)

        # Get the script.
        script = sitesmap._script(variable_name="sitesmap")

        ref_script = """
# -----------------------------------------------------------------------------
# Unit cell

cell_vectors = [[   2.800000e+00,   0.000000e+00,   0.000000e+00],
                [   0.000000e+00,   3.200000e+00,   0.000000e+00],
                [   0.000000e+00,   5.000000e-01,   3.000000e+00]]

basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

unit_cell = KMCUnitCell(
    cell_vectors=cell_vectors,
    basis_points=basis_points)

# -----------------------------------------------------------------------------
# Lattice

lattice = KMCLattice(
    unit_cell=unit_cell,
    repetitions=(1,1,1),
    periodic=(False, False, False))

# -----------------------------------------------------------------------------
# SitesMap

types = ['ThisIsTheTypeOfMyOnlySiteInThisUnrealisticallyShortStructure']

possible_types = ['A','CDEFGHI','ThisIsTheTypeOfMyOnlySiteInThisUnrealisticallyShortStructure',
                  'B']

sitesmap = KMCSitesMap(
    lattice=lattice,
    types=types,
    possible_types=possible_types)
"""
        self.assertEqual(script, ref_script)
        # }}}

if __name__ == '__main__':
    unittest.main()
