""" Module for testing the KMCControlParameters class. """


# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest

from KMCLib.Exceptions.Error import Error
from KMCLib.Backend import Backend

# Import from the module we test.
from KMCLib.CoreComponents.KMCControlParameters import KMCControlParameters


# Implement the test.
class KMCControlParametersTest(unittest.TestCase):
    """ Class for testing the KMCControlParameters class. """

    def testConstructionAndQuery(self):
        """ Test the construction of the control parametes object """
        # {{{
        # Default construction.
        control_params = KMCControlParameters()

        # Check the default values.
        self.assertEqual(control_params.numberOfSteps(), 0)
        self.assertEqual(control_params.dumpInterval(), 1)
        self.assertEqual(control_params.seed(), 1)
        self.assertTrue(control_params.timeSeed())
        self.assertEqual(control_params.rngType(), Backend.MT)

        # Non-default construction.
        control_params = KMCControlParameters(number_of_steps=2000000,
                                              dump_interval=1000,
                                              analysis_interval=888,
                                              seed=2013,
                                              rng_type='DEVICE')

        # Check the values.
        self.assertEqual(control_params.numberOfSteps(), 2000000)
        self.assertEqual(control_params.dumpInterval(), 1000)
        self.assertEqual(control_params.analysisInterval(), 888)
        self.assertEqual(control_params.seed(), 2013)
        self.assertFalse(control_params.timeSeed())
        self.assertEqual(control_params.rngType(), Backend.DEVICE)
        # }}}

    def testAnalysisInterval(self):
        """ Test all valid values of the analysis_interval parameter. """
        # {{{
        control_params = KMCControlParameters()
        self.assertEqual(control_params.analysisInterval(), 1)

        control_params = KMCControlParameters(analysis_interval=10)
        self.assertEqual(control_params.analysisInterval(), 10)

        control_params = KMCControlParameters(analysis_interval=(1, 2, 3))
        self.assertTupleEqual(control_params.analysisInterval(), (1, 2, 3))

        control_params = KMCControlParameters(analysis_interval=[1, 2, 3])
        self.assertListEqual(control_params.analysisInterval(), [1, 2, 3])

        control_params = KMCControlParameters(analysis_interval=[1, 2, (1, 3, 1)])
        self.assertListEqual(control_params.analysisInterval(), [1, 2, (1, 3, 1)])

        # Check wrong values.
        self.assertRaises(Error, lambda: KMCControlParameters(analysis_interval=-1))
        self.assertRaises(Error, lambda: KMCControlParameters(analysis_interval=(1, 2, 3, (1, 2))))
        self.assertRaises(Error, lambda: KMCControlParameters(analysis_interval=(-11, 2, 3)))
        self.assertRaises(Error, lambda: KMCControlParameters(analysis_interval=(1, 2, 3, (1, -4, 1))))
        # }}}

    def testRngTypeInput(self):
        """ Test all valid values of the rng_type parameter. """
        # {{{
        control_params = KMCControlParameters()
        self.assertEqual(control_params.rngType(), Backend.MT)

        control_params = KMCControlParameters(rng_type='MT')
        self.assertEqual(control_params.rngType(), Backend.MT)

        control_params = KMCControlParameters(rng_type='RANLUX24')
        self.assertEqual(control_params.rngType(), Backend.RANLUX24)

        control_params = KMCControlParameters(rng_type='RANLUX48')
        self.assertEqual(control_params.rngType(), Backend.RANLUX48)

        control_params = KMCControlParameters(rng_type='MINSTD')
        self.assertEqual(control_params.rngType(), Backend.MINSTD)

        control_params = KMCControlParameters(rng_type='DEVICE')
        self.assertEqual(control_params.rngType(), Backend.DEVICE)

        # Wrong value.
        self.assertRaises( Error,
                           lambda : KMCControlParameters(rng_type='ABC'))

        # Wrong type.
        self.assertRaises( Error,
                           lambda : KMCControlParameters(rng_type=123))
        # }}}

    def testStartTime(self):
        """ Make sure we can set start time correctly. """
        # {{{
        control_params = KMCControlParameters()
        self.assertEqual(control_params.startTime(), 0.0)

        control_params = KMCControlParameters(start_time=1.0)
        self.assertEqual(control_params.startTime(), 1.0)

        # Negative value.
        self.assertRaises(Error, KMCControlParameters, start_time=-1.0)

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters, start_time=(1, 1))
        # }}}

    def testExtraTraj(self):
        " Make sure we can set extra trajectory setting parameter correctly. "
        # {{{
        control_params = KMCControlParameters()
        self.assertEqual(control_params.extraTraj(), None)

        control_params = KMCControlParameters(extra_traj=(0, 100, 2))
        self.assertTupleEqual(control_params.extraTraj(), (0, 100, 2))

        # Wrong start and end.
        self.assertRaises(Error, KMCControlParameters, extra_traj=(2, 0, 1))

        # Wrong interval.
        self.assertRaises(Error, KMCControlParameters, extra_traj=(0, 100, 101))

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters, extra_traj=1)
        self.assertRaises(Error, KMCControlParameters, extra_traj=1.0)
        self.assertRaises(Error, KMCControlParameters, extra_traj=(1.0, 100.0, 1.0))
        self.assertRaises(Error, KMCControlParameters, extra_traj=('a', 'as'))
        # }}}

    def testDoRedistribution(self):
        " Make sure the redistribution flag can be set correctly. "
        # {{{
        control_params = KMCControlParameters()
        self.assertFalse(control_params.doRedistribution())

        control_params = KMCControlParameters(do_redistribution=True)
        self.assertTrue(control_params.doRedistribution())

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters, do_redistribution=1)
        # }}}

    def testRedistributionInterval(self):
        " Make sure the redistribution_interval can be set correctly. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        self.assertEqual(control_params.redistributionInterval(), 10)

        control_params = KMCControlParameters(do_redistribution=True,
                                              redistribution_interval=100)
        self.assertEqual(control_params.redistributionInterval(), 100)

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          redistribution_interval=100.0)

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          redistribution_interval=(1,2,3))

        control_params = KMCControlParameters(redistribution_interval=100)
        self.assertRaises(AttributeError, control_params.redistributionInterval)
        # }}}

    def testFastSpecies(self):
        " Make sure the default fast species can be set properly. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        self.assertListEqual(control_params.fastSpecies(), [])

        control_params = KMCControlParameters(do_redistribution=True,
                                              fast_species=["V"])
        self.assertListEqual(control_params.fastSpecies(), ["V"])

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          fast_species=[1, 2])

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          fast_species="as")

        control_params = KMCControlParameters(fast_species=["V"])
        self.assertRaises(AttributeError, control_params.fastSpecies)
        # }}}

    def testNsplits(self):
        " Make sure the nsplits can be set correctly. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        self.assertTupleEqual(control_params.nsplits(), (1, 1, 1))

        control_params = KMCControlParameters(do_redistribution=True,
                                              nsplits=(2, 2, 2))
        self.assertTupleEqual(control_params.nsplits(), (2, 2, 2))

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          nsplits=2)

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          nsplits="as")

        # Wrong length.
        self.assertRaises(Error, KMCControlParameters,
                         do_redistribution=True,
                         nsplits=(2, 2))

        control_params = KMCControlParameters(nsplits=(2, 2, 2))
        self.assertRaises(AttributeError, control_params.nsplits)
        # }}}

    def testRedisDumpInterval(self):
        " Make sure the redist_dump_interval can be set correctly. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        # The default interval should be equal to the dump interval.
        self.assertEqual(control_params.redistDumpInterval(), 1)

        control_params = KMCControlParameters(do_redistribution=True,
                                              redist_dump_interval=100)
        self.assertEqual(control_params.redistDumpInterval(), 100)

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          redist_dump_interval=(1, 1, 1))

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          redist_dump_interval=0.1)

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          redist_dump_interval="asf")
        # }}}

    def testSlowFlagsFunc(self):
        " Make sure we can get the correct function to return slow indices. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        # The default interval should be equal to the dump interval.
        self.assertListEqual(control_params.slowIndicesFunc()(), [])

        control_params = KMCControlParameters(do_redistribution=True,
                                              slow_indices_func=lambda *args: [1, 2, 3])
        self.assertListEqual(control_params.slowIndicesFunc()(), [1, 2, 3])
        # }}}

    def testDistributorTypeAndEmptyElement(self):
        " Make sure the distributor type can be set correctly. "
        # {{{
        control_params = KMCControlParameters(do_redistribution=True)
        self.assertEqual(control_params.distributorType(), "SplitRandomDistributor")

        control_params = KMCControlParameters(do_redistribution=True,
                                              distributor_type="ProcessRandomDistributor",
                                              empty_element="V")
        self.assertEqual(control_params.distributorType(), "ProcessRandomDistributor")
        self.assertEqual(control_params.emptyElement(), "V")

        # Wrong type.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          distributor_type=1)

        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          empty_element=1)

        # No empty element for ProcessRandomDistributor.
        self.assertRaises(Error, KMCControlParameters,
                          do_redistribution=True,
                          distributor_type="ProcessRandomDistributor")
        # }}}

    def testConstructionFail(self):
        """ Make sure we can not give invalid paramtes on construction. """
        # {{{
        # Negative values.
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps=-1,
                                                         dump_interval=1,
                                                         analysis_interval=1) )
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps=1,
                                                         dump_interval=-1,
                                                         analysis_interval=1) )
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps=1,
                                                         dump_interval=1,
                                                         analysis_interval=-1) )

        # Wrong type.
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps="1",
                                                         dump_interval=1,
                                                         analysis_interval=1) )
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps=1,
                                                         dump_interval="1",
                                                         analysis_interval=1) )
        self.assertRaises( Error,
                           lambda : KMCControlParameters(number_of_steps=1,
                                                         dump_interval=1,
                                                         analysis_interval="1") )
        # }}}

if __name__ == '__main__':
    unittest.main()

