""" Module for easy KMC from python. """


# Copyright (c)  2013  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


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

__all__ = ['KMCLocalConfiguration', 'KMCInteractions', 'KMCConfiguration',
           'KMCLattice', 'KMCLatticeModel', 'KMCUnitCell',
           'KMCControlParameters', 'KMCInteractionsFromScript',
           'KMCConfigurationFromScript', 'KMCRateCalculatorPlugin',
           'KMCAnalysisPlugin', 'KMCProcess', 'OnTheFlyMSD',
           'TimeStepDistribution', 'MPICommons']

# Trick to initialize and finalize MPI only once.
MPICommons.init()

# Print the header when the module is loaded.
Utilities.PrintUtilities.printHeader()

# Make sure to finalize MPI on exit.
def killme():
    MPICommons.finalize()

import atexit
atexit.register(killme)

