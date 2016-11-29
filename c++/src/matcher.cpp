/*
  Copyright (c)  2012  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matcher.cpp
 *  brief  : File for the Matcher class implimentation.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-11   1.2          Change match list presentation,
 *                                       Remove the isMatch function.
 *
 *  zjshao     2016-10-20   1.4          Add configuration classification.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#include <cstdio>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <cassert>
#endif

#include "matcher.h"
#include "process.h"
#include "interactions.h"
#include "configuration.h"
#include "latticemap.h"
#include "matchlist.h"
#include "sitesmap.h"

#include "mpicommons.h"
#include "mpiroutines.h"

// -----------------------------------------------------------------------------
//
Matcher::Matcher()
{
    // NOTHING HERE YET
}


// -----------------------------------------------------------------------------
//
// TODO: OpenMP.
//
std::vector<std::pair<int, int> > \
Matcher::indexProcessToMatch(const std::vector<Process *> & process_ptrs,
                             Configuration      & configuration,
                             const SitesMap     & sitesmap,
                             const LatticeMap   & lattice_map,
                             const std::vector<int> & indices) const
{
    // {{{

    // PERFORMME: May use OpenMP here to parallelize the check loop.

    // The pair list to be returned.
    std::vector<std::pair<int, int> > index_process_to_match;

    for (size_t i = 0; i < indices.size(); ++i)
    {
        // Get the index.
        const int index = indices[i];
        bool use_index = false;

        // Get the basis site.
        const int basis_site = lattice_map.basisSiteFromIndex(index);

        // Get site match list.
        const SiteMatchList & site_matchlist = sitesmap.matchList(index);

        // For each process, check if we should try to match.
        for (size_t j = 0; j < process_ptrs.size(); ++j)
        {
            // Check if the basis site is listed.
            const std::vector<int> & process_basis_sites = \
                (*process_ptrs[j]).basisSites();

            if ( std::find(process_basis_sites.begin(),
                           process_basis_sites.end(),
                           basis_site) \
                  != process_basis_sites.end() )
            {
                // Pick out the process.
                const Process * process_ptr = process_ptrs[j];

                // Check if process site types is set.
                if (process_ptr->hasSiteTypes())
                {
                    // Get process match list.
                    const ProcessMatchList & process_matchlist = process_ptr->matchList();

                    // Check if the process matches with site types.
                    bool is_match = whateverMatch(process_matchlist, site_matchlist);

                    if (is_match)
                    {
                        // Register the candidate.
                        index_process_to_match.push_back(std::pair<int, int>(index, j));
                        use_index = true;
                    }
                }
                else
                {
                    // Register the candidate.
                    index_process_to_match.push_back(std::pair<int, int>(index, j));
                    use_index = true;
                }
            }
        }

        // Update the configuration match list for this index if it will be used.
        if ( use_index )
        {
            configuration.updateMatchList(index);
        }
    }

    return index_process_to_match;

    // }}}
}


// -----------------------------------------------------------------------------
//
void Matcher::calculateMatching(Interactions & interactions,
                                Configuration & configuration,
                                const SitesMap & sitesmap,
                                const LatticeMap & lattice_map,
                                const std::vector<int> & indices) const
{
    // {{{

    // Build the list of indices and processes to match.
    const std::vector<Process *> & process_ptrs = interactions.processes();
    const std::vector<std::pair<int,int> > && index_process_to_match = \
        indexProcessToMatch(process_ptrs, configuration, sitesmap,
                            lattice_map, indices);

    // Generate the lists of tasks.
    std::vector<RemoveTask> remove_tasks;
    std::vector<RateTask>   update_tasks;
    std::vector<RateTask>   add_tasks;

    matchIndicesWithProcesses(index_process_to_match,
                              interactions,
                              configuration,
                              remove_tasks,
                              update_tasks,
                              add_tasks);

    // Calculate the new rates in needed.
    if (interactions.useCustomRates())
    {
        // Create a common task list for getting a good load balance.
        std::vector<RateTask> global_tasks(add_tasks.size()+update_tasks.size());
        std::copy( update_tasks.begin(),
                   update_tasks.end(),
                   std::copy(add_tasks.begin(),
                             add_tasks.end(),
                             global_tasks.begin()) );

        // Split up the tasks.
        std::vector<RateTask> local_tasks = splitOverProcesses(global_tasks);
        std::vector<double> local_tasks_rates(local_tasks.size(), 0.0);

        // Update.
        updateRates(local_tasks_rates, local_tasks, interactions, configuration);

        // Join the results.
        const std::vector<double> global_tasks_rates = joinOverProcesses(local_tasks_rates);

        // Copy the results over.
        for (size_t i = 0; i < add_tasks.size(); ++i)
        {
            add_tasks[i].rate = global_tasks_rates[i];
        }

        const size_t offset = add_tasks.size();
        for (size_t i = 0; i < update_tasks.size(); ++i)
        {
            update_tasks[i].rate = global_tasks_rates[offset + i];
        }
    }

    // Update the processes.
    updateProcesses(remove_tasks,
                    update_tasks,
                    add_tasks,
                    interactions);

    // }}}
}

// -----------------------------------------------------------------------------
//
void Matcher::matchIndicesWithProcesses(const std::vector<std::pair<int,int> > & index_process_to_match,
                                        const Interactions  & interactions,
                                        const Configuration & configuration,
                                        std::vector<RemoveTask> & remove_tasks,
                                        std::vector<RateTask> & update_tasks,
                                        std::vector<RateTask> & add_tasks) const
{
    // {{{

    // Setup local variables for running in parallel.
    std::vector< std::pair<int,int> > && local_index_process_to_match = \
        splitOverProcesses(index_process_to_match);

    // These are the local task types to fill with matching restults.
    const int n_local_tasks = local_index_process_to_match.size();
    std::vector<int> local_task_types(n_local_tasks, 0);

    // Loop over pairs to match.
    for (size_t i = 0; i < local_index_process_to_match.size(); ++i)
    {
        // Get the process and index to match.
        const int index = local_index_process_to_match[i].first;
        const int p_idx = local_index_process_to_match[i].second;
        Process & process = (*interactions.processes()[p_idx]);

        // Perform the matching.
        const bool in_list = process.isListed(index);

        const ProcessMatchList & process_match_list = process.matchList();
        const ConfigMatchList & index_match_list = configuration.matchList(index);

        const bool is_match = whateverMatch(process_match_list,
                                            index_match_list);

        // Determine what to do with this pair of processes and indices.
        if (!is_match && in_list)
        {
            // If no match and previous match - remove.
            local_task_types[i] = 1;
        }
        else if (is_match && in_list)
        {
            // If match and previous match - update the rate.
            local_task_types[i] = 2;
        }
        else if (is_match && !in_list)
        {
            // If match and not previous match - add.
            local_task_types[i] = 3;
        }
    }

    // Join the result - parallel.
    const std::vector<int> task_types = joinOverProcesses(local_task_types);

    // Loop again (not in parallel) and add the tasks to the tasks vectors.
    const size_t n_tasks = index_process_to_match.size();
    for (size_t i = 0; i < n_tasks; ++i)
    {
        const int index = index_process_to_match[i].first;
        const int p_idx = index_process_to_match[i].second;
        const Process & process = (*interactions.processes()[p_idx]);

        // If no match and previous match - remove.
        if (task_types[i] == 1)
        {
            RemoveTask t;
            t.index   = index;
            t.process = p_idx;
            remove_tasks.push_back(t);
        }

        // If match and previous match - update the rate.
        else if (task_types[i] == 2)
        {
            RateTask t;
            t.index   = index;
            t.process = p_idx;
            t.rate    = process.rateConstant();
            update_tasks.push_back(t);
        }

        // If match and not previous match - add.
        else if (task_types[i] == 3)
        {
            RateTask t;
            t.index   = index;
            t.process = p_idx;
            t.rate    = process.rateConstant();
            add_tasks.push_back(t);
        }
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
void Matcher::updateProcesses(const std::vector<RemoveTask> & remove_tasks,
                              const std::vector<RateTask>   & update_tasks,
                              const std::vector<RateTask>   & add_tasks,
                              Interactions & interactions) const
{
    // {{{

    // This could perhaps be OpenMP parallelized.

    // Remove.
    for (size_t i = 0; i < remove_tasks.size(); ++i)
    {
        const int index = remove_tasks[i].index;
        const int p_idx = remove_tasks[i].process;
        interactions.processes()[p_idx]->removeSite(index);
    }

    // Update.
    for (size_t i = 0; i < update_tasks.size(); ++i)
    {
        const int index   = update_tasks[i].index;
        const int p_idx   = update_tasks[i].process;
        const double rate = update_tasks[i].rate;
        interactions.processes()[p_idx]->removeSite(index);
        interactions.processes()[p_idx]->addSite(index, rate);
    }

    // Add.
    for (size_t i = 0; i < add_tasks.size(); ++i)
    {
        const int index   = add_tasks[i].index;
        const int p_idx   = add_tasks[i].process;
        const double rate = add_tasks[i].rate;
        interactions.processes()[p_idx]->addSite(index, rate);
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
void Matcher::updateRates(std::vector<double>         & new_rates,
                          const std::vector<RateTask> & tasks,
                          const Interactions          & interactions,
                          const Configuration         & configuration) const
{
    // {{{

    // Use the backendCallBack function on the RateCalculator stored on the
    // interactions object, to get an updated rate for each process.

    const RateCalculator & rate_calculator = interactions.rateCalculator();

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        // Get the rate process to use.
        const Process & process = (*interactions.processes()[tasks[i].process]);

        // Get the coordinate index.
        const int index = tasks[i].index;

        // Send this information to the updateSingleRate function.
        new_rates[i] = updateSingleRate(index, process, configuration, rate_calculator);
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
double Matcher::updateSingleRate(const int index,
                                 const Process        & process,
                                 const Configuration  & configuration,
                                 const RateCalculator & rate_calculator) const
{
    // {{{

    // Get the match lists.
    const ProcessMatchList & process_match_list = process.matchList();
    const ConfigMatchList & config_match_list  = configuration.matchList(index);

    // We will also need the elements.
    const std::vector<std::string> & elements = configuration.elements();

    // Get cutoff distance from the process.
    const double cutoff = process.cutoff();
    ConfigMatchList::const_iterator it1 = config_match_list.begin();
    int len = 0;
    while ( (*it1).distance <= cutoff && it1 != config_match_list.end())
    {
        ++it1;
        ++len;
    }
    const ConfigMatchList::const_iterator new_end = it1;

    // Allocate memory for the numpy geometry and copy the data over.
    std::vector<double> numpy_geo(len*3);
    std::vector<double>::iterator it_geo = numpy_geo.begin();
    std::vector<std::string> types_before;

    it1 = config_match_list.begin();
    for ( ; it1 != new_end; ++it1 )
    {
        const Coordinate & coord   = (*it1).coordinate;

        (*it_geo) = coord.x();
        ++it_geo;

        (*it_geo) = coord.y();
        ++it_geo;

        (*it_geo) = coord.z();
        ++it_geo;

        const int idx = (*it1).index;
        types_before.push_back(elements[idx]);
    }

    // Types after the process.
    std::vector<std::string> types_after = types_before;

    // Rewind the config match list iterator.
    it1 = config_match_list.begin();

    // Get the iterators to the process match list and types after.
    ProcessMatchList::const_iterator it2 = process_match_list.begin();
    std::vector<std::string>::iterator it3 = types_after.begin();
    const ProcessMatchList::const_iterator end = process_match_list.end();

    // Loop over the process match list and update the types_after vector.
    for ( ; it2 != end; ++it1, ++it2, ++it3 )
    {
        const int update_type = (*it2).update_type;
        const int match_type  = (*it1).match_type;

        // Set the type after process. NOTE: The > 0 is needed for handling wildcards.
        if ( match_type != update_type  && update_type > 0)
        {
            (*it3) = configuration.typeName(update_type);
        }
    }

    // Calculate the rate using the provided rate calculator.
    const double rate_constant = process.rateConstant();
    const int process_number   = process.processNumber();
    const double global_x = configuration.coordinates()[index].x();
    const double global_y = configuration.coordinates()[index].y();
    const double global_z = configuration.coordinates()[index].z();

    return rate_calculator.backendRateCallback(numpy_geo,
                                               len,
                                               types_before,
                                               types_after,
                                               rate_constant,
                                               process_number,
                                               global_x,
                                               global_y,
                                               global_z);

    // }}}
}


// -----------------------------------------------------------------------------
//
// TODO: MPI
void Matcher::classifyConfiguration(const Interactions & interactions,
                                    Configuration      & configuration,
                                    const SitesMap     & sitesmap,
                                    const LatticeMap   & lattice_map,
                                    const std::vector<int> & indices,
                                    const std::vector<std::string> & fast_elements,
                                    const std::vector<int> & slow_indices) const
{
    // {{{

    // NOTE: No sites type checking here, we assume that
    //       all sites are equivalent, sitesmap may be
    //       used in the future, if needed.

    // Reset slow flags in configuration.
    configuration.resetSlowFlags(fast_elements);

    // Get the list of indices and process to match.
    const std::vector<Process *> & fast_process_ptrs = interactions.fastProcesses();
    const std::vector<std::pair<int, int> > && index_process_to_match = \
        indexProcessToMatch(fast_process_ptrs, configuration, sitesmap,
                            lattice_map, indices);

    // Loop over all indices and processes.
    for (const auto & idx_proc : index_process_to_match)
    {
        const int conf_idx = idx_proc.first;
        const int proc_idx = idx_proc.second;

        // Get configuration and process matchlists.
        Process & process = *(fast_process_ptrs[proc_idx]);
        const ProcessMatchList & process_matchlist = process.matchList();
        const ConfigMatchList & config_matchlist = configuration.matchList(conf_idx);

        // Check matching.
        bool in_list = process.isListed(conf_idx);
        if (!in_list)
        {
            continue;
        }

#ifdef DEBUG
        // Assertion match and inlist consistency here
        // just in case that there are bugs in interaction updating.
        const bool is_match = whateverMatch(process_matchlist, config_matchlist);
        assert(is_match && in_list);
#endif

        // Vars for match lists loop.
        auto proc_it = process_matchlist.begin();
        auto conf_it = config_matchlist.begin();

        // Loop over the match lists to update slow flags of configuration.
        for (; proc_it != process_matchlist.end(); ++proc_it, ++conf_it)
        {
            // The match type and updated type in process.
            const int match_type = (*proc_it).match_type;
            const int update_type = (*proc_it).update_type;

            // The index in the global structure.
            const int index = (*conf_it).index;

            // Get the affected index.
            if (match_type != update_type)
            {
                configuration.updateSlowFlag(index, false);
            }
        }
    }

    // Reset the custom slow flags.
    if ( !slow_indices.empty() )
    {
        for (int slow_index : slow_indices)
        {
            configuration.updateSlowFlag(slow_index, true);
        }
    }

#ifdef DEBUG
    // Check the species can be classified correctly.
    for (size_t i = 0; i < configuration.slowFlags().size(); ++i)
    {
        if (i > 0 && i % 50 == 0)
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << configuration.slowFlags()[i];
        }
    }
#endif

    // }}}
}

