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

try:
    import mpi4py.MPI as MPI
    mpi_comm = MPI.COMM_WORLD
    mpi_rank = mpi_comm.Get_rank()
except ImportError:
    mpi_rank = 0

mpi_master = (mpi_rank == 0)

from CoreComponents.KMCLocalConfiguration import KMCLocalConfiguration
from CoreComponents.KMCInteractions import KMCInteractions
from CoreComponents.KMCProcess import KMCProcess
from CoreComponents.KMCConfiguration import KMCConfiguration
from CoreComponents.KMCSitesMap import KMCSitesMap
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
           'KMCLattice', 'KMCLatticeModel', 'KMCUnitCell', 'KMCSitesMap',
           'KMCControlParameters', 'KMCInteractionsFromScript',
           'KMCConfigurationFromScript', 'KMCRateCalculatorPlugin',
           'KMCAnalysisPlugin', 'KMCProcess', 'OnTheFlyMSD',
           'TimeStepDistribution', 'MPICommons']

# Trick to initialize and finalize MPI only once.
MPICommons.init()

# Print the header when the module is loaded.
printHeader()


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

