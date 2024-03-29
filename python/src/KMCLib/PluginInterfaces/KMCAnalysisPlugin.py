""" Module for the KMCAnalysisPlugin class """


# Copyright (c)  2013  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


class KMCAnalysisPlugin(object):
    """
    Class for providing an interface to easily extend and customize
    the behaviour of the on-the-fly analysis functionality of KMCLib.
    """

    def __init__(self):
        """
        The constructor of the base-class.
        """
        pass

    def setup(self, step, time, configuration, interactions=None):
        """
        Function called right before the start of the KMC loop to allow for
        custom setup of the analysis object.

        :param step: The simulation step number.
        :type step: int

        :param time: The simulation time.
        :type time: float

        :param configuration: The up to date configuration of the simulation.
        :type configuration: KMCConfiguration

        :param interactions: The up to date interactions of the simulation,
                             the default is None.
        :type interactions: KMCInteractions
        """
        pass

    def registerStep(self, step, time, configuration, interactions=None):
        """
        Called from the KMC loop after each step.

        :param step: The simulation step number.
        :type step: int

        :param time: The simulation time.
        :type time: float

        :param interactions: The up to date interactions of the simulation,
                             the default is None.
        :type interactions: KMCInteractions
        """
        pass

    def finalize(self):
        """
        Called after the KMC loop to allow for custom finalization and
        post-processing of collected data.
        """
        pass
