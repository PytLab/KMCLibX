

# Copyright (c)  2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest

from KMCConfigurationTest import KMCConfigurationTest
from KMCSitesMapTest import KMCSitesMapTest
from KMCControlParametersTest import KMCControlParametersTest
from KMCInteractionsTest import KMCInteractionsTest
from KMCLatticeModelTest import KMCLatticeModelTest
from KMCLatticeTest import KMCLatticeTest
from KMCLocalConfigurationTest import KMCLocalConfigurationTest
from KMCProcessTest import KMCProcessTest
from KMCUnitCellTest import KMCUnitCellTest

def suite():
    suite = unittest.TestSuite([
         unittest.TestLoader().loadTestsFromTestCase(KMCConfigurationTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCSitesMapTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCControlParametersTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCInteractionsTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCLatticeModelTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCLatticeTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCLocalConfigurationTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCProcessTest),
         unittest.TestLoader().loadTestsFromTestCase(KMCUnitCellTest)
])
    return suite


if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

