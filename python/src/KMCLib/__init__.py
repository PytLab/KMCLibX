""" Module for easy KMC from python. """


# Copyright (c)  2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLibX project(based on KMCLib) distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

import sys

if sys.version > "3":
    PY2 = False
else:
    PY2 = True

__author__ = "Shao Zhengjiang"
__copyright__ = "Copyright (C)  2016-2019  Shao Zhengjiang"
__version__ = "2.0.0"
__license__ = "GPLv3.0"
__maintainer__ = __author__
__email__ = "shaozhengjiang@gmail.com"

from KMCLib.CoreComponents.KMCLocalConfiguration import KMCLocalConfiguration
from KMCLib.CoreComponents.KMCInteractions import KMCInteractions
from KMCLib.CoreComponents.KMCProcess import KMCProcess
from KMCLib.CoreComponents.KMCConfiguration import KMCConfiguration
from KMCLib.CoreComponents.KMCSitesMap import KMCSitesMap
from KMCLib.CoreComponents.KMCLattice import KMCLattice
from KMCLib.CoreComponents.KMCLatticeModel import KMCLatticeModel
from KMCLib.CoreComponents.KMCUnitCell import KMCUnitCell
from KMCLib.CoreComponents.KMCControlParameters import KMCControlParameters
from KMCLib.Analysis.OnTheFlyMSD import OnTheFlyMSD
from KMCLib.Analysis.TimeStepDistribution import TimeStepDistribution
from KMCLib.Utilities.SaveAndReadUtilities import KMCInteractionsFromScript
from KMCLib.Utilities.SaveAndReadUtilities import KMCConfigurationFromScript
from KMCLib.PluginInterfaces.KMCRateCalculatorPlugin import KMCRateCalculatorPlugin
from KMCLib.PluginInterfaces.KMCAnalysisPlugin import KMCAnalysisPlugin
from KMCLib.Backend.Backend import MPICommons
from KMCLib.Utilities.PrintUtilities import printHeader

__all__ = ['KMCLocalConfiguration', 'KMCInteractions', 'KMCConfiguration',
           'KMCLattice', 'KMCLatticeModel', 'KMCUnitCell', 'KMCSitesMap',
           'KMCControlParameters', 'KMCInteractionsFromScript',
           'KMCConfigurationFromScript', 'KMCRateCalculatorPlugin',
           'KMCAnalysisPlugin', 'KMCProcess', 'OnTheFlyMSD',
           'TimeStepDistribution', 'MPICommons']

# Trick to initialize and finalize MPI only once.
MPICommons.init()

# Print the header when the module is loaded.
#printHeader()

# Instantialize logger.
import logging

logger = logging.getLogger("KMCLibX")
logger.setLevel(logging.INFO)

# Set logger handlers.
std_hdlr = logging.FileHandler("out.log")
std_hdlr.setLevel(logging.DEBUG)
console_hdlr = logging.StreamHandler()
console_hdlr.setLevel(logging.INFO)

# Create formatter and add it to the handlers.
formatter = logging.Formatter('%(name)s   %(levelname)-8s %(message)s')
std_hdlr.setFormatter(formatter)
console_hdlr.setFormatter(formatter)

# Add the handlers to logger.
logger.addHandler(std_hdlr)
logger.addHandler(console_hdlr)


# Make sure to finalize MPI on exit.
def killme():
    MPICommons.finalize()

import atexit
atexit.register(killme)

