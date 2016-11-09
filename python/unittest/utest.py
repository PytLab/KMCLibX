# Copyright (c)  2012  Mikael Leetmaa
# Copyright (c)  2016-2019 Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import os
import unittest

from KMCLib.Backend.Backend import MPICommons

# Import the tests.
from KMCLibTest import KMCLibTests


# Setup the suites.
def suite():
    suite = unittest.TestSuite([KMCLibTests.suite()])
    return suite

# Run the tests.
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())
    if MPICommons.isMaster():
        os.remove("out.log")

