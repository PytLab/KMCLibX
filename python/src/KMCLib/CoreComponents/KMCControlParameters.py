""" Module for the KMCControlParameters """


# Copyright (c)  2012-2013  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

from KMCLib.Utilities.CheckUtilities import checkPositiveInteger
from KMCLib.Backend import Backend
from KMCLib.Exceptions.Error import Error

class KMCControlParameters(object):
    """
    Class for handling the control parameters that goes in to a KMC simulation.
    """

    def __init__(self,
                 number_of_steps=None,
                 dump_interval=None,
                 analysis_interval=None,
                 seed=None,
                 rng_type=None):
        """
        Constructuor for the KMCControlParameters object that
        holds all parameters controlling the flow of the KMC simulation.

        :param number_of_steps: The number of KMC steps to take. If not provided
                                the default value 0 will  be used.
        :type number_of_steps: int

        :param dump_interval: The number of steps between subsequent trajectory
                              dumps. The default value is 1, i.e. dump every step.
        :type dump_interval: int

        :param analysis_interval: The number of steps between subsequent calls to
                                  the custom analysis 'registerStep' functions.
                                  The default value is 1, i.e. analysis every step.
        :type analysis_interval: int

        :param seed: The seed value to use for the backend random number generator.
                     If no seed value is given the random numnber generator will be
                     seeded based on the wall-clock time.
        :type seed: int

        :param rng_type: The type of pseudo random number generator to use.  The random
                         number generators are the once in the standard C++ library and
                         the supported types maps directly to the given C++ construction
                         as indicated here:

                         'MT' for Mersenne-Twister (Default) [rng = std::mt19937],
                         'MINSTD' for the 'minimum standard' rng [rng = std::minstd_rand],
                         'RANLUX24' for the 24-bit version of the ranlux rng [rng = std::ranlux24],
                         'RANLUX48' for the 48-bit version of the ranlux rng [rng = std::ranlux48].

                         The sequence of pseudo-random numbers is generated by repeated calls to
                         std::generate_canonical<double, 32>(rng);

                         See: http://en.cppreference.com/w/cpp/numeric/random for further details on
                         C++ random number generators.

                         There is also the option 'DEVICE' for using a random device
                         installed on the macine through [rng = std::random_device] in C++. Note
                         however that you should spend some time evaluating this option to make
                         sure it works as you expect if you have a random device installed, since this
                         has not been tested with a random device by the KMCLib developers.
        :type rng_type: str
        """
        # Check and set the number of steps.
        self.__number_of_steps = checkPositiveInteger(number_of_steps,
                                                      0,
                                                      "number_of_steps")

        self.__dump_interval = checkPositiveInteger(dump_interval,
                                                    1,
                                                    "dump_interval")

        self.__analysis_interval = checkPositiveInteger(analysis_interval,
                                                        1,
                                                        "analysis_interval")

        self.__time_seed = (seed is None)
        self.__seed = checkPositiveInteger(seed,
                                           1,
                                           "seed")

        # Check and set the random number generator type.
        self.__rng_type  = self.__checkRngType(rng_type, "MT")

    def __checkRngType(self, rng_type, default):
        """
        Private helper function to check the random number generator input.
        """
        if rng_type is None:
            rng_type = default

        rng_dict = { "MT"       : Backend.MT,
                     "MINSTD"   : Backend.MINSTD,
                     "RANLUX24" : Backend.RANLUX24,
                     "RANLUX48" : Backend.RANLUX48,
                     "DEVICE"   : Backend.DEVICE,
                     }

        if not rng_type in rng_dict.keys():
            raise Error("'rng_type' input must be one of the supported types. Check the documentation for the list of supported types. Default is 'MT' (Mersenne-Twister) [std::mt19937].")

        return rng_dict[rng_type]

    def numberOfSteps(self):
        """
        Query for the number of steps.

        :returns: The number of steps.
        """
        return self.__number_of_steps

    def dumpInterval(self):
        """
        Query for the dump interval.

        :returns: The dump interval.
        """
        return self.__dump_interval

    def analysisInterval(self):
        """
        Query for the analysis interval.

        :returns: The analysis interval.
        """
        return self.__analysis_interval

    def seed(self):
        """
        Query for the seed value.

        :returns: The seed value.
        """
        return self.__seed

    def timeSeed(self):
        """
        Query for the time seed value.

        :returns: The time seed value.
        """
        return self.__time_seed

    def rngType(self):
        """
        Query for the pseudo random number generator type.
        """
        return self.__rng_type

