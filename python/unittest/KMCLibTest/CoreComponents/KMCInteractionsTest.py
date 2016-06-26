"""" Module for testing KMCInteractions """


# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest
import numpy
import sys
import os

from KMCLib.CoreComponents.KMCLocalConfiguration import KMCLocalConfiguration
from KMCLib.CoreComponents.KMCProcess import KMCProcess
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap
from KMCLib.CoreComponents.KMCUnitCell import KMCUnitCell
from KMCLib.CoreComponents.KMCLattice import KMCLattice
from KMCLib.PluginInterfaces.KMCRateCalculatorPlugin import KMCRateCalculatorPlugin
from KMCLib.Exceptions.Error import Error
from KMCLib.Backend import Backend

# Import the module to test.
from KMCLib.CoreComponents.KMCInteractions import KMCInteractions

# Import the test helpers.
sys.path.insert(0, os.path.join(os.path.abspath(os.path.dirname(__file__)), ".."))
from TestUtilities.Plugins.CustomRateCalculator.CustomRateCalculator import CustomRateCalculator


# Implementing the tests.
class KMCInteractionsTest(unittest.TestCase):
    """ Class for testing the KMCInteractions class """

    def testConstruction(self):
        """ Test that the KMCInteractions class can be constructed. """
        # {{{
        # A first process.
        coords = [[1.0,2.0,3.4],[12.0,13.0,-1.0],[1.1,1.2,1.3]]
        types0 = ["A","*","B"]
        types1 = ["B","*","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","C"]
        types1 = ["C","A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes)

        # Check the default implicit wildcard.
        self.assertTrue( kmc_interactions.implicitWildcards() )

        # Construct again with non default value.
        kmc_interactions = KMCInteractions(processes=processes,
                                           implicit_wildcards=False)

        # Check the wildcard again.
        self.assertFalse( kmc_interactions.implicitWildcards() )

        # Check the processes stored on the object.
        stored_processes = kmc_interactions._KMCInteractions__processes

        # Checks that the address is the same.
        self.assertTrue(stored_processes == processes)
        # }}}

    def testConstructionWithSitesMap(self):
        """ Test that the KMCInteractions class can be constructed with sitesmap provided. """
        # {{{
        # A first process.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "*", "B"]
        types1 = ["B", "*", "A"]
        site_types1 = ["M", "M", "M"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types1)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        site_types2 = ["M", "N"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types2)

        processes = [process_0, process_1]

        # Setup a sitesmap.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8, 0.0, 0.0],
                                                          [0.0, 3.2, 0.0],
                                                          [0.0, 0.5, 3.0]]),
                                basis_points=[[0.0, 0.0, 0.0],
                                              [0.5, 0.5, 0.5],
                                              [0.25, 0.25, 0.75]])

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

        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a', 'c', 'b'])

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)

        # Check sitesmap object.
        self.assertIs(sitesmap, kmc_interactions.sitesMap())

        # }}}

    def testConstructionWithSitesMapFails(self):
        """ Test that the KMCInteractions construction fails with improper sitesmap provided. """
        # {{{
        # A first process.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "*", "B"]
        types1 = ["B", "*", "A"]
        site_types1 = ["M", "M", "M"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types1)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        site_types2 = ["M", "N"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types2)

        processes = [process_0, process_1]

        # Setup a sitesmap.
        unit_cell = KMCUnitCell(cell_vectors=numpy.array([[2.8, 0.0, 0.0],
                                                          [0.0, 3.2, 0.0],
                                                          [0.0, 0.5, 3.0]]),
                                basis_points=[[0.0, 0.0, 0.0],
                                              [0.5, 0.5, 0.5],
                                              [0.25, 0.25, 0.75]])

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

        sitesmap = KMCSitesMap(lattice=lattice,
                               types=types,
                               possible_types=['a', 'c', 'b'])

        # Interactions object constrution fails when processes have sites type
        # and no sitesmap is passed to interactions.
        regex = r"Site types in process\d+ are set, sitesmap must be supplied."
        self.assertRaisesRegexp(Error, regex, KMCInteractions, processes=processes)

        # If sitesmap is not instance of SitesMap class, construction fails too.
        regex = (r"The sitesmap given to the KMCInteractions constructor" + 
                 r" must be of type KMCSitesMap.")
        self.assertRaisesRegexp(Error, regex, KMCInteractions,
                                processes=processes,
                                sitesmap="sitesmap")

        # }}}

    def testConstructionWithCustomRates(self):
        """ Test construction with custom rates. """
        # {{{
        # A first process.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A","*","B"]
        types1 = ["B","*","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","C"]
        types1 = ["C","A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Test that it fails with the wrong rate calculator.
        interactions = KMCInteractions(processes=processes,
                                       implicit_wildcards=True)

        # Test that it fails if the rate calculator is of wrong type.
        self.assertRaises( Error, lambda : interactions.setRateCalculator(rate_calculator="CustomRateCalculator") )

        # Test that it fails if the rate calculator is of wrong class.
        self.assertRaises( Error, lambda : interactions.setRateCalculator(rate_calculator=Error) )

        # Test that it fails if the rate calculator is instantiated.
        self.assertRaises( Error, lambda : interactions.setRateCalculator(rate_calculator=CustomRateCalculator()) )

        # Test that it fails if the rate calculator is the base class.
        self.assertRaises( Error, lambda : interactions.setRateCalculator(rate_calculator=KMCRateCalculatorPlugin) )

        # Construct the interactions object with a custom rate calculator class.
        kmc_interactions = KMCInteractions(processes=processes,
                                           implicit_wildcards=True)
        kmc_interactions.setRateCalculator(rate_calculator=CustomRateCalculator)

        # Test the stored name.
        self.assertEqual(kmc_interactions._KMCInteractions__rate_calculator_str, "CustomRateCalculator")

        # Test the stored rate calculator.
        self.assertTrue(isinstance(kmc_interactions._KMCInteractions__rate_calculator, CustomRateCalculator) )
        # }}}

    def testConstructionFailNoList(self):
        """ Test that the construction fails if the interactions list is not a list. """
        # {{{
        # Setup.
        coords = [[1.0,2.0,3.4],[12.0,13.0,-1.0],[1.1,1.2,1.3]]
        types0 = ["A","*","B"]
        types1 = ["B","*","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)
        # Construct and fail.
        self.assertRaises(Error, lambda: KMCInteractions(processes=process_0))

    def testConstructionFailWrongList(self):
        """ Test that the construction fails if the interactions list wrong. """
        # Setup.
        coords = [[1.0,2.0,3.4],[12.0,13.0,-1.0],[1.1,1.2,1.3]]
        types0 = ["A","*","B"]
        types1 = ["B","*","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)
        # Construct and fail.
        self.assertRaises(Error, lambda: KMCInteractions(processes=[process_0, coords]))

    def testConstructionFailsWrongWildcard(self):
        """ Test that we fail to construct with other than bool input for the implicit wildcard flag """
        # A first process.
        coords = [[1.0,2.0,3.4],[12.0,13.0,-1.0],[1.1,1.2,1.3]]
        types0 = ["A","*","B"]
        types1 = ["B","*","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","C"]
        types1 = ["C","A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1, basis_sites=[0,1,3], rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Construct and fail.
        self.assertRaises(Error, lambda: KMCInteractions(processes=processes,
                                                         implicit_wildcards=0) )

        self.assertRaises(Error, lambda: KMCInteractions(processes=processes,
                                                         implicit_wildcards="True") )

        self.assertRaises(Error, lambda: KMCInteractions(processes=processes,
                                                         implicit_wildcards=[False]) )
        # }}}

    def testBackend(self):
        """
        Test that the generated backend object is what we expect.
        """
        # {{{
        # A first process.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "D", "B"]
        types1 = ["B", "F", "A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes)

        # Setup a dict with the possible types.
        possible_types = {
            "A" : 13,
            "B" : 3,
            "J" : 4,
            "C" : 5,
            }

        # Check that setting up the backend fails if we have types in the processes that do
        # not corresponds to a type in the list of possible types.
        self.assertRaises(Error, lambda : kmc_interactions._backend(possible_types, 2))

        possible_types = {
            "A" : 13,
            "B" : 3,
            "D" : 2,
            "J" : 4,
            "C" : 5,
            }

        self.assertRaises(Error, lambda : kmc_interactions._backend(possible_types, 2))

        possible_types = {
            "A" : 13,
            "B" : 3,
            "F" : 2,
            "J" : 4,
            "C" : 5,
            }

        self.assertRaises(Error, lambda : kmc_interactions._backend(possible_types, 2))

        # Both "D" and "F" must be present.
        possible_types["D"] = 123

        # Get the backend.
        cpp_interactions = kmc_interactions._backend(possible_types, 2)

        # Check the type.
        self.assertTrue(isinstance(cpp_interactions, Backend.Interactions))

        # Get the processes out.
        cpp_processes = cpp_interactions.processes()

        # Check the length of the processes.
        self.assertEqual(cpp_processes.size(), 2)

        # Get the elements out of the second process.
        match_type  = cpp_processes[1].matchList()[0].match_type
        update_type = cpp_processes[1].matchList()[0].update_type

        # Match type should be "A" -> 15 and update type "C" -> 5
        self.assertEqual(match_type, 13)
        self.assertEqual(update_type, 5)

        # Get the elements out of the second process.
        match_type  = cpp_processes[1].matchList()[1].match_type
        update_type = cpp_processes[1].matchList()[1].update_type

        # Match type should be "C" -> 5 and update type "A" -> 13
        self.assertEqual(match_type, 5)
        self.assertEqual(update_type, 13)

        # Check the initial picked index, should be -1.
        self.assertEqual(cpp_interactions.pickedIndex(), -1)
        self.assertEqual(kmc_interactions.pickedIndex(), -1)

        # Check the initial process available sites list.
        self.assertTupleEqual(cpp_interactions.processAvailableSites(), (0, 0))
        self.assertTupleEqual(kmc_interactions.processAvailableSites(), (0, 0))
        # }}}

    def testBackendWithSiteTypes(self):
        """
        Test that the generated backend object is what we expect when site types is provided.
        """
        # {{{
        # A first process.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "D", "B"]
        types1 = ["B", "F", "A"]
        site_types = ["a", "b", "a"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        site_types = ["a", "b"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 2],
                               rate_constant=rate_0_1,
                               site_types=site_types)

        processes = [process_0, process_1]

        # Setup sitesmap.
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

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)

        # Setup a dict with the possible types.

        possible_types = {
            "A" : 13,
            "B" : 3,
            "D" : 123,
            "F" : 2,
            "J" : 4,
            "C" : 5,
            }

        # Get the backend.
        cpp_interactions = kmc_interactions._backend(possible_types, 3)

        # Check the type.
        self.assertTrue(isinstance(cpp_interactions, Backend.Interactions))

        # Get the processes out.
        cpp_processes = cpp_interactions.processes()

        # Check the length of the processes.
        self.assertEqual(cpp_processes.size(), 2)

        # Get the elements out of the first process.
        match_type = cpp_processes[0].matchList()[0].match_type
        update_type = cpp_processes[0].matchList()[0].update_type
        site_type = cpp_processes[0].matchList()[0].site_type
        self.assertEqual(match_type, 13)
        self.assertEqual(update_type, 3)
        self.assertEqual(site_type, 1)

        match_type = cpp_processes[0].matchList()[1].match_type
        update_type = cpp_processes[0].matchList()[1].update_type
        site_type = cpp_processes[0].matchList()[1].site_type
        self.assertEqual(match_type, 3)
        self.assertEqual(update_type, 13)
        self.assertEqual(site_type, 1)

        match_type = cpp_processes[0].matchList()[2].match_type
        update_type = cpp_processes[0].matchList()[2].update_type
        site_type = cpp_processes[0].matchList()[2].site_type
        self.assertEqual(match_type, 123)
        self.assertEqual(update_type, 2)
        self.assertEqual(site_type, 3)

        # Get the elements out of the second process.
        match_type  = cpp_processes[1].matchList()[0].match_type
        update_type = cpp_processes[1].matchList()[0].update_type
        site_type = cpp_processes[1].matchList()[0].site_type
        self.assertEqual(match_type, 13)
        self.assertEqual(update_type, 5)
        self.assertEqual(site_type, 1)

        match_type  = cpp_processes[1].matchList()[1].match_type
        update_type = cpp_processes[1].matchList()[1].update_type
        site_type = cpp_processes[1].matchList()[1].site_type
        self.assertEqual(match_type, 5)
        self.assertEqual(update_type, 13)
        self.assertEqual(site_type, 3)
        # }}}

    def testBackendWithCustomRates(self):
        """ Test that we can construct the backend object with custom rates. """
        # {{{
        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0], rate_constant=rate_0_1)

        # A second process.
        types0 = ["A","C"]
        types1 = ["C","A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1, basis_sites=[0], rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Set the custom rates class to use.
        custom_rates_class = CustomRateCalculator

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes,
                                           implicit_wildcards=False)
        kmc_interactions.setRateCalculator(rate_calculator=custom_rates_class)

        # Set the rate function on the custom rates calculator for testing.
        ref_rnd = numpy.random.uniform(0.0,1.0)
        def testRateFunction(coords, types_before, types_after, rate_const, process_number, global_coordinate):
            return ref_rnd
        kmc_interactions._KMCInteractions__rate_calculator.rate = testRateFunction

        # Setup a dict with the possible types.
        possible_types = {
            "A" : 13,
            "D" : 2,
            "B" : 3,
            "J" : 4,
            "C" : 5,
            }

        # Construct the backend.
        cpp_interactions = kmc_interactions._backend(possible_types, 2)

        # Get the rate calculator reference out of the C++ object and
        # check that a call from C++ calls the Python extension.
        cpp_coords = Backend.StdVectorDouble()
        cpp_types1 = Backend.StdVectorString()
        cpp_types2 = Backend.StdVectorString()
        rate_constant = 543.2211
        process_number = 33
        coordinate = (1.2,3.4,5.6)

        self.assertAlmostEqual( cpp_interactions.rateCalculator().backendRateCallback(cpp_coords,
                                                                                      cpp_coords.size()/3,
                                                                                      cpp_types1,
                                                                                      cpp_types2,
                                                                                      rate_constant,
                                                                                      process_number,
                                                                                      coordinate[0],
                                                                                      coordinate[1],
                                                                                      coordinate[2]), ref_rnd, 12 )
        self.assertAlmostEqual( kmc_interactions._KMCInteractions__rate_calculator.backendRateCallback(cpp_coords,
                                                                                                       cpp_coords.size()/3,
                                                                                                       cpp_types1,
                                                                                                       cpp_types2,
                                                                                                       rate_constant,
                                                                                                       process_number,
                                                                                                       coordinate[0],
                                                                                                       coordinate[1],
                                                                                                       coordinate[2]), ref_rnd, 12 )

        # Construct a C++ RateCalculator object directly and check that this object
        # returns the rate given to it.
        cpp_rate_calculator = Backend.RateCalculator()
        self.assertAlmostEqual( cpp_rate_calculator.backendRateCallback(cpp_coords,
                                                                        cpp_coords.size()/3,
                                                                        cpp_types1,
                                                                        cpp_types2,
                                                                        rate_constant,
                                                                        process_number,
                                                                        coordinate[0],
                                                                        coordinate[1],
                                                                        coordinate[2]), rate_constant, 12 )
        # }}}

    def testBackendNoFailWrongBasisMatch(self):
        """ Test for no failure when constructing backend with wrong n_basis """
        # A first process.
        coords = [[1.0,2.0,3.4],[1.1,1.2,1.3]]
        types0 = ["A","B"]
        types1 = ["B","A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1, basis_sites=[0,4], rate_constant=rate_0_1)

        # A second process.
        types0 = ["A","C"]
        types1 = ["C","A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1, basis_sites=[0,1], rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes)

        # Setup a dict with the possible types.
        possible_types = {
            "A" : 13,
            "D" : 2,
            "B" : 3,
            "J" : 4,
            "C" : 5,
            }

        # Get the backend - The [0,4] sites list for process_0 is simply ignored.
        cpp_interactions = kmc_interactions._backend(possible_types, 2)

        self.assertEqual( len(cpp_interactions.processes()[0].basisSites()), 1 )
        self.assertEqual( cpp_interactions.processes()[0].basisSites()[0], 0 )

        self.assertEqual( len(cpp_interactions.processes()[1].basisSites()), 2 )
        self.assertEqual( cpp_interactions.processes()[1].basisSites()[0], 0 )
        self.assertEqual( cpp_interactions.processes()[1].basisSites()[1], 1 )

    def testScript(self):
        """ Test that the KMCInteractions can generate its own script. """
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "D", "B"]
        types1 = ["B", "F", "A"]
        site_types = ["a", "b", "a"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1,
                               site_types=site_types)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        site_types = ["a", "b"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 2],
                               rate_constant=rate_0_1,
                               site_types=site_types)

        processes = [process_0, process_1]

        # Setup sitesmap.
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

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes, sitesmap=sitesmap)
        script = kmc_interactions._script()

        ref_script = """
# -----------------------------------------------------------------------------
# Interactions

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00],
               [   1.100000e+01,   1.100000e+01,  -4.400000e+00]]

elements_before = ['A','B','D']
elements_after  = ['B','A','F']
site_types = ['a','a','b']
move_vectors    = None
basis_sites     = [0,1,3]
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
site_types = ['a','b']
move_vectors    = [(  0,[   1.000000e-01,  -8.000000e-01,  -2.100000e+00]),
                   (  1,[  -1.000000e-01,   8.000000e-01,   2.100000e+00])]
basis_sites     = [0,1,2]
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

interactions = KMCInteractions(
    processes=processes,
    sitesmap=sitesmap,
    implicit_wildcards=True)
"""
        self.assertEqual(script, ref_script)

        # Test script without sitetypes.
        coords = [[1.0, 2.0, 3.4], [12.0, 13.0, -1.0], [1.1, 1.2, 1.3]]
        types0 = ["A", "D", "B"]
        types1 = ["B", "F", "A"]
        rate_0_1 = 3.5
        process_0 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 3],
                               rate_constant=rate_0_1)

        # A second process.
        coords = [[1.0, 2.0, 3.4], [1.1, 1.2, 1.3]]
        types0 = ["A", "C"]
        types1 = ["C", "A"]
        rate_0_1 = 1.5
        process_1 = KMCProcess(coords, types0, types1,
                               basis_sites=[0, 1, 2],
                               rate_constant=rate_0_1)

        processes = [process_0, process_1]

        # Construct the interactions object.
        kmc_interactions = KMCInteractions(processes=processes)
        script = kmc_interactions._script()

        ref_script = """
# -----------------------------------------------------------------------------
# Interactions

coordinates = [[   0.000000e+00,   0.000000e+00,   0.000000e+00],
               [   1.000000e-01,  -8.000000e-01,  -2.100000e+00],
               [   1.100000e+01,   1.100000e+01,  -4.400000e+00]]

elements_before = ['A','B','D']
elements_after  = ['B','A','F']
site_types = None
move_vectors    = None
basis_sites     = [0,1,3]
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
basis_sites     = [0,1,2]
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
"""
        self.assertEqual(script, ref_script)


if __name__ == '__main__':
    unittest.main()
