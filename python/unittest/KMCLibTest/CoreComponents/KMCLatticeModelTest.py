""" Module for testing the KMCLatticeModel class. """


# Copyright (c)  2012-2014  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest
import numpy
import sys
import os

from KMCLib.CoreComponents.KMCInteractions import KMCInteractions
from KMCLib.CoreComponents.KMCProcess import KMCProcess
from KMCLib.CoreComponents.KMCConfiguration import KMCConfiguration
from KMCLib.CoreComponents.KMCLocalConfiguration import KMCLocalConfiguration
from KMCLib.CoreComponents.KMCControlParameters import KMCControlParameters
from KMCLib.CoreComponents.KMCUnitCell import KMCUnitCell
from KMCLib.CoreComponents.KMCLattice  import KMCLattice
from KMCLib.PluginInterfaces.KMCAnalysisPlugin import KMCAnalysisPlugin
from KMCLib.Exceptions.Error import Error
from KMCLib.Backend.Backend import MPICommons

# Import from the module we test.
from KMCLib.CoreComponents.KMCLatticeModel import KMCLatticeModel

# Test helpers.
sys.path.insert(0, os.path.join(os.path.abspath(os.path.dirname(__file__)), ".."))
from TestUtilities.Plugins.CustomRateCalculator.CustomRateCalculator import CustomRateCalculator

from KMCLib.Backend.Backend import MPICommons

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
        model = KMCLatticeModel(config, interactions)

        # Check that it has the attribute _backend which is None
        self.assertTrue(hasattr(model,"_KMCLatticeModel__backend"))
        self.assertTrue(model._KMCLatticeModel__backend is None)

        # Check that it has the correct interactions stored.
        self.assertTrue(model._KMCLatticeModel__interactions == interactions)

        # Check that it has the correct configuration stored.
        self.assertTrue(model._KMCLatticeModel__configuration == config)


    def testRunImplicitWildcards(self):
        """ Test that ta valid model can run for a few steps. """
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
        model = KMCLatticeModel(config, interactions)

        # Get the match types out.
        match_types = [ l.match_type for l in model._backend().interactions().processes()[0].minimalMatchList() ]

        # This does not have wildcards added.
        ref_match_types = [1, 2, 2, 2, 2, 1]
        self.assertEqual( match_types, ref_match_types )

        # Create with implicit wildcards - this is default behavior.
        interactions = KMCInteractions(processes=processes)

        # Create the model.
        model = KMCLatticeModel(config, interactions)

        # Check the process matchlists again.
        match_types = [ l.match_type for l in model._backend().interactions().processes()[0].minimalMatchList() ]

        ref_match_types = [1, 2, 2, 2, 2,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 1]

        # This one has the wildcards (zeroes) added.
        self.assertEqual( match_types, ref_match_types )

        # Setup the run paramters.
        control_parameters = KMCControlParameters(number_of_steps=10,
                                                  dump_interval=1)
        model.run(control_parameters)

    def testRun2(self):
        """ Test the run of an A-B flip model. """
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
        ab_flip_model = KMCLatticeModel(configuration, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = os.path.join(name, "ab_flip_traj.py")
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
            execfile(trajectory_filename, global_dict, local_dict)

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

    def testCustomRatesRun(self):
        """ Test the run of an A-B flip model with custom rates. """
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
        ab_flip_model = KMCLatticeModel(configuration, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = os.path.join(name, "ab_flip_traj_custom.py")
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
            execfile(trajectory_filename, global_dict, local_dict)

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

    def testRunWithAnalysis(self):
        """ Test that the analyis plugins get called correctly. """
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
        ab_flip_model = KMCLatticeModel(configuration, interactions)

        # Run the model with a trajectory file.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")
        trajectory_filename = os.path.join(name, "ab_flip_traj.py")
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

            def setup(self, step, time, configuration):
                self.setup_called = True

            def registerStep(self, step, time, configuration):
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

    def testRunFailAnalysis(self):
        """ Test that the analyis plugins get called correctly. """
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
        ab_flip_model = KMCLatticeModel(configuration, interactions)

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

    def testRunTrajectory(self):
        """ Test the run of an A-B flip model with trajectory output. """
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
        ab_flip_model = KMCLatticeModel(configuration, interactions)

        # Construct the trajectory fileames.
        name = os.path.abspath(os.path.dirname(__file__))
        name = os.path.join(name, "..", "TestUtilities", "Scratch")

        lattice_trajectory_filename = os.path.join(name, "ab_flip_traj_lattice.py")
        xyz_trajectory_filename = os.path.join(name, "ab_flip_traj_xyz.xyz")

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

    def testRunRngType(self):
        """ Test that it is possible to run with each of the supported PRNG:s. """
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
        ab_flip_model_MT       = KMCLatticeModel(configuration, interactions)
        ab_flip_model_RANLUX24 = KMCLatticeModel(configuration, interactions)
        ab_flip_model_RANLUX48 = KMCLatticeModel(configuration, interactions)
        ab_flip_model_MINSTD   = KMCLatticeModel(configuration, interactions)
        ab_flip_model_DEVICE   = KMCLatticeModel(configuration, interactions)

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

    def testRunRngTypeDevice(self):
        """ Test to use the PRNG DEVICE. """
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
        ab_flip_model_DEVICE   = KMCLatticeModel(configuration, interactions)

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

    def testBackend(self):
        """ Test that the backend object is correctly constructed. """
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

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A','C','B'])


        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        sites  = [0,1,2,3,4]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=sites,
                               rate_constant=rate_0_1)

        # A second process.
        types0 = ["A","C"]
        types1 = ["C","A"]
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
        model = KMCLatticeModel(config, interactions)

        # Get the c++ backend out.
        cpp_model = model._backend()

        # Check that this backend object is stored on the class.
        self.assertTrue(model._KMCLatticeModel__backend == cpp_model)

    def testScript(self):
        """ Test that a script can be created. """
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

        # Setup the configuration.
        config = KMCConfiguration(lattice=lattice,
                                  types=types,
                                  possible_types=['A','C','B'])

        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords,
                               types0,
                               types1,
                               basis_sites=[0],
                               rate_constant=rate_0_1)

        # A second process.
        types0 = ["A","C"]
        types1 = ["C","A"]
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
        model = KMCLatticeModel(config, interactions)

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

possible_types = ['A','C','B']

configuration = KMCConfiguration(
    lattice=lattice,
    types=types,
    possible_types=possible_types)

# -----------------------------------------------------------------------------
# Interactions

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','B']
elements_after  = ['B','A']
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
    rate_constant=rate_constant)

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','C']
elements_after  = ['C','A']
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
    rate_constant=rate_constant)

processes = [process_0,
             process_1]

interactions = KMCInteractions(
    processes=processes,
    implicit_wildcards=True)

# -----------------------------------------------------------------------------
# Lattice model

model = KMCLatticeModel(
    configuration=configuration,
    interactions=interactions)
"""
        self.assertEqual(script, ref_script)

        # Get the script again, with a different variable name.
        script = model._script(variable_name="my_model")

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

possible_types = ['A','C','B']

configuration = KMCConfiguration(
    lattice=lattice,
    types=types,
    possible_types=possible_types)

# -----------------------------------------------------------------------------
# Interactions

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','B']
elements_after  = ['B','A']
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
    rate_constant=rate_constant)

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00]]

elements_before = ['A','C']
elements_after  = ['C','A']
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
    rate_constant=rate_constant)

processes = [process_0,
             process_1]

interactions = KMCInteractions(
    processes=processes,
    implicit_wildcards=True)

# -----------------------------------------------------------------------------
# Lattice model

my_model = KMCLatticeModel(
    configuration=configuration,
    interactions=interactions)
"""

        # Check.
        self.assertEqual(script, ref_script)


if __name__ == '__main__':
    unittest.main()

