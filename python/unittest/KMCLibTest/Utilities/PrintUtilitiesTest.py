""" Module for testing the common print utilities. """


# Copyright (c)  2013 - 2015  Mikael Leetmaa
# Copyright (c)  2016 - 2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

import unittest
import sys

from KMCLib import PY2

if PY2:
    from StringIO import StringIO
else:
    from io import StringIO

from KMCLib.Backend.Backend import MPICommons

# Import from the module we test.
from KMCLib.Utilities.PrintUtilities import prettyPrint
from KMCLib.Utilities.PrintUtilities import printHeader


# Implement the test.
class PrintUtilitiesTest(unittest.TestCase):
    """ Class for testing the print utility functions. """

    def testPrettyPrint(self):
        """ Test that we can call the pretty print function. """
        # Print to stdout.
        original_sys_stdout = sys.stdout
        try:
            stream_1   = StringIO()
            sys.stdout = stream_1

            # Print to stdout.
            ref_str = "This is what we print"
            prettyPrint(ref_str)

            # Check.
            if MPICommons.myRank() == 0:
                ref_str = ref_str + "\n"
            else:
                ref_str = ""
            self.assertEqual(stream_1.getvalue(), ref_str)

        finally:
            # Put the original stdout back.
            sys.stdout = original_sys_stdout

        # Print to another stream.
        stream_2 = StringIO()
        ref_str = "This is what we print next time."
        prettyPrint(ref_str, output=stream_2)

        # Check.
        if MPICommons.myRank() == 0:
            ref_str = ref_str + "\n"
        else:
            ref_str = ""

        self.assertEqual(stream_2.getvalue(), ref_str)

    def testPrintHeader(self):
        """ Test the print header function. """
        # Print to stdout.
        original_sys_stdout = sys.stdout

        try:
            stream_1   = StringIO()
            sys.stdout = stream_1

            # Print to stdout.
            printHeader()

            # Check.
            if MPICommons.myRank() == 0:
                ref_str = """# -----------------------------------------------------------------------------
# KMCLibX version 2.0.0
# Distributed under the GPLv3.0 license
#
# Copyright (C)  2016-2019  Shao Zhengjiang
# Developed by Shao Zhengjiang <shaozhengjiang@gmail.com>
#
# KMCLibX is developed based on KMCLib (https://github.com/leetmaa/KMCLib)
# Developed by Mikael Leetmaa <leetmaa@kth.se>
#
# This program is distributed in the hope that it will be useful
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# LICENSE and README files, and the source code, for details.
#
# You should have received a copy of the GNU General Public License version 3
# (GPLv3) along with this program. If not, see <http://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------

"""
            else:
                ref_str = ""

            # Check.
            self.assertEqual(ref_str, stream_1.getvalue())

        finally:
            # Put the original stdout back.
            sys.stdout = original_sys_stdout

        # Print to another stream.
        stream_2 = StringIO()
        prettyPrint(ref_str, output=stream_2)

        # Check.
        self.assertTrue(ref_str in stream_2.getvalue())


if __name__ == '__main__':
    unittest.main()
