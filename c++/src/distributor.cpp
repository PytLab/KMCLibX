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
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#include "distributor.h"
#include "configuration.h"
#include "matcher.h"
#include "random.h"


// ----------------------------------------------------------------------------
// TODO: OpenMp
//
std::vector<int> RandomDistributor::reDistribute(Configuration & configuration) const
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

void PartialRandomDistributor:: \
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
std::vector<int> PartialRandomDistributor::reDistribute(Configuration & configuration,
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
        std::vector<int> sub_fast_indices = reDistribute(sub_config);
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

