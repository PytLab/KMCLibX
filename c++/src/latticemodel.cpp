/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : latticemodel.cpp
 *  brief  : File for the implementation code of the LatticeModel class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-11-01   1.4          Add redistribution.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#include "latticemodel.h"
#include "configuration.h"
#include "simulationtimer.h"
#include "random.h"
#include "sitesmap.h"

#include <cstdio>

// -----------------------------------------------------------------------------
//
LatticeModel::LatticeModel(Configuration & configuration,
                           SitesMap & sitesmap,
                           SimulationTimer & simulation_timer,
                           const LatticeMap & lattice_map,
                           Interactions & interactions) :
    configuration_(configuration),
    sitesmap_(sitesmap),
    simulation_timer_(simulation_timer),
    lattice_map_(lattice_map),
    interactions_(interactions)
{
    // Setup the mapping between coordinates and processes.
    calculateInitialMatching();

    // Initialize the interactions table here.
    interactions_.updateProbabilityTable();

    // Initialize the process available sites.
    interactions_.updateProcessAvailableSites();
}


// -----------------------------------------------------------------------------
//
void LatticeModel::calculateInitialMatching()
{
    // Calculate the match lists of configuration.
    configuration_.initMatchLists(lattice_map_, interactions_.maxRange());

    // Calculate the match lists of sitesmap.
    sitesmap_.initMatchLists(lattice_map_, interactions_.maxRange());

    // Update the interactions matchlists.
    interactions_.updateProcessMatchLists(configuration_, lattice_map_);

    // Match all centeres.
    std::vector<int> indices;

    for(size_t i = 0; i < configuration_.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher_.calculateMatching(interactions_,
                               configuration_,
                               sitesmap_,
                               lattice_map_,
                               indices);
}


// -----------------------------------------------------------------------------
//
void LatticeModel::singleStep()
{
    // Select a process.
    Process & process = (*interactions_.pickProcess());

    // Select a site.
    const int site_index = process.pickSite();

    // Perform the operation.
    configuration_.performProcess(process, site_index);

    // Propagate the time.
    simulation_timer_.propagateTime(interactions_.totalRate());

    // Run the re-matching of the affected sites and their neighbours.
    const std::vector<int> && indices = \
        lattice_map_.supersetNeighbourIndices(process.affectedIndices(),
                                              interactions_.maxRange());

    matcher_.calculateMatching(interactions_,
                               configuration_,
                               sitesmap_,
                               lattice_map_,
                               indices);

    // Update the interactions' probability table.
    interactions_.updateProbabilityTable();

    // Update the interactions' process available sites.
    interactions_.updateProcessAvailableSites();
}

// ----------------------------------------------------------------------------
//
const std::vector<int> \
LatticeModel::redistribute(const std::vector<std::string> & fast_species,
                           const std::vector<int> & slow_indices,
                           int x, int y, int z)
{
    // Classify species in current configuration.
    matcher_.classifyConfiguration(interactions_,
                                   configuration_,
                                   sitesmap_,
                                   lattice_map_,
                                   configuration_.indices(),
                                   fast_species,
                                   slow_indices);

    // Re-distribute the current configuration.
    const std::vector<int> affected_indices = \
        distributor_.constrainedRedistribute(configuration_, lattice_map_, x, y, z);

    // Run the re-matching of the affected sites and their neighbours.
    const std::vector<int> && indices = \
        lattice_map_.supersetNeighbourIndices(affected_indices,
                                              interactions_.maxRange());

    matcher_.calculateMatching(interactions_,
                               configuration_,
                               sitesmap_,
                               lattice_map_,
                               indices);

    // Update the interactions' probability table.
    interactions_.updateProbabilityTable();

    // Update the interactions' process available sites.
    interactions_.updateProcessAvailableSites();

    // Return the affected indices.
    return affected_indices;
}


// ----------------------------------------------------------------------------
//
const std::vector<int> \
LatticeModel::redistribute(const std::string & replace_elements,
                           const std::vector<std::string> & fast_species,
                           const std::vector<int> & slow_indices)
{
    // Classify species in current configuration.
    matcher_.classifyConfiguration(interactions_,
                                   configuration_,
                                   sitesmap_,
                                   lattice_map_,
                                   configuration_.indices(),
                                   fast_species,
                                   slow_indices);

    // Re-distribute the current configuration.
    const std::vector<int> affected_indices = \
        distributor_.processRedistribute(configuration_,
                                         interactions_,
                                         sitesmap_,
                                         lattice_map_,
                                         matcher_,
                                         replace_elements);

    // Run the re-matching of the affected sites and their neighbors.
    const std::vector<int> && indices = \
        lattice_map_.supersetNeighbourIndices(affected_indices,
                                              interactions_.maxRange());

    matcher_.calculateMatching(interactions_,
                               configuration_,
                               sitesmap_,
                               lattice_map_,
                               indices);

    // Update the interactions' probability table.
    interactions_.updateProbabilityTable();

    // Update the interactions' process available sites.
    interactions_.updateProcessAvailableSites();

    // Return the affected indices.
    return affected_indices;
}

