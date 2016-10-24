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
 *  zjshao     2016-10-24   1.4          Inital creation.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#include <ctime>
#include <random>

#include "distributor.h"
#include "configuration.h"
#include "matcher.h"


// ----------------------------------------------------------------------------
//
void Distributor::reDistribute(Configuration & configuration) const
{
    // {{{

    // Get the PRIVATE member variables of Configuration.
    std::vector<int> & types = configuration.types_;
    std::vector<int> & atom_id = configuration.atom_id_;
    std::vector<std::string> & elements = configuration.elements_;

    const std::vector<bool> & slow_flags = configuration.slowFlags();

    // Extract all fast species to a list.
    std::vector<int> fast_types;
    std::vector<int> fast_atom_id;
    std::vector<std::string> fast_elements;

    std::vector<int> fast_indices;

    for (size_t i = 0; i < slow_flags.size(); ++i)
    {
        if (!slow_flags[i])
        {
            fast_indices.push_back(i);
            fast_atom_id.push_back(atom_id[i]);
            fast_types.push_back(types[i]);
            fast_elements.push_back(elements[i]);
        }
    }

    // Number of fast species.
    const int n_fast = fast_indices.size();

    // Initialize random number generator and distribution.
    static std::default_random_engine generator(time(NULL));
    static std::uniform_int_distribution<int> distribution(0, n_fast-1);

    // Re-distribution.
    for (int i = 0; i < n_fast; ++i)
    {
        // Generate a random integer in 0 ~ n_fast.
        int randn = distribution(generator);

        // Put the random selected species into configuration.
        int index = fast_indices[i];
        configuration.types_[index] = fast_types[randn];
        configuration.atom_id_[index] = fast_atom_id[randn];
        configuration.elements_[index] = fast_elements[randn];
    }

    // }}}
}

