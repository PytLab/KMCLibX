/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* *****************************************************************************
 *  file   : interactions.h
 *  brief  : File for the Interactions class definition.
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

#ifndef __INTERACTIONS__
#define __INTERACTIONS__


#include <vector>

#include "customrateprocess.h"
#include "ratecalculator.h"


// Forward declarations.
class Configuration;
class LatticeMap;
class Process;

/*! \brief Class for holding information about all interactions and possible
 *         processes in the system.
 */
class Interactions {

public:

    /*! \brief Construct the interactions object from a list of processes.
     *  \param processes: The list of proceeses.
     *  \param implicit_wildcards: A flag indicating if implicit wildcards
     *                             should be added to the process matchlists.
     */
    Interactions(const std::vector<Process> & processes,
                 const bool implicit_wildcards);

    /*! \brief Construct the interactions object from a list of processes.
     *  \param processes: The list of proceeses.
     *  \param implicit_wildcards: A flag indicating if implicit wildcards
     *                             should be added to the process matchlists.
     *  \param rate_calculator: The custom rate calculator to use for updating
     *                          the rates.
     */
    Interactions(const std::vector<CustomRateProcess> & processes,
                 const bool implicit_wildcards,
                 const RateCalculator & rate_calculator);

    /*! \brief Get the max range of all processes.
     *  \return : The max range in shells.
     */
    int maxRange() const;

    /*! \brief Query for the custom rates flag.
     *  \return : The custom rates flag, (true) if we use custom rates.
     */
    bool useCustomRates() const { return use_custom_rates_; }

    /*! \brief Update the process matchlists with implicit wildcards if needed.
     *  \param configuration : The configuration needed to determine wildcard positions.
     *  \param lattice_map   : The lattice map to determine wildcard positions.
     */
    void updateProcessMatchLists( const Configuration & configuration,
                                  const LatticeMap & lattice_map);

    /*! \brief Query for the processes.
     *  \return : The processes of the system.
     */
    std::vector<Process*> & processes() { return process_pointers_; }

    /*! \brief Query for the fast processes.
     *  \return : The pointers of fast processes of the system.
     */
    std::vector<Process *> & fastProcesses() { return fast_process_pointers_; }

    /*! \brief Query for the slow processes.
     *  \return : The pointers of slow processes of the system.
     */
    std::vector<Process *> & slowProcesses() { return slow_process_pointers_; }

    /*! \brief Const query for the processes.
     *  \return : A handle to the processes of the system.
     */
    const std::vector<Process*> & processes() const { return process_pointers_; }

    /*! \brief Const query for the fast processes.
     *  \return : The pointers of fast processes of the system.
     */
    const std::vector<Process *> & fastProcesses() const
    { return fast_process_pointers_; }

    /*! \brief Const query for the slow processes.
     *  \return : The pointers of slow processes of the system.
     */
    const std::vector<Process *> & slowProcesses() const
    { return slow_process_pointers_; }

    /*! \brief Const query for the redistribute processes.
     *  \return : The pointers of redistribute processes of the system.
     */
    const std::vector<Process *> & redistProcesses() const
    { return redist_process_pointers_; }

    /*! \brief Const query for the rate calculator reference.
     *  \return : A handle to the rate calculator in use.
     */
    const RateCalculator & rateCalculator() const { return rate_calculator_; }

    /*! \brief Const query for the number of available sites in the whole system.
     *  \return : The number of available sites in the whole system.
     */
    int totalAvailableSites() const;

    /*! \brief Const query for the probability table.
     *  \return : A handle to the present probability table.
     */
    const std::vector<std::pair<double,int> > & probabilityTable() const
    { return probability_table_; }

    /*! \brief Const query for the available site for all processes.
     *  \return : A vector of available sites for all processes.
     */
    const std::vector<int> & processAvailableSites() const
    { return process_available_sites_; }

    /*! \brief Recalculate the table of process probabilities based on the
     *         number of available sites for each process and their rates.
     */
    void updateProbabilityTable();

    /*! \brief Recalculate the available site number for each process.
     */
    void updateProcessAvailableSites();

    /*! \brief Query for the total rate of the system.
     *  \return : The total rate.
     */
    double totalRate() const { return probability_table_.back().first; }

    /*! \brief Pick an availabe process according to its probability.
     *  \return : The index of a possible available process picked according
     *            to its probability.
     *
     *  NOTE: The index is the index in slow processes.
     */
    int pickProcessIndex();

    /*! \brief Pick an availabe process according to its probability and return
     *         a reference to that process.
     *  \return : A reference to a possible available process picked according
     *            to its probability.
     */
    Process* pickProcess();

    /*! \brief Query for the index of process which was picked in last step.
     *  \return : The index number.
     */
    const int & pickedIndex() const { return picked_index_; }

    const std::vector<std::string> redistSpecies() const;

protected:

private:

    /// The processes.
    std::vector<Process> processes_;

    /// The process vector of processes with individual rates.
    std::vector<CustomRateProcess> custom_rate_processes_;

    /// Pointers to the processes we use.
    std::vector<Process*> process_pointers_;

    /// Pointers to the slow processes.
    std::vector<Process *> slow_process_pointers_;

    /// Pointers to the fast processes.
    std::vector<Process *> fast_process_pointers_;

    /// Pointers to the redistribution processes.
    std::vector<Process *> redist_process_pointers_;

    /// The probability table.
    std::vector<std::pair<double,int> > probability_table_;

    /// The available numbers for each process.
    std::vector<int> process_available_sites_;

    /// The flag indicating if implicit wildcards should  be used.
    bool implicit_wildcards_;

    /// The flag indicating if custom rates should be used.
    bool use_custom_rates_;

    /// A rate calculator placeholder if non is given on construction.
    RateCalculator rate_calculator_placeholder_;

    /// A reference to the rate calculator to use.
    const RateCalculator & rate_calculator_;

    /// The index of process picked in the latest step.
    int picked_index_;

};


#endif // __INTERACTIONS__

