/*
  Copyright (c)  2012  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matcher.h
 *  brief  : File for the definition of the Matcher class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-11   1.2          Change match list presentation,
 *                                       Remove the isMatch function.
 *
 *  zjshao     2016-10-15   1.4          Add fast and slow species
 *                                       classification.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#ifndef __MATCHER__
#define __MATCHER__

#include <vector>

// Forward declarations.
class Interactions;
class Configuration;
class SitesMap;
class Process;
class LatticeMap;
class RateCalculator;

/// A minimal struct for representing a task with a rate.
struct RateTask
{
    int index;
    int process;
    double rate;
};


/// A minimal struct for representing a remove task.
struct RemoveTask
{
    int index;
    int process;
};


/*! \brief Class for matching local geometries.
 */
class Matcher {

public:

    /*! \brief Default constructor.
     */
    Matcher();


    /* \brief Build the list of indices and processes to match later.
     *  \param interactions  : The interactions object holding info on possible processes.
     *  \param configuration : The configuration which the list of indices refers to.
     *  \param sitesmap      : The sites map which the list of inidices refers to.
     *  \param lattice_map   : The lattice map describing the configuration.
     *  \param indices       : The configuration indices that will be checked.
     */
    std::vector<std::pair<int, int> > indexProcessToMatch(const Interactions & interactions,
                                                          Configuration & configuration,
                                                          const SitesMap & sitesmap,
                                                          const LatticeMap & lattice_map,
                                                          const std::vector<int> & indices) const;


    /*! \brief Calculate/update the matching of provided indices with
     *         all possible processes.
     *  \param interactions  : The interactions object holding info on possible processes.
     *  \param configuration : The configuration which the list of indices refers to.
     *  \param sitesmap      : The sites map which the list of inidices refers to.
     *  \param lattice_map   : The lattice map describing the configuration.
     *  \param indices       : The configuration indices for which the neighbourhood should
     *                         be matched against all possible processes.
     */
    void calculateMatching(Interactions & interactions,
                           Configuration & configuration,
                           const SitesMap & sitesmap,
                           const LatticeMap & lattice_map,
                           const std::vector<int> & indices) const;


    /*! \brief Calculate the matching for a list of match tasks (pairs of indices
     *         and processes).
     *  \param index_process_to_match : The list of indices and process numbers
     *                                  to match.
     *  \param interactions           : The interactions to get the processes from.
     *  \param configuration          : The configuration which the index refers to.
     *  \param remove_tasks (out)     : A vector that will be filled with tasks
     *                                  for removal after match.
     *  \param update_tasks (out)     : A vector that will be filled with tasks
     *                                  for update after match.
     *  \param add_tasks    (out)     : A vector that will be filled with tasks
     *                                  for adding after match.
     */
    void matchIndicesWithProcesses(const std::vector<std::pair<int,int> > & index_process_to_match,
                                   const Interactions  & interactions,
                                   const Configuration & configuration,
                                   std::vector<RemoveTask> & remove_tasks,
                                   std::vector<RateTask>   & update_tasks,
                                   std::vector<RateTask>   & add_tasks) const;


    /*! \brief Update the rates of the rate tasks by calling the
     *         backend call-back function of the RateCalculator stored
     *         on the interactions object.
     *  \param new_rates(out): The vector to place the updated rates in.
     *  \param tasks         : A vector with tasks to update.
     *  \param interactions  : The interactions to get the rate calculator from.
     *  \param configuration : The configuration to use.
     */
    void updateRates(std::vector<double>         & new_rates,
                     const std::vector<RateTask> & tasks,
                     const Interactions          & interactions,
                     const Configuration         & configuration) const;


    /*! \brief Update the processes with the given tasks.
     *  \param remove_tasks  : A vector with remove tasks for updating the processes.
     *  \param update_tasks  : A vector with update tasks for updating the processes.
     *  \param add_tasks     : A vector with add tasks for updating the processes.
     *  \param interactions  : The interactions to get the processes from.
     */
    void updateProcesses(const std::vector<RemoveTask> & to_remove,
                         const std::vector<RateTask>   & to_update,
                         const std::vector<RateTask>   & to_add,
                         Interactions & interactions) const;

    /*! \brief Calculate the rate for a single process using the rate calculator.
     *  \param index           : The index to perform the process at.
     *  \param process         : The process to perform.
     *  \param configuration   : The configuration the index is referring to.
     *  \param rate_calculator : The rate calculator to use.
     *  \returns : The calculated rate for the process at the given index.
     */
    double updateSingleRate(const int index,
                            const Process        & process,
                            const Configuration  & configuration,
                            const RateCalculator & rate_calculator) const;

    /*! \brief Classify slow/fast species in configuration.
     *  \param interactions       : The interactions object holding info on possible
     *                              processes.
     *  \param configuration(out) : The configuration which the list of indices refers to.
     *  \param lattice_map        : The lattice map describing the configuration.
     */
    void classifyConfiguration(const Interactions & interactions,
                               Configuration      & configuration,
                               const LatticeMap   & lattice_map) const;

protected:

private:

};


#endif // __MATCHER__

