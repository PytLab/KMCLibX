/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* *****************************************************************************
 *  file   : interactions.cpp
 *  brief  : File for the implementation code of the Interactions class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ---------------------------------------------------------------------------
 *  zjshao     2016-06-26   1.3          Add picked index & process available
 *                                       sites.
 *  zjshao     2016-10-30   1.4          Add slow/fast process classification
 *  zjshao     2016-11-21   1.4          Add redist process.
 *  ---------------------------------------------------------------------------
 * *****************************************************************************
 */

#include <algorithm>

#include "interactions.h"
#include "random.h"
#include "configuration.h"
#include "latticemap.h"
#include "ratecalculator.h"
#include "process.h"


// -----------------------------------------------------------------------------
// Function for comparing two entries in the probability list.
bool pairComp(const std::pair<double,int> & p1,
              const std::pair<double, int> & p2)
{
    // This skips all processes with no available sites.
    if (p2.second == 0)
    {
        return false;
    }
    else
    {
        return (p1.first < p2.first);
    }
}


// -----------------------------------------------------------------------------
//
Interactions::Interactions(const std::vector<Process> & processes,
                           const bool implicit_wildcards) :
    processes_(processes),
    custom_rate_processes_(0),
    process_pointers_(processes.size(), NULL),
    process_available_sites_(processes.size(), 0),
    implicit_wildcards_(implicit_wildcards),
    use_custom_rates_(false),
    rate_calculator_placeholder_(RateCalculator()),
    rate_calculator_(rate_calculator_placeholder_),
    picked_index_(-1)
{
    // Point the process pointers to the right places.
    for (size_t i = 0; i < processes_.size(); ++i)
    {
        Process * process_ptr = &processes_[i];
        process_pointers_[i] = process_ptr;

        // Classify fast and slow process pointers.
        if ( process_ptr->fast() )
        {
            fast_process_pointers_.push_back(process_ptr);

            // Pointers for redistribution processes.
            if ( process_ptr->redistribution() )
            {
                redist_process_pointers_.push_back(process_ptr);
            }
        }
        else
        {
            slow_process_pointers_.push_back(process_ptr);
        }
    }

    // Initialize probablity table after processes are classified.
    probability_table_.resize(slow_process_pointers_.size(),
                              std::pair<double, int>(0.0, 0));
}


// -----------------------------------------------------------------------------
// NOTE: There is no classification of fast and slow process pointers.
//
Interactions::Interactions(const std::vector<CustomRateProcess> & processes,
                           const bool implicit_wildcards,
                           const RateCalculator & rate_calculator) :
    processes_(0),
    custom_rate_processes_(processes),
    process_pointers_(processes.size(), NULL),
    probability_table_(processes.size(), std::pair<double,int>(0.0,0)),
    process_available_sites_(processes.size(), 0),
    implicit_wildcards_(implicit_wildcards),
    use_custom_rates_(true),
    rate_calculator_(rate_calculator),
    picked_index_(-1)
{
    // Point the process pointers to the right places.
    for (size_t i = 0; i < custom_rate_processes_.size(); ++i)
    {
        Process * process_ptr = &custom_rate_processes_[i];
        process_pointers_[i] = process_ptr;

        // Classify fast and slow process pointers.
        if ( process_ptr->fast() )
        {
            fast_process_pointers_.push_back(process_ptr);

            // Pointers for redistribution processes.
            if ( process_ptr->redistribution() )
            {
                redist_process_pointers_.push_back(process_ptr);
            }
        }
        else
        {
            slow_process_pointers_.push_back(process_ptr);
        }
    }

    // Initialize probablity table after processes are classified.
    probability_table_.resize(slow_process_pointers_.size(),
                              std::pair<double, int>(0.0, 0));
}


// -----------------------------------------------------------------------------
//
int Interactions::maxRange() const
{
    // Loop through all processes and find the largest range in each of them.
    int max_range = 1;

    std::vector<Process*>::const_iterator it1 = process_pointers_.begin();
    for ( ; it1 != process_pointers_.end(); ++it1 )
    {
        max_range = std::max( max_range, (**it1).range() );
    }

    // Return.
    return max_range;
}


// -----------------------------------------------------------------------------
//
void Interactions::updateProcessMatchLists(const Configuration & configuration,
                                           const LatticeMap & lattice_map)
{
    // {{{

    // NOTE: No the site match list checking here, the coordinates in
    //       site match list are assumed to be the same as that in
    //       configuration match list. -- zjshao

    // Skip if we are not using implicit wildcards.
    if (!implicit_wildcards_)
    {
        return;
    }

    // Loop through each process.
    for (size_t i = 0; i < process_pointers_.size(); ++i)
    {
        Process & p = (*process_pointers_[i]);

        // Skip this process unless the size of basis sites is one.
        if ( p.basisSites().size() != 1 )
        {
            continue;
        }

        // Get the match list for this process.
        ProcessMatchList & process_matchlist = p.matchList();

        // Take out the basis position for the process.
        const int  basis_position = p.basisSites()[0];

        // Get the configuration match list for this basis position in the
        // most central cell.
        const int ii    = lattice_map.repetitionsA() / 2;
        const int jj    = lattice_map.repetitionsB() / 2;
        const int kk    = lattice_map.repetitionsC() / 2;
        const int index = lattice_map.indicesFromCell(ii, jj, kk)[basis_position];
        const ConfigMatchList config_matchlist = configuration.matchList(index);

        // Perform the match where we add wildcards to fill the vacancies in the
        // process match list.
        ProcessMatchList::iterator proc_it = process_matchlist.begin();
        ConfigMatchList::const_iterator conf_it = config_matchlist.begin();

        // Insert the wildcards and update the indexing.
        int old_index = 0;
        int new_index = 0;
        std::vector<int> index_mapping(config_matchlist.size());

        for (; proc_it != process_matchlist.end() && conf_it != config_matchlist.end();
               ++proc_it, ++conf_it )
        {
            // Check if there is a match in lattice point.
            if( !(*proc_it).samePoint(*conf_it) )
            {
                // If not matching, add a wildcard entry to proc_it.
                ProcessMatchListEntry wildcard_entry(*conf_it);
                wildcard_entry.match_type = 0;
                wildcard_entry.update_type = 0;
                wildcard_entry.site_type = 0;

                proc_it = process_matchlist.insert(proc_it, wildcard_entry);
                // proc_it now points to the newly inserted position.

                ++new_index;
            }
            else
            {
                // Add the mapping.
                index_mapping[old_index] = new_index;
                ++old_index;
                ++new_index;
           }
        }

        // With this mapping information we can update the process id moves.
        index_mapping.resize(old_index);
        std::vector<std::pair<int,int> > & id_moves = p.idMoves();
        for (size_t j = 0; j < id_moves.size(); ++j)
        {
            const int old_index_first  = id_moves[j].first;
            const int old_index_second = id_moves[j].second;

            id_moves[j].first  = index_mapping[old_index_first];
            id_moves[j].second = index_mapping[old_index_second];
        }
    }

    //}}}
}


// -----------------------------------------------------------------------------
//
int Interactions::totalAvailableSites() const
{
    // Loop through and sum all available sites on all processes.
    size_t sum = 0;
    for (const Process* const & slow_process_pointer : slow_process_pointers_)
    {
        sum += slow_process_pointer->nSites();
    }

    return static_cast<int>(sum);
}


// -----------------------------------------------------------------------------
//
void Interactions::updateProbabilityTable()
{
    // {{{

    // Loop over all processes.
    std::vector<Process *>::const_iterator it1 = slow_process_pointers_.begin();
    std::vector<std::pair<double, int> >::iterator it2 = probability_table_.begin();
    const std::vector<Process *>::const_iterator end = slow_process_pointers_.end();

    double previous_rate = 0.0;
    for ( ; it1 != end; ++it1, ++it2 )
    {
        // Find out its total probability.
        const int n_sites = (**it1).nSites();
        const double total_rate = (**it1).totalRate();
        // Store the probability with the process number in a table.
        (*it2).first = total_rate + previous_rate;
        previous_rate += total_rate;
        // Store the number of available processes to filter out zeroes later.
        (*it2).second = n_sites;
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
void Interactions::updateProcessAvailableSites()
{
    // Loop over all processes.
    std::vector<Process*>::const_iterator it1 = process_pointers_.begin();
    std::vector<int>::iterator it2 = process_available_sites_.begin();
    const std::vector<Process*>::const_iterator end = process_pointers_.end();

    for (; it1 != end; ++it1, ++it2)
    {
        // Update availability for each process.
        *it2 = (*it1)->nSites();
    }
}


// -----------------------------------------------------------------------------
//
int Interactions::pickProcessIndex()
{
    // PERFORMME:
    // This implements the O(N) SSA algorithm.
    // One could consider implementing the O(logN) SSA-GB algorithm,
    // or the O(1) algorithm described in J.Chem.Phys. 128, 205101, (2008)
    // but it is typically the re-matching after an event that sets the
    // limit in terms of scaling with the number of processes.

    // Get a random number between 0.0 and the total imcremented rate.
    const double rnd = randomDouble01() * totalRate();
    const std::pair<double,int> rnd_pair(rnd,1);

    // Find the lower bound - corresponding to the first element for which
    // the accumulated rate is not smaller than rnd, and the number
    // of available processes is larger than zero.
    const std::vector<std::pair<double, int> >::const_iterator begin = \
        probability_table_.begin();
    const std::vector<std::pair<double, int> >::const_iterator end = \
        probability_table_.end();
    const std::vector<std::pair<double, int> >::const_iterator it1 = \
        std::lower_bound(begin, end, rnd_pair, pairComp); 

    // Find the index in the process.
    int picked_index = it1 - begin;
    
    // Update private variable.
    picked_index_ = picked_index;

    return picked_index;
}


// -----------------------------------------------------------------------------
//
Process* Interactions::pickProcess()
{
    const int index = pickProcessIndex();

    // Update the process internal probablility table if needed.
    slow_process_pointers_[index]->updateRateTable();

    return slow_process_pointers_[index];
}


// -----------------------------------------------------------------------------
//
const std::vector<std::string> Interactions::redistSpecies() const
{
    std::vector<std::string> redist_species = {};

    if ( redist_process_pointers_.empty() )
    {
        return redist_species;
    }
    else
    {
        for (const auto process_ptr : redist_process_pointers_)
        {
            redist_species.push_back(process_ptr->redistSpecies());
        }
        return redist_species;
    }
}

