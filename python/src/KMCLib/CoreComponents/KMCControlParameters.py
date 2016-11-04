""" Module for the KMCControlParameters """


# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

from KMCLib.Utilities.CheckUtilities import checkPositiveInteger
from KMCLib.Utilities.CheckUtilities import checkPositiveFloat
from KMCLib.Utilities.CheckUtilities import checkSequenceOfPositiveIntegers
from KMCLib.Utilities.CheckUtilities import checkSequenceOf
from KMCLib.Utilities.CheckUtilities import checkBoolean
from KMCLib.Backend import Backend
from KMCLib.Exceptions.Error import Error


class KMCControlParameters(object):
    """
    Class for handling the control parameters that goes in to a KMC simulation.
    """

    def __init__(self, **kwargs):
        """
        Constructuor for the KMCControlParameters object that
        holds all parameters controlling the flow of the KMC simulation.

        :param time_limit: The upper-bound time limit of kmc loop. If not provided
                           the default valut inf will be used.
        :type time_limit: float.

        :param number_of_steps: The number of KMC steps to take. If not provided
                                the default value 0 will  be used.
        :type number_of_steps: int

        :param dump_interval: The number of steps between subsequent trajectory
                              dumps. The default value is 1, i.e. dump every step.
        :type dump_interval: int

        :param analysis_interval: The numbers of steps between subsequent calls to
                                  the custom analysis 'registerStep' functions.
                                  The default value is 1, i.e. analysis every step.
                                  It could be a list of int, i.e. [1, 2, 3] means
                                  the intervals for the corresponding analysis.
        :type analysis_interval: list of int / int

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
                         sure it works as you expect if you have a random device installed,
                         since this has not been tested with a random device by the KMCLib developers.
        :type rng_type: str
        """
        # {{{
        # Check and set the time limit.
        time_limit = kwargs.pop("time_limit", None)
        self.__time_limit = checkPositiveFloat(time_limit, float("inf"), "time_limit")

        # Check and set the number of steps.
        number_of_steps = kwargs.pop("number_of_steps", None)
        self.__number_of_steps = checkPositiveInteger(number_of_steps, 0,
                                                      "number_of_steps")

        dump_interval = kwargs.pop("dump_interval", None)
        self.__dump_interval = checkPositiveInteger(dump_interval, 1,
                                                    "dump_interval")

        # Check the analysis intervals.
        analysis_interval = kwargs.pop("analysis_interval", None)
        self.__analysis_interval = self.__checkAnalysisInterval(analysis_interval)

        seed = kwargs.pop("seed", None)
        self.__time_seed = (seed is None)
        self.__seed = checkPositiveInteger(seed, 1, "seed")

        # Check and set the random number generator type.
        rng_type = kwargs.pop("rng_type", None)
        self.__rng_type = self.__checkRngType(rng_type, "MT")

        # Check and set start time.
        start_time = kwargs.pop("start_time", None)
        self.__start_time = self.__checkStartTime(start_time, 0.0)

        # Check the extra trjactory output parameter.
        extra_traj = kwargs.pop("extra_traj", None)
        self.__extra_traj = self.__checkExtraTraj(extra_traj, None)

        # Check redistribution operation flag.
        do_redistribution = kwargs.pop("do_redistribution", None)
        self.__do_redistribution = checkBoolean(do_redistribution, False,
                                                "do_redistribution")

        if self.__do_redistribution:
            # Check redistribution interval.
            redistribution_interval = kwargs.pop("redistribution_interval", None)
            self.__redistribution_interval = checkPositiveInteger(redistribution_interval,
                                                                  10, "redistribution_interval")
            if self.__redistribution_interval == 0:
                raise Error("redistribution_interval must be a positive integer.")

            # Check fast species.
            fast_species = kwargs.pop("fast_species", None)
            self.__fast_species = self.__checkFastSpecies(fast_species, [])

            # Check nsplits.
            nsplits = kwargs.pop("nsplits", None)
            self.__nsplits = self.__checkNsplits(nsplits, (1, 1, 1))

        # }}}

    def __checkAnalysisInterval(self, analysis_interval):
        """
        Private helper function to check the analysis interval input.
        """
        # {{{
        if analysis_interval is None:
            return 1
        elif type(analysis_interval) is int:
            checkPositiveInteger(analysis_interval, 1, "analysis_interval")
        elif type(analysis_interval) in (list, tuple):
            for entry in analysis_interval:
                # Check single integer.
                if type(entry) is int and entry <= 0:
                    msg = "Interval {} is not positive interger.".format(entry)
                    raise Error(msg)

                # Check custom interval.
                if type(entry) in (list, tuple):
                    if len(entry) != 3:
                        msg = "Interval list {} length is not 3".format(entry)
                        raise Error(msg)
                    msg = "{} is not a list of positive integers.".format(entry)
                    checkSequenceOfPositiveIntegers(entry, msg)
        else:
            msg = "Incorrect analysis interval: {}".format(analysis_interval)
            raise Error(msg)

        return analysis_interval
        # }}}

    def __checkRngType(self, rng_type, default):
        """
        Private helper function to check the random number generator input.
        """
        # {{{
        if rng_type is None:
            rng_type = default

        rng_dict = { "MT"       : Backend.MT,
                     "MINSTD"   : Backend.MINSTD,
                     "RANLUX24" : Backend.RANLUX24,
                     "RANLUX48" : Backend.RANLUX48,
                     "DEVICE"   : Backend.DEVICE,
                     }

        if not rng_type in rng_dict.keys():
            msg = ("'rng_type' input must be one of the supported types." +
                   "Check the documentation for the list of supported types." +
                   "Default is 'MT' (Mersenne-Twister) [std::mt19937].")
            raise Error(msg)

        return rng_dict[rng_type]
        # }}}

    def __checkStartTime(self, start_time, default):
        """
        Private helper function to check the start time for KMC simulation.
        """
        if start_time is None:
            return default

        checkPositiveFloat(start_time, default, "start_time")

        return start_time

    def __checkExtraTraj(self, extra_traj, default):
        """
        Private helper function to check the extra trajectory output control parameter.
        """
        # {{{
        if extra_traj is None:
            return default

        checkSequenceOf(extra_traj, int, msg="'extra_traj' is not a sequence of int")

        start, end, interval = extra_traj
        msg = "Bad extra trajectory paramter: {}".format(extra_traj)
        for i in extra_traj:
            if i < 0:
                raise Error(msg)
        if start >= end:
            raise Error(msg)
        if interval > (end - start):
            raise Error(msg)

        return extra_traj
        # }}}

    def __checkFastSpecies(self, fast_species, default):
        """
        Private helper function to check default fast species.
        """
        if fast_species is None:
            return []

        msg = "The parameter 'fast_species' must be a sequence of strings."
        fast_species = checkSequenceOf(fast_species, str, msg)

        # In case that a str is passed in.
        if not hasattr(fast_species, "__iter__"):
            raise Error(msg)

        return fast_species

    def __checkNsplits(self, nsplits, default):
        if nsplits is None:
            return default

        nsplits = checkSequenceOfPositiveIntegers(nsplits, "nsplits")

        if len(nsplits) != 3:
            msg = "Length of nsplits must be equal to 3."
            raise Error(msg)

        return nsplits

    def timeLimit(self):
        """
        Query for the time upper bound limit.

        :returns: The time limit.
        """
        return self.__time_limit

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

    def startTime(self):
        """
        Query for the start time.
        """
        return self.__start_time

    def extraTraj(self):
        """
        Query for extra trojectory setting paramter.
        """
        return self.__extra_traj

    def doRedistribution(self):
        """
        Query for the flag for redistribution operation.
        """
        return self.__do_redistribution

    def redistributionInterval(self):
        """
        Query for the interval of redistribution operation.
        """
        return self.__redistribution_interval

    def fastSpecies(self):
        """
        Query for the default fast species.
        """
        return self.__fast_species

    def nsplits(self):
        """
        Query for the split number.
        """
        return self.__nsplits

