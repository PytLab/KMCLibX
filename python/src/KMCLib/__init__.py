""" Module for easy KMC from python. """


# Copyright (c)  2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project(based on KMCLib) distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

__author__ = "Shao Zhengjiang"
__copyright__ = "Copyright (C)  2016-2019  Shao Zhengjiang"
__version__ = "2.0.0"
__license__ = "GPLv3.0"
__maintainer__ = __author__
__email__ = "shaozhengjiang@gmail.com"


from CoreComponents.KMCLocalConfiguration import KMCLocalConfiguration
from CoreComponents.KMCInteractions import KMCInteractions
from CoreComponents.KMCProcess import KMCProcess
from CoreComponents.KMCConfiguration import KMCConfiguration
from CoreComponents.KMCLattice import KMCLattice
from CoreComponents.KMCLatticeModel import KMCLatticeModel
from CoreComponents.KMCUnitCell import KMCUnitCell
from CoreComponents.KMCControlParameters import KMCControlParameters
from Analysis.OnTheFlyMSD import OnTheFlyMSD
from Analysis.TimeStepDistribution import TimeStepDistribution
from Utilities.SaveAndReadUtilities import KMCInteractionsFromScript
from Utilities.SaveAndReadUtilities import KMCConfigurationFromScript
from PluginInterfaces.KMCRateCalculatorPlugin import KMCRateCalculatorPlugin
from PluginInterfaces.KMCAnalysisPlugin import KMCAnalysisPlugin
from Backend.Backend import MPICommons
from Utilities.PrintUtilities import printHeader


__all__ = ['KMCLocalConfiguration', 'KMCInteractions', 'KMCConfiguration',
           'KMCLattice', 'KMCLatticeModel', 'KMCUnitCell',
           'KMCControlParameters', 'KMCInteractionsFromScript',
           'KMCConfigurationFromScript', 'KMCRateCalculatorPlugin',
           'KMCAnalysisPlugin', 'KMCProcess', 'OnTheFlyMSD',
           'TimeStepDistribution', 'MPICommons']

# Trick to initialize and finalize MPI only once.
MPICommons.init()

# Print the header when the module is loaded.
printHeader()


# Make sure to finalize MPI on exit.
def killme():
    MPICommons.finalize()

import atexit
atexit.register(killme)
