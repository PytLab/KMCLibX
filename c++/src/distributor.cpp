/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLibX project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : distributor.cpp
 *  brief  : File for the implementation code of the Distributor class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-10-24   1.4          Initial creation.
 *  zjshao     2016-11-22   1.4          Add new redistribute method.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#include <iostream>
#include <cmath>
#include <queue>
#include <cstdio>
#include <string>

#include "distributor.h"
#include "configuration.h"
#include "matcher.h"
#include "random.h"
#include "interactions.h"
#include "latticemap.h"

#ifdef DEBUG
#include <cassert>
#endif  // DEBUG


// ----------------------------------------------------------------------------
// TODO: OpenMp
//
std::vector<int> RandomDistributor::redistribute(Configuration & configuration) const
{
    // {{{

    // Get the PRIVATE member variables of Configuration.
    std::vector<int> & types = configuration.types_;
    std::vector<int> & atom_id = configuration.atom_id_;
    std::vector<std::string> & elements = configuration.elements_;

    const std::vector<bool> & slow_flags = configuration.slowFlags();
    const std::vector<int> & global_indices = configuration.globalIndices();

    // Extract all fast species to a list.
    std::vector<int> fast_types;
    std::vector<int> fast_atom_id;
    std::vector<std::string> fast_elements;
    std::vector<int> fast_global_indices;
    std::vector<int> fast_local_indices;

    for (size_t i = 0; i < slow_flags.size(); ++i)
    {
        if (!slow_flags[i])
        {
            fast_local_indices.push_back(i);
            fast_global_indices.push_back(global_indices[i]);
            fast_atom_id.push_back(atom_id[i]);
            fast_types.push_back(types[i]);
            fast_elements.push_back(elements[i]);
        }
    }

    // Number of fast species.
    const int n_fast = fast_local_indices.size();

    // Initialize a indices vector to shuffle other vectors.
    std::vector<int> shuffle_indices;
    for (int i = 0; i < n_fast; ++i)
    {
        shuffle_indices.push_back(i);
    }

    // Shuffle the indices vector.
    shuffleIntVector(shuffle_indices);

    for (int i = 0; i < n_fast; ++i)
    {
        // Index in the original vector.
        const int index = shuffle_indices[i];

        // Index in the configuration.
        const int config_index = fast_local_indices[i];

        // Put the shuffled entries into configuration.
        types[config_index] = fast_types[index];
        atom_id[config_index] = fast_atom_id[index];
        elements[config_index] = fast_elements[index];
    }

    return fast_global_indices;

    // }}}
}

bool RandomDistributor::metropolisAccept(int site_index,
                                         const std::vector<int> env_local_indices,
                                         const Configuration & configuration,
                                         const LatticeMap & latticemap) const
{
    const std::vector<std::string> & elements = configuration.elements_;
    const std::vector<int> neighbour_indices = latticemap.neighbourIndices(site_index);
    std::vector<int> env_global_indices {};

    double delta_E = 0.0;

    for (const auto local_idx : env_local_indices)
    {
        env_global_indices.push_back(neighbour_indices[local_idx]);
    }

    // Calculate new adsorption energy
    for (int env_idx : env_global_indices)
    {
        const std::string & env_element = elements[env_idx];
        if (env_element == "O")
        {
            delta_E += 0.18;
        }
        else if (env_element == "C")
        {
            delta_E += 0.08;
        }
    }

    // Metropolis Acceptance check.
    if (delta_E> 0.0)
    {
        const double T = 500.0;
        const double kB = 8.6173324e-5;
        double acc_prob = std::exp(static_cast<float>(-delta_E/(kB*T)));
        double randn = randomDouble01();
        if (randn > acc_prob)
        {
            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
//
std::vector<int> RandomDistributor::processRedistribute(Configuration & configuration,
                                                        Interactions & interactions,
                                                        const SitesMap & sitesmap,
                                                        const LatticeMap & latticemap,
                                                        const Matcher & matcher,
                                                        const std::string & replace_species) const
{
    // {{{

    // Extract all fast species.
    const std::vector<std::string> && redist_species = interactions.redistSpecies();
    std::vector<std::string> extracted_species = {};
    std::vector<int> extracted_indices = {};
    configuration.extractFastSpecies(redist_species,
                                     replace_species,
                                     extracted_species,
                                     extracted_indices);

    // Run the rematching of the affected sites of extraction.
    std::vector<int> && matching_indices = \
        latticemap.supersetNeighbourIndices(extracted_indices,
                                            interactions.maxRange());

    matcher.calculateMatching(interactions,
                              configuration,
                              sitesmap,
                              latticemap,
                              matching_indices);

    // The indices list to be extended.
    std::vector<int> & all_affected_indices = extracted_indices;

    // Scatter the extracted species.
    const std::vector<int> && space_indices = configuration.fastIndices();

    std::vector<int> && affected_indices = scatterSpecies(extracted_species,
                                                               space_indices,
                                                               configuration,
                                                               interactions,
                                                               sitesmap,
                                                               latticemap,
                                                               matcher);

    // Merge all affected indices.
    all_affected_indices.insert(all_affected_indices.end(),
                                affected_indices.begin(),
                                affected_indices.end());

    return all_affected_indices;
    // }}}
}


// ----------------------------------------------------------------------------
//
std::vector<int> RandomDistributor::scatterSpecies(std::vector<std::string> & species,
                                                   const std::vector<int> & space_indices,
                                                   Configuration & configuration,
                                                   Interactions & interactions,
                                                   const SitesMap & sitesmap,
                                                   const LatticeMap & latticemap,
                                                   const Matcher & matcher) const
{
    // {{{

    // List to collect all affected indices.
    std::vector<int> all_affected_indices = {};
    std::vector<int> shuffle_space_indices;
    
    // Re-scatter the extracted species.
    for (const std::string & sp : species)
    {
        // Shuffle the space indices.
        std::vector<int> shuffle_space_indices = space_indices;
        shuffleIntVector(shuffle_space_indices);

        // Flag for successful species scattering.
        bool scatter_success = false;
        for (const int site_index : shuffle_space_indices)
        {
            if (scatter_success)
            {
                break;
            }

            // Get all redistribution process and shuffle them.
            std::vector<Process *> redist_process_ptrs = interactions.redistProcesses();
            shuffleProcessPtrVector(redist_process_ptrs);

            // Loop over all redistribution process to check if it can happen here.
            for ( const auto & process_ptr : redist_process_ptrs )
            {
                // Species and location matching.
                if ( sp == process_ptr->redistSpecies() && \
                        process_ptr->isListed(site_index))
                {
                    // Throw the species at the index.
                    configuration.performProcess(*process_ptr, site_index);
                    // Re-matching the affected indices.
                    const std::vector<int> & affected_indices = process_ptr->affectedIndices();
#ifdef DEBUG
                    assert(affected_indices.size() == 1 && affected_indices[0] == site_index);
#endif // DEBUG
                    const std::vector<int> && matching_indices = \
                        latticemap.supersetNeighbourIndices(affected_indices,
                                                            interactions.maxRange());
                    // Extend all affected indices.
                    all_affected_indices.insert(all_affected_indices.end(),
                                                affected_indices.begin(),
                                                affected_indices.end());

                    // Re-match the affected indices.
                    matcher.calculateMatching(interactions,
                                              configuration,
                                              sitesmap,
                                              latticemap,
                                              matching_indices);

                    // Re-classify configuration.
                    //matcher.classifyConfiguration(interactions,
                    //                              configuration,
                    //                              sitesmap,
                    //                              latticemap,
                    //                              matching_indices,
                    //                              {},
                    //                              slow_indices);

                    // Set flag and jump out.
                    scatter_success = true;
                    break;
                }
            }
        }
    }

    return all_affected_indices;
    // }}}
}

// ----------------------------------------------------------------------------
//
std::vector<int> RandomDistributor::scatterSpeciesMetro(std::vector<std::string> & species,
                                                        const std::vector<int> & space_indices,
                                                        Configuration & configuration,
                                                        Interactions & interactions,
                                                        const SitesMap & sitesmap,
                                                        const LatticeMap & latticemap,
                                                        const Matcher & matcher) const
{
    // {{{

    // List to collect all affected indices.
    std::vector<int> all_affected_indices = {};
    // Shuffle the space indices.
    std::vector<int> shuffle_space_indices = space_indices;
    shuffleIntVector(shuffle_space_indices);
    // Space indices queue.
    std::queue<int, std::deque<int>> space_indices_queue(std::deque<int>(shuffle_space_indices.begin(),
                                                                         shuffle_space_indices.end()));
    
    // Local env indices.
    std::vector<int> env_local_indices {10, 16, 22, 34, 40, 46};

    // Re-scatter the extracted species.
    for (const std::string & sp : species)
    {

        // Flag for successful species scattering.
        bool scatter_success = false;
        //for (const int site_index : shuffle_space_indices)
        while (!scatter_success)
        {
            // Pop up a site index.
            const int site_index = space_indices_queue.front();
            space_indices_queue.pop();

            // Get all redistribution process and shuffle them.
            std::vector<Process *> redist_process_ptrs = interactions.redistProcesses();
            shuffleProcessPtrVector(redist_process_ptrs);

            // Loop over all redistribution process to check if it can happen here.
            for ( const auto & process_ptr : redist_process_ptrs )
            {
                bool metropolis_accepted = metropolisAccept(site_index,
                                                            env_local_indices,
                                                            configuration,
                                                            latticemap);
                // Species and location matching.
                if ( sp == process_ptr->redistSpecies() && \
                        process_ptr->isListed(site_index) &&\
                        metropolis_accepted)
                {
                    // Throw the species at the index.
                    configuration.performProcess(*process_ptr, site_index);
                    // Re-matching the affected indices.
                    const std::vector<int> & affected_indices = process_ptr->affectedIndices();
                    const std::vector<int> && matching_indices = \
                        latticemap.supersetNeighbourIndices(affected_indices,
                                                            interactions.maxRange());
                    // Extend all affected indices.
                    all_affected_indices.insert(all_affected_indices.end(),
                                                affected_indices.begin(),
                                                affected_indices.end());

                    // Re-match the affected indices.
                    matcher.calculateMatching(interactions,
                                              configuration,
                                              sitesmap,
                                              latticemap,
                                              matching_indices);

                    // Re-classify configuration.
                    //matcher.classifyConfiguration(interactions,
                    //                              configuration,
                    //                              sitesmap,
                    //                              latticemap,
                    //                              matching_indices,
                    //                              {},
                    //                              slow_indices);

                    // Set flag and jump out.
                    scatter_success = true;
                    break;
                }
            }
            if (!scatter_success)
            {
                space_indices_queue.push(site_index);
            }
        }
    }

    return all_affected_indices;
    // }}}
}

// ----------------------------------------------------------------------------
//
double RandomDistributor::calcInteractionEnergy(const Configuration & configuration,
                                                const LatticeMap & latticemap,
                                                const std::vector<int> & env_local_indices) const
{
    double E = 0.0;
    const std::vector<std::string> & elements = configuration.elements_;

    for (const auto & idx: configuration.indices())
    {
        const std::string & element = elements[idx];
        if (element == "C")
        {
            std::vector<int> env_global_indices {};
            const std::vector<int> & neighbour_indices = latticemap.neighbourIndices(idx);
            for (const auto & local_idx : env_local_indices)
            {
                env_global_indices.push_back(neighbour_indices[local_idx]);
            }

            for (const auto & env_idx : env_global_indices)
            {
                const std::string & env_element = elements[env_idx];
                if (env_element == "O")
                {
                    E += 0.18;
                }
                else if (env_element == "C")
                {
                    E += 0.08;
                }
            }
        }
    }

    return E;
}

// ----------------------------------------------------------------------------
//
void ConstrainedRandomDistributor:: \
updateLocalFromSubConfig(Configuration & global_config,
                         const SubConfiguration & sub_config) const
{
    // {{{

    // Get global indices.
    const std::vector<int> & global_indices = sub_config.globalIndices();

    // Update local info in global configuration.
    for (size_t i = 0; i < global_indices.size(); ++i)
    {
        int global_index = global_indices[i];

        // Use at() to do bound check here.
        global_config.types_.at(global_index) = sub_config.types()[i];
        global_config.elements_.at(global_index) = sub_config.elements()[i];
        global_config.atom_id_.at(global_index) = sub_config.atomID()[i];
    }

    // }}}
}


// ----------------------------------------------------------------------------
//
std::vector<int> ConstrainedRandomDistributor:: \
constrainedRedistribute(Configuration & configuration,
                        const LatticeMap & lattice_map,
                        int x, int y, int z) const
{
    // {{{

    // Split global configuration to sub-configuraitons.
    std::vector<SubConfiguration> && sub_configs = configuration.split(lattice_map,
                                                                       x, y, z);
    // Loop over all sub-configurations to update global configuration.
    std::vector<int> fast_indices(0);
    for (SubConfiguration & sub_config : sub_configs)
    {
        // Re-distribute sub-configuration.
        std::vector<int> sub_fast_indices = redistribute(sub_config);
        // Update local configuration.
        updateLocalFromSubConfig(configuration, sub_config);
        // Insert sub_fast_indices to total fast indices.
        fast_indices.insert(fast_indices.end(),
                            sub_fast_indices.begin(),
                            sub_fast_indices.end());
    }

    return fast_indices;

    // }}}
}


// ----------------------------------------------------------------------------
//
std::vector<int> ConstrainedRandomDistributor:: \
    constrainedProcessRedistribute(Configuration & configuration,
                                   Interactions & interactions,
                                   const SitesMap & sitesmap,
                                   const LatticeMap & latticemap,
                                   const Matcher & matcher,
                                   const std::string & replace_species,
                                   int x, int y, int z,
                                   bool metropolis_acceptance) const
{
    // {{{
    
    double ori_energy = 0.0;
    std::vector<int> env_local_indices;
    std::vector<int> ori_types;
    std::vector<int> ori_atom_id;
    std::vector<std::string> ori_elements;

    if (metropolis_acceptance)
    {
        // Calculate original interaction energy.
        env_local_indices = {10, 16, 22, 34, 40, 46};
        ori_energy = calcInteractionEnergy(configuration,
                                           latticemap,
                                           env_local_indices);
        // Backup original configuration members
        ori_types = configuration.types_;
        ori_atom_id = configuration.atom_id_;
        ori_elements = configuration.elements_;
    }

    std::vector<SubConfiguration> && sub_configs = configuration.split(latticemap,
                                                                       x, y, z);

    const std::vector<std::string> && redist_species = interactions.redistSpecies();

    // Loop over all sub-configurations to collect essential info.
    std::vector<std::vector<std::string> > all_extracted_species = {};
    std::vector<int> extracted_global_indices = {};

    for (SubConfiguration & sub_config : sub_configs)
    {
        // Extract fast species from sub-configuration.
        std::vector<std::string> extracted_species = {};
        std::vector<int> extracted_local_indices = {};
        sub_config.extractFastSpecies(redist_species,
                                      replace_species,
                                      extracted_species,
                                      extracted_local_indices);

        // Collect fast species.
        all_extracted_species.push_back(extracted_species);

        // Update the global configuration.
        updateLocalFromSubConfig(configuration, sub_config);

        // Collect global affected indices.
        for ( const int & local_index : extracted_local_indices)
        {
            int global_index = sub_config.globalIndices()[local_index];
            extracted_global_indices.push_back(global_index);
        }
    }

    // Run the rematching of the affected sites of extraction.
    std::vector<int> && matching_indices = \
        latticemap.supersetNeighbourIndices(extracted_global_indices,
                                            interactions.maxRange());

    matcher.calculateMatching(interactions,
                              configuration,
                              sitesmap,
                              latticemap,
                              matching_indices);

    // The indices list to be extended.
    std::vector<int> & all_affected_indices = extracted_global_indices;

    for (size_t i = 0; i < sub_configs.size(); ++i)
    {
        std::vector<std::string> extracted_local_species = all_extracted_species[i];
        const std::vector<int> & local_space_indices = sub_configs[i].globalIndices();

        // Scatter in the local part of configuration.
        std::vector<int> && affected_indices = scatterSpecies(extracted_local_species,
                                                              local_space_indices,
                                                              configuration,
                                                              interactions,
                                                              sitesmap,
                                                              latticemap,
                                                              matcher);
        // Collect affected indices.
        all_affected_indices.insert(all_affected_indices.end(),
                                    affected_indices.begin(),
                                    affected_indices.end());
    }

    if (metropolis_acceptance)
    {
        // Calculate current interaction energy.
        const double cur_energy = calcInteractionEnergy(configuration,
                                                        latticemap,
                                                        env_local_indices);
        const double delta = cur_energy - ori_energy;

        if (delta > 0.0)
        {
            const double T = 500.0;
            const double kB = 8.6173324e-5;
            double acc_prob = std::exp(static_cast<float>(-delta/(kB*T)));
            double randn = randomDouble01();
            if (randn > acc_prob)
            {
                // Not accepted, revert configuration.
                configuration.types_ = ori_types;
                configuration.atom_id_ = ori_atom_id;
                configuration.elements_ = ori_elements;

                // Rematching all affected indices.
                matcher.calculateMatching(interactions,
                                          configuration,
                                          sitesmap,
                                          latticemap,
                                          all_affected_indices);
                return {};
            }
        }
    }

    return all_affected_indices;

    // }}}
}

