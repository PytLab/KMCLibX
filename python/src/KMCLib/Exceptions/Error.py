""" Module containing error handling. """


# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project(based on KMCLib) distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

from KMCLib import PY2

if PY2:
    from exceptions import Exception


class Error(Exception):
    """ Class for describing an error. """

    def __init__(self, msg=''):
        """
        Constructor for the error class.

        :param msg: A message to the user describing what went wrong. If none
                    is given the message is set to an empty string.
        :type msg: string
        """
        self.__msg = msg

    def __str__(self):
        """
        Get a string representation of the error.

        :returns: The error message string.
        """
        return self.__msg
