""" Module for testing the KMCLatticeModel class. """


# Copyright (c)  2012-2014  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest
import numpy
import sys
import os
from copy import deepcopy

from KMCLib.CoreComponents.KMCInteractions import KMCInteractions
from KMCLib.CoreComponents.KMCProcess import KMCProcess
from KMCLib.CoreComponents.KMCConfiguration import KMCConfiguration
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap
from KMCLib.CoreComponents.KMCControlParameters import KMCControlParameters
from KMCLib.CoreComponents.KMCUnitCell import KMCUnitCell
from KMCLib.CoreComponents.KMCLattice import KMCLattice
from KMCLib.PluginInterfaces.KMCAnalysisPlugin import KMCAnalysisPlugin
from KMCLib.Exceptions.Error import Error
from KMCLib.Backend.Backend import MPICommons

# Import from the module we test.
from KMCLib.CoreComponents.KMCLatticeModel import KMCLatticeModel

# Test helpers.
sys.path.insert(0, os.path.join(os.path.abspath(os.path.dirname(__file__)), ".."))
from TestUtilities.Plugins.CustomRateCalculator.CustomRateCalculator import CustomRateCalculator


# Implement the test.
class KMCLatticeModelTest(unittest.TestCase):
    """ Class for testing the KMCLatticeModel class. """

    def setUp(self):
        """ The setUp method for test fixtures. """
        self.__files_to_remove = []

    def tearDown(self):
        """ The tearDown method for test fixtures. """
        for f in self.__files_to_remove:
            if MPICommons.isMaster():
                os.remove(f)
        MPICommons.barrier()

    def testConstruction(self):
        """ Test the construction of the lattice model """
        # {{{
        # Setup a unitcell.
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

        types = ['A','A','A','A','B','B',
                 'A','A','A','B','B','B',
                 'B','B','A','A','B','A',
                 'B','B','B','A','B','A',
                 'B','A','A','A','B','B',
                 'B','B','B','B','B','B',
                 'A','A','A','A','B','B',
                 'B','B','A','B','B','A']

        site_types = ['a','a','a','a','b','b',
                      'a','a','a','b','b','b',
                      'b','b','a','a','b','a',
                      'b','b','b','a','b','a',
                      'b','a','a','a','b','b',
                      'b','b','b','b','b','b',
                      'a','a','a','a','b','b',
                      'b','b','a','b','b','a']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=['a','c','b'])

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A','C','B'])

        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        sites  = [0,1,2]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","C"]
        types1 = ["C","A"]
        sites  = [0,1,2]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # Construct the interactions object.
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes=processes)

        # Construct the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Check that it has the attribute _backend which is None
        self.assertTrue(hasattr(model,"_KMCLatticeModel__backend"))
        self.assertTrue(model._KMCLatticeModel__backend is None)

        # Check that it has the correct interactions stored.
        self.assertTrue(model._KMCLatticeModel__interactions == interactions)

        # Check that it has the correct configuration stored.
        self.assertTrue(model._KMCLatticeModel__configuration == config)
        # }}}

    def testConstructionWithSiteTypes(self):
        """ Test the construction of the lattice model with site types provided. """
        # {{{
        # Setup a unitcell.
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

        types = ['A','A','A','A','B','B',
                 'A','A','A','B','B','B',
                 'B','B','A','A','B','A',
                 'B','B','B','A','B','A',
                 'B','A','A','A','B','B',
                 'B','B','B','B','B','B',
                 'A','A','A','A','B','B',
                 'B','B','A','B','B','A']

        site_types = ['a','a','a','a','b','b',
                      'a','a','a','b','b','b',
                      'b','b','a','a','b','a',
                      'b','b','b','a','b','a',
                      'b','a','a','a','b','b',
                      'b','b','b','b','b','b',
                      'a','a','a','a','b','b',
                      'b','b','a','b','b','a']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=['a','c','b'])

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A','C','B'])

        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        site_types = ["a", "b"]
        sites  = [0,1,2]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               site_types=site_types,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","C"]
        types1 = ["C","A"]
        site_types = ["a", "b"]
        sites  = [0,1,2]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords,
                               types0,
                               types1,
                               site_types=site_types,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # Construct the interactions object.
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)

        # Construct the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Check that it has the attribute _backend which is None
        self.assertTrue(hasattr(model,"_KMCLatticeModel__backend"))
        self.assertTrue(model._KMCLatticeModel__backend is None)

        # Check that it has the correct interactions stored.
        self.assertTrue(model._KMCLatticeModel__interactions == interactions)

        # Check that it has the correct configuration stored.
        self.assertTrue(model._KMCLatticeModel__configuration == config)
        # }}}


    def testRunImplicitWildcards(self):
        """ Test that ta valid model can run for a few steps. """
        # {{{
        # Setup a unitcell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[1.0,0.0,0.0],
                                                          [0.0,1.0,0.0],
                                                          [0.0,0.0,1.0]]),
                                basis_points=[[0.0,0.0,0.0]])
        # And a lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(10,10,1),
                             periodic=(True,True,False))

        # Set the stating configuration types.
        types = ['B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'A', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'A', 'B', 'B', 'B', 'B', 'B', 'A', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'A', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'A', 'B', 'B',
                 'A', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'A']

        site_types = ['b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'a', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'a', 'b', 'b', 'b', 'b', 'b', 'a', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'a', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'a', 'b', 'b',
                      'a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'a']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=["a", "b"])
        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=["A","B"])

        # Generate the interactions with a distance so large that we get a
        # layer of implicite wildcards in the C++ matchlists.
        sites = [0]
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
                       [   1.000000e+00,   0.000000e+00,   0.000000e+00],
                       [  -1.000000e+00,   0.000000e+00,   0.000000e+00],
                       [   0.000000e+00,  -1.000000e+00,   0.000000e+00],
                       [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                       [   2.000000e+00,   2.000000e+00,   0.000000e+00]]

        types0 = ['A', 'B', 'B', 'B', 'B', 'A']
        types1 = ['B', 'B', 'A', 'B', 'B', 'A']
        process_0 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'A', 'B', 'B', 'B']
        process_1 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'B', 'A', 'B', 'B']
        process_2 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'B', 'B', 'A', 'B']
        process_3 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        # Processes.
        processes = [process_0,
                     process_1,
                     process_2,
                     process_3]

        # No implicit wildcards.
        interactions = KMCInteractions(processes=processes,
                                       implicit_wildcards=False)

        # Create the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Get the match types out.
        matchlist = model._backend(0.0).interactions().processes()[0].matchList()
        match_types = [l.match_type for l in matchlist]

        # This does not have wildcards added.
        ref_match_types = [1, 2, 2, 2, 2, 1]
        self.assertEqual(match_types, ref_match_types)

        # Create with implicit wildcards - this is default behavior.
        interactions = KMCInteractions(processes=processes)

        # Create the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Check the process matchlists again.
        matchlist = model._backend(0.0).interactions().processes()[0].matchList()
        match_types = [l.match_type for l in matchlist]

        ref_match_types = [1, 2, 2, 2, 2,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 1]

        # This one has the wildcards (zeroes) added.
        self.assertEqual(match_types, ref_match_types)

        # Setup the run paramters.
        control_parameters = KMCControlParameters(number_of_steps=10,
                                                  dump_interval=1)

        # Check picked index before running.
        self.assertEqual(interactions.pickedIndex(), -1)

        # Check process available sites list after constructing model.
        self.assertNotEqual(interactions.processAvailableSites(), (0, 0, 0, 0))

        # Run model.
        model.run(control_parameters)

        # Check picked index after running.
        self.assertNotEqual(interactions.pickedIndex(), -1)
        # }}}

    def testInteractionQueryFunctions(self):
        """ Test that ta valid model can run for a few steps. """
        # {{{
        # Setup a unitcell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[1.0, 0.0, 0.0],
                                                          [0.0, 1.0, 0.0],
                                                          [0.0, 0.0, 1.0]]),
                                basis_points=[[0.0, 0.0, 0.0]])
        # And a lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(10, 1, 1),
                             periodic=(True, True, False))

        # Set the stating configuration types.
        types = ['A', 'B', 'C', 'D', 'B', 'B', 'B', 'B', 'B', 'B']

        site_types = ['b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=["a", "b"])
        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=["A", "B", "C", "D"])

        # Generate the interactions with a distance so large that we get a
        # layer of implicite wildcards in the C++ matchlists.
        sites = [0]
        coordinates = [[0.0, 0.0, 0.0],
                       [1.0, 0.0, 0.0]]

        types0 = ['A', 'B']
        types1 = ['B', 'B']
        process_0 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['B', 'C']
        types1 = ['B', 'B']
        process_1 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['C', 'D']
        types1 = ['B', 'B']
        process_2 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        types0 = ['D', 'A']
        types1 = ['A', 'D']
        process_3 = KMCProcess(coordinates, types0, types1, None, sites, 1.0)

        # Processes.
        processes = [process_0,
                     process_1,
                     process_2,
                     process_3]

        # No implicit wildcards.
        interactions = KMCInteractions(processes=processes,
                                       implicit_wildcards=True)

        # Create the model.
        model = KMCLatticeModel(config, sitesmap, interactions)
        cpp_model = model._backend(start_time=0.0)

        # Check attributes.
        self.assertEqual(interactions.pickedIndex(), -1)
        self.assertTupleEqual(interactions.processAvailableSites(), (1, 1, 1, 0))
        self.assertListEqual(interactions.processRates(), [1.0, 1.0, 1.0, 1.0])

        # }}}

    def testRunImplicitWildcardsWithSiteTypes(self):
        """ Test that ta valid model can run for a few steps with sites types provided. """
        # {{{
        # Setup a unitcell.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[1.0,0.0,0.0],
                                                          [0.0,1.0,0.0],
                                                          [0.0,0.0,1.0]]),
                                basis_points=[[0.0,0.0,0.0]])
        # And a lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(10,10,1),
                             periodic=(True,True,False))

        # Set the stating configuration types.
        types = ['B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'A', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B']

        site_types = ['b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'a', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b']

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=["a", "b"])
        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=["A", "B"])

        # Generate the interactions with a distance so large that we get a
        # layer of implicite wildcards in the C++ matchlists.
        sites = [0]
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
                       [   1.000000e+00,   0.000000e+00,   0.000000e+00],
                       [  -1.000000e+00,   0.000000e+00,   0.000000e+00],
                       [   0.000000e+00,  -1.000000e+00,   0.000000e+00],
                       [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                       [   2.000000e+00,   2.000000e+00,   0.000000e+00]]

        site_types = ["a", "b", "b", "b", "b", "b"]

        types0 = ['A', 'B', 'B', 'B', 'B', 'A']
        types1 = ['B', 'B', 'A', 'B', 'B', 'A']
        process_0 = KMCProcess(coordinates, types0, types1, None, sites, 1.0, site_types)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'A', 'B', 'B', 'B']
        process_1 = KMCProcess(coordinates, types0, types1, None, sites, 1.0, site_types)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'B', 'A', 'B', 'B']
        process_2 = KMCProcess(coordinates, types0, types1, None, sites, 1.0, site_types)

        types0 = ['A', 'B', 'B', 'B', 'B', 'B']
        types1 = ['B', 'B', 'B', 'B', 'A', 'B']
        site_types3 = ["*", "*", "*", "*", "*", "*"]
        process_3 = KMCProcess(coordinates, types0, types1, None, sites, 1.0, site_types3)

        # Processes.
        processes = [process_0,
                     process_1,
                     process_2,
                     process_3]

        # No implicit wildcards.
        interactions = KMCInteractions(processes=processes,
                                       sitesmap=sitesmap,
                                       implicit_wildcards=False)

        # Create the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Get the match types out.
        matchlist = model._backend(0.0).interactions().processes()[0].matchList()
        match_types = [l.match_type for l in matchlist]
        ret_site_types = [l.site_type for l in matchlist]

        # This does not have wildcards added.
        ref_match_types = [1, 2, 2, 2, 2, 1]
        self.assertEqual(match_types, ref_match_types)

        # This does not have wildcatds added.
        ref_site_types = [1, 2, 2, 2, 2, 2]
        #self.assertListEqual(ref_site_types, ret_site_types)

        # Create with implicit wildcards - this is default behavior.
        interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)

        # Create the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Check the process matchlists again.
        matchlist = model._backend(0.0).interactions().processes()[0].matchList()
        match_types = [l.match_type for l in matchlist]
        ret_site_types = [l.site_type for l in matchlist]

        ref_match_types = [1, 2, 2, 2, 2,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 1]

        ref_site_types = [1, 2, 2, 2, 2,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 2]

        # This one has the wildcards (zeroes) added.
        self.assertEqual(match_types, ref_match_types)

        # This one has the wildcards (zeroes) added.
        #self.assertEqual(ref_site_types, ret_site_types)

        # Setup the run paramters.
        control_parameters = KMCControlParameters(number_of_steps=10,
                                                  dump_interval=1)
        model.run(control_parameters)
        # }}}

    def testRun2(self):
        """ Test the run of an A-B flip model. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  seed=2013)

        # Run the model for 1000 steps.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename)

        # Read the first last frames from the trajectory file and check that
        # the fraction of A is close to 20% in the last, and 0 in the first.
        if MPICommons.isMaster():
            global_dict = {}
            local_dict  = {}
            exec(open(trajectory_filename, 'rb').read(), global_dict, local_dict)

            # Count the first frame.
            elem = local_dict["types"][0]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])
            self.assertEqual(nA, 0)
            self.assertEqual(nB, 100)

            # Count the last frame.
            elem = local_dict["types"][-1]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])

            # Note that the average should be 20.0% over a long run.
            # It is pure luck that it is exact at this particular
            # step with the presently used random number seed.
            fraction =  nA * 100.0 / (nA + nB)
            target = 20.0
            self.assertAlmostEqual(fraction, target, 3)
        # }}}

    def testRunTimeNotZero(self):
        """ Test the run with start time not equal to 0.0 """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  seed=2013,
                                                  start_time=1.2345e-1)

        # Run the model for 1000 steps.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename)
        # }}}

    def testRun2WithSiteTypes(self):
        """ Test the run of an A-B flip model with site types provided. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0,
                               site_types=['b'])
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0,
                               site_types=['b'])

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes, sitesmap)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  seed=2013)

        # Run the model for 1000 steps.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename)

        # Read the first last frames from the trajectory file and check that
        # the fraction of A is close to 20% in the last, and 0 in the first.
        if MPICommons.isMaster():
            global_dict = {}
            local_dict  = {}
            exec(open(trajectory_filename).read(), global_dict, local_dict)

            # Count the first frame.
            elem = local_dict["types"][0]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])
            self.assertEqual(nA, 0)
            self.assertEqual(nB, 100)

            # Count the last frame.
            elem = local_dict["types"][-1]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])

            # Note that the average should be 20.0% over a long run.
            # It is pure luck that it is exact at this particular
            # step with the presently used random number seed.
            fraction =  nA * 100.0 / (nA + nB)
            target = 20.0
            self.assertAlmostEqual(fraction, target, 3)
        # }}}

    def testCustomRatesRun(self):
        """ Test the run of an A-B flip model with custom rates. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates, ['A'], ['B'], None, [0], 4.0)
        process_1 = KMCProcess(coordinates, ['B'], ['A'], None, [0], 1.0)
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes,
                                       implicit_wildcards=True)

        # Custom rates.
        rate_calculator = CustomRateCalculator
        interactions.setRateCalculator(rate_calculator)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj_custom.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  seed=2013)

        # Run the model for 1000 steps.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename)

        # Read the first last frames from the trajectory file and check that
        # the fraction of A is close to 75% in the last, and 0 in the first.
        if MPICommons.isMaster():
            global_dict = {}
            local_dict  = {}
            exec(open(trajectory_filename).read(), global_dict, local_dict)

            # Count the first frame.
            elem = local_dict["types"][0]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])
            self.assertEqual(nA, 0)
            self.assertEqual(nB, 100)

            # Count the last frame.
            elem = local_dict["types"][-1]
            nA = len([ ee for ee in elem if ee == "A" ])
            nB = len([ ee for ee in elem if ee == "B" ])

            # Note that the average over a long simulation should be
            # 75% A using the modified rate function. In this particular
            # step the A population is 74%.
            value = 1.0 * nA / (nA + nB)
            self.assertAlmostEqual(0.74, value, 2)
        # }}}

    def testRunWithAnalysis(self):
        """ Test that the analyis plugins get called correctly. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  analysis_interval=300)


        # Setup a valid minimal analysis object.
        class AnalysisProxy1(KMCAnalysisPlugin):
            def __init__(self):
                pass

        # Setup a slightly larger analyis object.
        class AnalysisProxy2(KMCAnalysisPlugin):
            def __init__(self):
                self.setup_called = False
                self.finalize_called = False
                self.register_step_counts = 0

            def setup(self, step, time, configuration, interactions=None):
                self.setup_called = True

            def registerStep(self, step, time, configuration, interactions=None):
                self.register_step_counts += 1

            def finalize(self):
                self.finalize_called = True

        ap2 = AnalysisProxy2()
        analysis = [ AnalysisProxy1(), ap2 ]

        # Run the model for 1000 steps with the analysis objects.
        # With dump interval 500 the analysis objects should  be
        # called on startup, the at step 300, 600 and step 900 and
        # a finalization after that.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename,
                          analysis=analysis)

        self.assertTrue(ap2.setup_called)
        self.assertTrue(ap2.finalize_called)
        self.assertEqual(ap2.register_step_counts, 3)
        # }}}

    def testRunWithMultiAnalysis(self):
        """ Test that the multiple analysis plugins can get called correctly. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "ab_flip_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  analysis_interval=[300, 500])


        # Setup a valid minimal analysis object.
        class AnalysisProxy1(KMCAnalysisPlugin):
            def __init__(self):
                self.setup_called = False
                self.finalize_called = False
                self.register_step_counts = 0

            def setup(self, step, time, configuration, interactions=None):
                self.setup_called = True

            def registerStep(self, step, time, configuration, interactions=None):
                self.register_step_counts += 1

            def finalize(self):
                self.finalize_called = True

        # Setup a slightly larger analyis object.
        class AnalysisProxy2(KMCAnalysisPlugin):
            def __init__(self):
                self.setup_called = False
                self.finalize_called = False
                self.register_step_counts = 0

            def setup(self, step, time, configuration, interactions=None):
                self.setup_called = True

            def registerStep(self, step, time, configuration, interactions=None):
                self.register_step_counts += 1

            def finalize(self):
                self.finalize_called = True

        ap1 = AnalysisProxy1()
        ap2 = AnalysisProxy2()
        analysis = [ap1, ap2]

        # Run the model for 1000 steps with the analysis objects.
        # With dump interval 500 the analysis objects should  be
        # called on startup, the at step 300, 600 and step 900 and
        # a finalization after that.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=trajectory_filename,
                          analysis=analysis)

        self.assertTrue(ap1.setup_called)
        self.assertTrue(ap1.finalize_called)
        self.assertEqual(ap1.register_step_counts, 3)

        self.assertTrue(ap2.setup_called)
        self.assertTrue(ap2.finalize_called)
        self.assertEqual(ap2.register_step_counts, 2)

        # Check exception because the number of analysis and intervals are not equal.
        self.assertRaisesRegexp(Error, "^analysis intervals number",
                                lambda : ab_flip_model.run(control_parameters,
                                                           analysis=[ap1]))
        # }}}

    def testRunFailAnalysis(self):
        """ Test that the analyis plugins get called correctly. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(10,10,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*100
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*100
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500)

        # Setup a valid minimal analysis object.
        class AnalysisProxy1(KMCAnalysisPlugin):
            def __init__(self):
                pass

        # Fail because of not instantitated analysis.
        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      analysis=[AnalysisProxy1]) )
        # Fail because of empty analyis list.
        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      analysis=[]) )
        # Fail because of not list.
        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      analysis=AnalysisProxy1()) )
        # Fail because of wrong type.
        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      analysis=[AnalysisProxy1(), "AP3"]) )
        # }}}

    def testRunTrajectory(self):
        """ Test the run of an A-B flip model with trajectory output. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(4,4,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*16
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*16
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Construct the trajectory fileames.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")

        lattice_trajectory_filename = str(os.path.join(name, "ab_flip_traj_lattice.py"))
        xyz_trajectory_filename = str(os.path.join(name, "ab_flip_traj_xyz.xyz"))

        self.__files_to_remove.append(lattice_trajectory_filename)
        self.__files_to_remove.append(xyz_trajectory_filename)

        # The control parameters.
        control_parameters = KMCControlParameters(number_of_steps=1000,
                                                  dump_interval=500,
                                                  seed=2013)

        # Run the model for 1000 steps with a lattice trajectory.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=lattice_trajectory_filename,
                          trajectory_type='lattice')

        # Check the file content.
        ref_lattice = """
sites=[[       0.000000,       0.000000,       0.000000],
       [       0.000000,       1.000000,       0.000000],
       [       0.000000,       2.000000,       0.000000],
       [       0.000000,       3.000000,       0.000000],
       [       1.000000,       0.000000,       0.000000],
       [       1.000000,       1.000000,       0.000000],
       [       1.000000,       2.000000,       0.000000],
       [       1.000000,       3.000000,       0.000000],
       [       2.000000,       0.000000,       0.000000],
       [       2.000000,       1.000000,       0.000000],
       [       2.000000,       2.000000,       0.000000],
       [       2.000000,       3.000000,       0.000000],
       [       3.000000,       0.000000,       0.000000],
       [       3.000000,       1.000000,       0.000000],
       [       3.000000,       2.000000,       0.000000],
       [       3.000000,       3.000000,       0.000000]]
times=[]
steps=[]
types=[]
times.append(  0.0000000000e+00)
steps.append(0)
types.append(["B","B","B","B","B","B","B","B","B","B","B","B","B","B","B","B"])
times.append(  1.9093684175e+01)
steps.append(500)
types.append(["B","A","B","B","A","B","B","B","A","B","B","B","B","B","B","A"])
times.append(  4.0622006972e+01)
steps.append(1000)
types.append(["B","A","A","B","B","B","B","A","B","B","B","A","B","B","B","B"])
"""

        with open(lattice_trajectory_filename, "r") as t:
            lattice_data = t.read()

        # Check with "in" to avoid comparing dates.
        self.assertTrue(ref_lattice in lattice_data)

        # Run the model for 1000 steps with an xyz trajectory.
        ab_flip_model.run(control_parameters,
                          trajectory_filename=xyz_trajectory_filename,
                          trajectory_type='xyz')


        ref_xyz = """KMCLib XYZ FORMAT VERSION 2013.10.15

CELL VECTORS
a: 1.0000000000e+00 0.0000000000e+00 0.0000000000e+00
b: 0.0000000000e+00 1.0000000000e+00 0.0000000000e+00
c: 0.0000000000e+00 0.0000000000e+00 1.0000000000e+00

REPETITIONS 4 1 1

PERIODICITY True True False

STEP 0
          16
    TIME 4.0622006972e+01
                B   0.0000000000e+00 0.0000000000e+00 0.0000000000e+00  0
                A   0.0000000000e+00 1.0000000000e+00 0.0000000000e+00  1
                A   0.0000000000e+00 2.0000000000e+00 0.0000000000e+00  2
                B   0.0000000000e+00 3.0000000000e+00 0.0000000000e+00  3
                B   1.0000000000e+00 0.0000000000e+00 0.0000000000e+00  4
                B   1.0000000000e+00 1.0000000000e+00 0.0000000000e+00  5
                B   1.0000000000e+00 2.0000000000e+00 0.0000000000e+00  6
                A   1.0000000000e+00 3.0000000000e+00 0.0000000000e+00  7
                B   2.0000000000e+00 0.0000000000e+00 0.0000000000e+00  8
                B   2.0000000000e+00 1.0000000000e+00 0.0000000000e+00  9
                B   2.0000000000e+00 2.0000000000e+00 0.0000000000e+00  10
                A   2.0000000000e+00 3.0000000000e+00 0.0000000000e+00  11
                B   3.0000000000e+00 0.0000000000e+00 0.0000000000e+00  12
                B   3.0000000000e+00 1.0000000000e+00 0.0000000000e+00  13
                B   3.0000000000e+00 2.0000000000e+00 0.0000000000e+00  14
                B   3.0000000000e+00 3.0000000000e+00 0.0000000000e+00  15
STEP 500
          16
    TIME 5.9660935122e+01
                B   0.0000000000e+00 0.0000000000e+00 0.0000000000e+00  0
                A   0.0000000000e+00 1.0000000000e+00 0.0000000000e+00  1
                B   0.0000000000e+00 2.0000000000e+00 0.0000000000e+00  2
                B   0.0000000000e+00 3.0000000000e+00 0.0000000000e+00  3
                B   1.0000000000e+00 0.0000000000e+00 0.0000000000e+00  4
                B   1.0000000000e+00 1.0000000000e+00 0.0000000000e+00  5
                B   1.0000000000e+00 2.0000000000e+00 0.0000000000e+00  6
                A   1.0000000000e+00 3.0000000000e+00 0.0000000000e+00  7
                B   2.0000000000e+00 0.0000000000e+00 0.0000000000e+00  8
                B   2.0000000000e+00 1.0000000000e+00 0.0000000000e+00  9
                B   2.0000000000e+00 2.0000000000e+00 0.0000000000e+00  10
                B   2.0000000000e+00 3.0000000000e+00 0.0000000000e+00  11
                A   3.0000000000e+00 0.0000000000e+00 0.0000000000e+00  12
                A   3.0000000000e+00 1.0000000000e+00 0.0000000000e+00  13
                B   3.0000000000e+00 2.0000000000e+00 0.0000000000e+00  14
                B   3.0000000000e+00 3.0000000000e+00 0.0000000000e+00  15
STEP 1000
          16
    TIME 8.1189257919e+01
                A   0.0000000000e+00 0.0000000000e+00 0.0000000000e+00  0
                B   0.0000000000e+00 1.0000000000e+00 0.0000000000e+00  1
                B   0.0000000000e+00 2.0000000000e+00 0.0000000000e+00  2
                B   0.0000000000e+00 3.0000000000e+00 0.0000000000e+00  3
                B   1.0000000000e+00 0.0000000000e+00 0.0000000000e+00  4
                A   1.0000000000e+00 1.0000000000e+00 0.0000000000e+00  5
                B   1.0000000000e+00 2.0000000000e+00 0.0000000000e+00  6
                B   1.0000000000e+00 3.0000000000e+00 0.0000000000e+00  7
                B   2.0000000000e+00 0.0000000000e+00 0.0000000000e+00  8
                B   2.0000000000e+00 1.0000000000e+00 0.0000000000e+00  9
                A   2.0000000000e+00 2.0000000000e+00 0.0000000000e+00  10
                A   2.0000000000e+00 3.0000000000e+00 0.0000000000e+00  11
                B   3.0000000000e+00 0.0000000000e+00 0.0000000000e+00  12
                B   3.0000000000e+00 1.0000000000e+00 0.0000000000e+00  13
                B   3.0000000000e+00 2.0000000000e+00 0.0000000000e+00  14
                B   3.0000000000e+00 3.0000000000e+00 0.0000000000e+00  15
"""

        with open(xyz_trajectory_filename, "r") as t:
            xyz_data = t.read()

        self.assertEqual(ref_xyz, xyz_data)

        # Running with wrong trajectory_type input fails.
        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      trajectory_filename=xyz_trajectory_filename,
                                                      trajectory_type='abc') )

        self.assertRaises( Error,
                           lambda : ab_flip_model.run(control_parameters,
                                                      trajectory_filename=xyz_trajectory_filename,
                                                      trajectory_type=123) )
        # }}}

    def testRunRngType(self):
        """ Test that it is possible to run with each of the supported PRNG:s. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(4,4,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*16
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*16
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the models.
        ab_flip_model_MT       = KMCLatticeModel(configuration, sitesmap, interactions)
        ab_flip_model_RANLUX24 = KMCLatticeModel(configuration, sitesmap, interactions)
        ab_flip_model_RANLUX48 = KMCLatticeModel(configuration, sitesmap, interactions)
        ab_flip_model_MINSTD   = KMCLatticeModel(configuration, sitesmap, interactions)
        ab_flip_model_DEVICE   = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model for 10000 steps with MT.
        ab_flip_model_MT.run(KMCControlParameters(number_of_steps=10000,
                                                  dump_interval=5000,
                                                  seed=2013,
                                                  rng_type="MT"))

        # Get the simulation time out.
        t_MT = ab_flip_model_MT._KMCLatticeModel__cpp_timer.simulationTime()

        # Run the model for 10000 steps with RANLUX24.
        ab_flip_model_RANLUX24.run(KMCControlParameters(number_of_steps=10000,
                                                        dump_interval=5000,
                                                        seed=2013,
                                                        rng_type="RANLUX24"))
        # Get the simulation time out.
        t_RANLUX24 = ab_flip_model_RANLUX24._KMCLatticeModel__cpp_timer.simulationTime()


        # Run the model for 10000 steps with RANLUX48.
        ab_flip_model_RANLUX48.run(KMCControlParameters(number_of_steps=10000,
                                                        dump_interval=5000,
                                                        seed=2013,
                                                        rng_type="RANLUX48"))
        # Get the simulation time out.
        t_RANLUX48 = ab_flip_model_RANLUX48._KMCLatticeModel__cpp_timer.simulationTime()

        # Run the model for 10000 steps with MINSTD.
        ab_flip_model_MINSTD.run(KMCControlParameters(number_of_steps=10000,
                                                      dump_interval=5000,
                                                      seed=2013,
                                                      rng_type="MINSTD"))
        # Get the simulation time out.
        t_MINSTD = ab_flip_model_MINSTD._KMCLatticeModel__cpp_timer.simulationTime()

        # These values should be simillar but not equal. Check against hardcoded values.
        self.assertAlmostEqual(t_MT,       394.569398158, 5)
        self.assertAlmostEqual(t_RANLUX24, 389.712162523, 5)
        self.assertAlmostEqual(t_RANLUX48, 390.544423280, 5)
        self.assertAlmostEqual(t_MINSTD,   384.712302086, 5)
        # }}}

    def testRunRngTypeDevice(self):
        """ Test to use the PRNG DEVICE. """
        # {{{
        # Cell.
        cell_vectors = [[   1.000000e+00,   0.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   1.000000e+00,   0.000000e+00],
                        [   0.000000e+00,   0.000000e+00,   1.000000e+00]]

        basis_points = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]

        unit_cell = KMCUnitCell(
            cell_vectors=cell_vectors,
            basis_points=basis_points)

        # Lattice.
        lattice = KMCLattice(
            unit_cell=unit_cell,
            repetitions=(4,4,1),
            periodic=(True, True, False))

        # Configuration.
        types = ['B']*16
        possible_types = ['A','B']
        configuration = KMCConfiguration(
            lattice=lattice,
            types=types,
            possible_types=possible_types)

        # Sitesmap.
        site_types = ['b']*16
        possible_site_types = ['a', 'b']
        sitesmap = KMCSitesMap(
            lattice=lattice,
            types=site_types,
            possible_types=possible_site_types)

        # Interactions.
        coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00]]
        process_0 = KMCProcess(coordinates,
                               ['A'],
                               ['B'],
                               basis_sites=[0],
                               rate_constant=4.0)
        process_1 = KMCProcess(coordinates,
                               ['B'],
                               ['A'],
                               basis_sites=[0],
                               rate_constant=1.0)

        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Setup the model.
        ab_flip_model_DEVICE   = KMCLatticeModel(configuration, sitesmap, interactions)

        support_device = False

        if (not support_device):
            # If DEVICE is not supported on your system this is the test you should run.
            self.assertRaises( Error,
                               lambda: ab_flip_model_DEVICE.run(KMCControlParameters(number_of_steps=10000,
                                                                                     dump_interval=5000,
                                                                                     seed=2013,
                                                                                     rng_type="DEVICE")))

        else:
            # If DEVICE is supported the aboove test will fail, and you should run this tests instead.

            # Run the model for 10000 steps with DEVICE.
            ab_flip_model_DEVICE.run(KMCControlParameters(number_of_steps=10000,
                                                          dump_interval=5000,
                                                          seed=2013,
                                                          rng_type="DEVICE"))
            # Get the simulation time out.
            t_DEVICE = ab_flip_model_DEVICE._KMCLatticeModel__cpp_timer.simulationTime()
            self.assertTrue(t_DEVICE < 410.0 and t_DEVICE > 370.0)
        # }}}

    def testBackend(self):
        """ Test that the backend object is correctly constructed. """
        # {{{
        # Setup a unitcell.
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

        types = ['A', 'A', 'A', 'A', 'B', 'B',
                 'A', 'A', 'A', 'B', 'B', 'B',
                 'B', 'B', 'A', 'A', 'B', 'A',
                 'B', 'B', 'B', 'A', 'B', 'A',
                 'B', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B',
                 'A', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'A', 'B', 'B', 'A']

        site_types = ['a', 'a', 'a', 'a', 'b', 'b',
                      'a', 'a', 'a', 'b', 'b', 'b',
                      'b', 'b', 'a', 'a', 'b', 'a',
                      'b', 'b', 'b', 'a', 'b', 'a',
                      'b', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b',
                      'a', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'a', 'b', 'b', 'a']

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A', 'C', 'B'])

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=['a', 'c', 'b'])

        # A first process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "B"]
        types1 = ["B", "A"]
        sites  = [0, 1, 2, 3, 4]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # A second process.
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # Construct the interactions object.
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes=processes)

        # Construct the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Get the c++ backend out.
        cpp_model = model._backend(start_time=0.0)

        # Check that this backend object is stored on the class.
        self.assertTrue(model._KMCLatticeModel__backend == cpp_model)
        # }}}

    def testBackendWithSiteTypes(self):
        """ Test that the backend object is correctly constructed with site types provided. """
        # {{{
        # Setup a unitcell.
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

        types = ['A', 'A', 'A', 'A', 'B', 'B',
                 'A', 'A', 'A', 'B', 'B', 'B',
                 'B', 'B', 'A', 'A', 'B', 'A',
                 'B', 'B', 'B', 'A', 'B', 'A',
                 'B', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B',
                 'A', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'A', 'B', 'B', 'A']

        site_types = ['a', 'a', 'a', 'a', 'b', 'b',
                      'a', 'a', 'a', 'b', 'b', 'b',
                      'b', 'b', 'a', 'a', 'b', 'a',
                      'b', 'b', 'b', 'a', 'b', 'a',
                      'b', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b',
                      'a', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'a', 'b', 'b', 'a']

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A', 'C', 'B'])

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=['a', 'c', 'b'])

        # A first process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "B"]
        types1 = ["B", "A"]
        sites  = [0, 1, 2, 3, 4]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1,
                               site_types=['a', 'b'])

        # A second process.
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1,
                               site_types=['b', 'b'])

        # Construct the interactions object.
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)

        # Construct the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Get the c++ backend out.
        cpp_model = model._backend(start_time=0.0)

        # Check that this backend object is stored on the class.
        self.assertTrue(model._KMCLatticeModel__backend == cpp_model)
        # }}}

    def testScript(self):
        """ Test that a script can be created. """
        # {{{
        # Setup a unitcell.
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

        types = ['A', 'A', 'A', 'A', 'B', 'B',
                 'A', 'A', 'A', 'B', 'B', 'B',
                 'B', 'B', 'A', 'A', 'B', 'A',
                 'B', 'B', 'B', 'A', 'B', 'A',
                 'B', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'B', 'B', 'B', 'B',
                 'A', 'A', 'A', 'A', 'B', 'B',
                 'B', 'B', 'A', 'B', 'B', 'A']

        site_types = ['a', 'a', 'a', 'a', 'b', 'b',
                      'a', 'a', 'a', 'b', 'b', 'b',
                      'b', 'b', 'a', 'a', 'b', 'a',
                      'b', 'b', 'b', 'a', 'b', 'a',
                      'b', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'b', 'b', 'b', 'b',
                      'a', 'a', 'a', 'a', 'b', 'b',
                      'b', 'b', 'a', 'b', 'b', 'a']

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A','C','B'])

        # Setup the sitesmap.
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=['a', 'c', 'b'])

        # A first process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "B"]
        types1 = ["B", "A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=[0],
                               rate_constant=rate_0_1)

        # A second process.
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=[0],
                               rate_constant=rate_0_1)

        # Construct the interactions object.
        processes = [process_0, process_1]
        interactions = KMCInteractions(processes)

        # Construct the model.
        model = KMCLatticeModel(config, sitesmap, interactions)

        # Get the script.
        script = model._script()

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
# Configuration

types = ['A','A','A','A','B','B','A','A','A','B','B','B','B',
         'B','A','A','B','A','B','B','B','A','B','A','B','A',
         'A','A','B','B','B','B','B','B','B','B','A','A','A',
         'A','B','B','B','B','A','B','B','A']

possible_types = ['A','B','C']

configuration = KMCConfiguration(
    lattice=lattice,
    types=types,
    possible_types=possible_types)

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

possible_types = ['a','b','c']

sitesmap = KMCSitesMap(
    lattice=lattice,
    types=types,
    possible_types=possible_types)

# -----------------------------------------------------------------------------
# Interactions

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','B']
elements_after  = ['B','A']
site_types = None
move_vectors    = [(  0,[   1.000000e-01,  -8.000000e-01,  -2.100000e+00]),
                   (  1,[  -1.000000e-01,   8.000000e-01,   2.100000e+00])]
basis_sites     = [0]
rate_constant   =    3.500000e+00

process_0 = KMCProcess(
    coordinates=coordinates,
    elements_before=elements_before,
    elements_after=elements_after,
    move_vectors=move_vectors,
    basis_sites=basis_sites,
    rate_constant=rate_constant,
    site_types=site_types)

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','C']
elements_after  = ['C','A']
site_types = None
move_vectors    = [(  0,[   1.000000e-01,  -8.000000e-01,  -2.100000e+00]),
                   (  1,[  -1.000000e-01,   8.000000e-01,   2.100000e+00])]
basis_sites     = [0]
rate_constant   =    1.500000e+00

process_1 = KMCProcess(
    coordinates=coordinates,
    elements_before=elements_before,
    elements_after=elements_after,
    move_vectors=move_vectors,
    basis_sites=basis_sites,
    rate_constant=rate_constant,
    site_types=site_types)

processes = [process_0,
             process_1]

interactions = KMCInteractions(
    processes=processes,
    sitesmap=None,
    implicit_wildcards=True)

# -----------------------------------------------------------------------------
# Lattice model

model = KMCLatticeModel(
    configuration=configuration,
    sitesmap=sitesmap,
    interactions=interactions)
"""
        # Check.
        self.assertEqual(script, ref_script)
        
        # }}}

    def testSplitRedistribution(self):
        " Make sure the split redistribution method can change the configuration correctly. "
        # {{{
        # Cell.
        cell_vectors = [[1.0, 0.0, 0.0],
                        [0.0, 1.0, 0.0],
                        [0.0, 0.0, 1.0]]

        basis_points = [[0.0, 0.0, 0.0],
                        [0.5, 0.5, 0.5]]

        unit_cell = KMCUnitCell(cell_vectors, basis_points)

        # Lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4, 4, 4),
                             periodic=(True, True, True))

        # Configuration.
        types = ["V"]*4*4*4*2

        types[0] = "A"
        types[1] = "B"
        types[32] = "B"
        types[2] = "A"
        types[3] = "B"

        possible_types = ["A", "B", "V"]

        configuration = KMCConfiguration(lattice=lattice,
                                         types=types,
                                         possible_types=possible_types)

        # Sitesmap.
        site_types = ["P"]*4*4*4*2
        possible_site_types = ["P"]
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=possible_site_types)

        # Interactions.
        rate = 1.0

        # A diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [0]
        process_1 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [1]
        process_2 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [0]
        process_3 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [1]
        process_4 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A + B -> V + V
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["A", "B"]
        elements_after = ["V", "V"]
        basis_sites = [0]
        process_5 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)

        # V + V -> A + B
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["V", "V"]
        elements_after = ["A", "B"]
        basis_sites = [0]
        process_6 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)
        processes = [process_1, process_2, process_3, process_4, process_5, process_6]
        interactions = KMCInteractions(processes, implicit_wildcards=True)

        # Setup the model.
        model = KMCLatticeModel(configuration, sitesmap, interactions)

        ori_types = deepcopy(model._KMCLatticeModel__configuration.types())

        # Redistribute.
        cpp_model = model._backend(start_time=0.0)
        affected_indices = cpp_model.redistribute(["V"], [], 2, 2, 2)
        new_types = deepcopy(model._KMCLatticeModel__configuration.types())

        # Check.

        # The first two elemwnts should be the same.
        self.assertListEqual(ori_types[: 2], new_types[: 2])

        # Others should be different.
        # NOTE: this would fail by chance, but that is very unlikely.
        self.assertNotEqual(ori_types[2:], new_types[2:])

        self.assertEqual(ori_types[2:].count("A"), 1)
        self.assertEqual(ori_types[2:].count("B"), 2)

        # Check the fast species before redistribution.
        self.assertEqual(4*4*4*2-2, len(affected_indices))
        ref_affected_indices = list(range(2, 4*4*4*2))
        ret_affected_indices = sorted(list(affected_indices))
        self.assertListEqual(ref_affected_indices, ret_affected_indices)

        # }}}

    def testProcessRedistribution(self):
        " Make sure the process redistribution method can change the configuration correctly. "
        # {{{
        # Cell.
        cell_vectors = [[1.0, 0.0, 0.0],
                        [0.0, 1.0, 0.0],
                        [0.0, 0.0, 1.0]]

        basis_points = [[0.0, 0.0, 0.0],
                        [0.5, 0.5, 0.5]]

        unit_cell = KMCUnitCell(cell_vectors, basis_points)

        # Lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4, 4, 4),
                             periodic=(True, True, True))

        # Configuration.
        types = ["V"]*4*4*4*2

        types[0] = "A"
        types[1] = "B"
        types[32] = "B"
        types[2] = "A"
        types[3] = "B"

        possible_types = ["A", "B", "V"]

        configuration = KMCConfiguration(lattice=lattice,
                                         types=types,
                                         possible_types=possible_types)

        # Sitesmap.
        site_types = ["P"]*4*4*4*2
        possible_site_types = ["P"]
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=possible_site_types)

        # Interactions.
        rate = 1.0

        # A diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [0]
        process_1 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [1]
        process_2 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [0]
        process_3 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [1]
        process_4 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A + B -> V + V
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["A", "B"]
        elements_after = ["V", "V"]
        basis_sites = [0]
        process_5 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)

        # V + V -> A + B
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["V", "V"]
        elements_after = ["A", "B"]
        basis_sites = [0]
        process_6 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)

        # A adsorption on basis 0.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["A"]
        basis_sites = [0]
        process_7 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="A")

        # A adsorption on basis 1.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["A"]
        basis_sites = [1]
        process_8 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="A")

        # B adsorption on basis 0.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["B"]
        basis_sites = [0]
        process_9 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="B")

        # B adsorption on basis 1.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["B"]
        basis_sites = [1]
        process_10 = KMCProcess(coordinates=coordinates,
                                elements_before=elements_before,
                                elements_after=elements_after,
                                basis_sites=basis_sites,
                                rate_constant=rate,
                                fast=True,
                                redist=True,
                                redist_species="B")

        processes = [process_1, process_2, process_3, process_4, process_5,
                     process_6, process_7, process_8, process_9, process_10]
        interactions = KMCInteractions(processes, implicit_wildcards=True)

        # Setup the model.
        model = KMCLatticeModel(configuration, sitesmap, interactions)

        ori_types = deepcopy(model._KMCLatticeModel__configuration.types())

        # Setup the model.
        cpp_model = model._backend(start_time=0.0)
        affected_indices = cpp_model.processRedistribute("V", [], [])
        new_types = deepcopy(model._KMCLatticeModel__configuration.types())

        # Check.

        # The first two elemwnts should be the same.
        self.assertListEqual(ori_types[: 2], new_types[: 2])

        # Others should be different.
        # NOTE: this would fail by chance, but that is very unlikely.
        self.assertNotEqual(ori_types[2:], new_types[2:])

        self.assertEqual(ori_types[2:].count("A"), 1)
        self.assertEqual(ori_types[2:].count("B"), 2)

        # }}}

    def testRunWithRedistribution(self):
        " Make sure the model can run with configuration redistribution. "
        # {{{
        # Cell.
        cell_vectors = [[1.0, 0.0, 0.0],
                        [0.0, 1.0, 0.0],
                        [0.0, 0.0, 1.0]]

        basis_points = [[0.0, 0.0, 0.0],
                        [0.5, 0.5, 0.5]]

        unit_cell = KMCUnitCell(cell_vectors, basis_points)

        # Lattice.
        lattice = KMCLattice(unit_cell=unit_cell,
                             repetitions=(4, 4, 4),
                             periodic=(True, True, True))

        # Configuration.
        types = ["V"]*4*4*4*2

        types[0] = "A"
        types[1] = "B"
        types[32] = "B"
        types[2] = "A"
        types[3] = "B"

        possible_types = ["A", "B", "V"]

        configuration = KMCConfiguration(lattice=lattice,
                                         types=types,
                                         possible_types=possible_types)

        # Sitesmap.
        site_types = ["P"]*4*4*4*2
        possible_site_types = ["P"]
        sitesmap = KMCSitesMap(lattice=lattice,
                               types=site_types,
                               possible_types=possible_site_types)

        # Interactions.
        rate = 1.0

        # A diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [0]
        process_1 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["A", "V"]
        elements_after = ["V", "A"]
        basis_sites = [1]
        process_2 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 0.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [0]
        process_3 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # B diffusion upwards at basis 1.
        coordinates = [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]]
        elements_before = ["B", "V"]
        elements_after = ["V", "B"]
        basis_sites = [1]
        process_4 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True)

        # A + B -> V + V
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["A", "B"]
        elements_after = ["V", "V"]
        basis_sites = [0]
        process_5 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)

        # V + V -> A + B
        coordinates = [[0.0, 0.0, 0.0], [0.5, 0.5, 0.5]]
        elements_before = ["V", "V"]
        elements_after = ["A", "B"]
        basis_sites = [0]
        process_6 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=False)

        # A adsorption on basis 0.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["A"]
        basis_sites = [0]
        process_7 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="A")

        # A adsorption on basis 1.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["A"]
        basis_sites = [1]
        process_8 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="A")

        # B adsorption on basis 0.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["B"]
        basis_sites = [0]
        process_9 = KMCProcess(coordinates=coordinates,
                               elements_before=elements_before,
                               elements_after=elements_after,
                               basis_sites=basis_sites,
                               rate_constant=rate,
                               fast=True,
                               redist=True,
                               redist_species="B")

        # B adsorption on basis 1.
        coordinates = [[0.0, 0.0, 0.0]]
        elements_before = ["V"]
        elements_after = ["B"]
        basis_sites = [1]
        process_10 = KMCProcess(coordinates=coordinates,
                                elements_before=elements_before,
                                elements_after=elements_after,
                                basis_sites=basis_sites,
                                rate_constant=rate,
                                fast=True,
                                redist=True,
                                redist_species="B")

        processes = [process_1, process_2, process_3, process_4, process_5,
                     process_6, process_7, process_8, process_9, process_10]
        interactions = KMCInteractions(processes, implicit_wildcards=True)

        # Setup the model.
        model = KMCLatticeModel(configuration, sitesmap, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = str(os.path.join(name, "run_redistribution_traj.py"))
        self.__files_to_remove.append(trajectory_filename)

        # Setup control parameters using SplitRandomRedistributor.
        split_control_parameters = KMCControlParameters(number_of_steps=10,
                                                        dump_interval=1,
                                                        do_redistribution=True,
                                                        redistribution_interval=2,
                                                        fast_species=["V"],
                                                        nsplits=(2, 2, 2))
        model.run(control_parameters=split_control_parameters,
                  trajectory_filename=trajectory_filename)

        # Setup control parameters using ProcessRandomRedistributor.
        distributor_type = "ProcessRandomDistributor"
        process_control_parameters = KMCControlParameters(number_of_steps=10,
                                                          dump_interval=1,
                                                          do_redistribution=True,
                                                          redistribution_interval=2,
                                                          distributor_type=distributor_type,
                                                          empty_element="V")
        model.run(control_parameters=process_control_parameters,
                  trajectory_filename=trajectory_filename)
        # }}}

if __name__ == '__main__':
    unittest.main()

