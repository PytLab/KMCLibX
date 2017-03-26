'''
Module for all test suites.
'''
# Copyright (c)  2012  Mikael Leetmaa
# Copyright (c)  2016-2019 Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import unittest

from .Analysis import AnalysisTests
from .Backend import BackendTests
from .CoreComponents import CoreComponentsTests
from .Exceptions import ExceptionsTests
from .PluginInterfaces import PluginInterfacesTests
from .Utilities import UtilitiesTests


def suite():
    '''
    Function to get all TestSuite containing all test suites.
    '''
    all_suites = unittest.TestSuite([
        AnalysisTests.suite(),
        BackendTests.suite(),
        CoreComponentsTests.suite(),
        ExceptionsTests.suite(),
        PluginInterfacesTests.suite(),
        UtilitiesTests.suite()
    ])
    return all_suites

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

