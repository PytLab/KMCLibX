/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : process.h
 *  brief  : File for the Process class definition.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-10   1.2          Modify match list presentation.
 *  zjshao     2016-04-10   1.3          Add site type.
 *  zjshao     2016-10-14   1.4          Add fast process flag.
 *  zjshao     2016-11-21   1.4          Add redistribution process flag.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#ifndef __PROCESS__
#define __PROCESS__

#include <vector>
#include <map>
#include <string>

#include "matchlist.h"

// Forward declarations.
class Configuration;

/*! \brief Class for defining a possible process int the system.
 */
class Process {

public:

    /*! \brief Default constructor needed for use in std::vector SWIG wrapping.
     */
    Process() {}

    /*! \brief Constructor for the process. Note that the configurations given
     *         to the process are local configurations and no periodic boundaries
     *         will be taken into consideration.
     *  \param first         : The first configuration, to match against the local
     *                         coordinates around an active site.
     *  \param second        : The second configuration, to update the local
     *                         configuration with if the process is selected.
     *                         It is assumed that the first and second configuration
     *                         has identical coordinates.
     *  \param rate          : The rate in Hz associated with the process.
     *  \param basis_sites   : The basis sites where this process is applicable.
     *  \param move_origins  : The vector of indices in the local configurations
     *                         that the move vectors originate from. This vector
     *                         can be empty if no move vectors are used.
     *  \param move_vectors  : The vector of coordinates for each moved atom.
     *                         This vector can be empty if no move vectors are used,
     *                         i.e., if only elements are moved on the lattice and no
     *                         atom id moves are considered.
     *  \param process_number: The id number of the process.
     *  \param site_types    : The site type on which the process is performed.
     *  \param fast          : The flag for fast process or not.
     *
     *  NOTE: The name of flag in process is **FAST** which is different from
     *        that in configuration, but the default value is false meaning
     *        that the default process is slow process.
     *
     *        In a word, the process and species in configuration are all slow
     *        by default, once the species is found participating in fast process
     *        it is converted to fast species.
     *
     *  \param redistribution: The flag for redistribution process.
     *  \param redist_species: The corresponding species to be scattered in
     *                         redistribution.
     *
     */
    Process(const Configuration & first,
            const Configuration & second,
            const double rate,
            const std::vector<int> & basis_sites,
            const std::vector<int> & move_origins = {},
            const std::vector<Coordinate> & move_vectors = {},
            const int process_number = -1,
            const std::vector<int> & site_types = {},
            const bool fast = false,
            const bool redistribution = false,
            const std::string & redist_species = "");

    /*! \brief Constructor for the process with fast flag provided explicitly.
     *  \param first         : The first configuration, to match against the local
     *                         coordinates around an active site.
     *  \param second        : The second configuration, to update the local
     *                         configuration with if the process is selected.
     *                         It is assumed that the first and second configuration
     *                         has identical coordinates.
     *  \param rate          : The rate in Hz associated with the process.
     *  \param basis_sites   : The basis sites where this process is applicable.
     *  \param fast          : The flag for fast process or not.
     *  \param redistribution: The flag for redistribution process.
     *  \param redist_species: The corresponding species to be scattered in
     *                         redistribution.
     */
    Process(const Configuration & first,
            const Configuration & second,
            const double rate,
            const std::vector<int> & basis_sites,
            const bool fast,
            const bool redistribution = false,
            const std::string & redist_species = "");

    /*! \brief Virtual destructor needed for use as base class.
     */
    virtual ~Process() {}

    /*! \brief Query for the total rate.
     *  \return : The total rate of the process.
     */
    virtual double totalRate() const { return rate_ * sites_.size(); }

    /*! \brief Add the index to the list of available sites.
     *  \param index : The index to add.
     *  \param rate  : Dummy argument needed for common interface.
     */
    virtual void addSite(const int index, const double rate=0.0);

    /*! \brief Remove the index from the list of available sites.
     *  \param index : The index to remove.
     */
    virtual void removeSite(const int index);

    /*! \brief Pick a random available process.
     *  \return : A random available process.
     */
    virtual int pickSite() const;

    /*! \brief Interface function for inherited classes.
     *         This function does nothing if not overloaded.
     */
    virtual void updateRateTable() {}

    /*! \brief Query for the rate constant associated with the process.
     *  \return : The rate constant part of the of rate for the process.
     */
    double rateConstant() const { return rate_; }

    /*! \brief Query for the number of listed possible sites for this process.
     *  \return : The number of listed indices.
     */
    size_t nSites() const {return sites_.size(); }

    /*! \brief Determine if an index is listed as available site for this process.
     *  \param index : The index to check.
     *  \return : True if match.
     */
    bool isListed(const int index) const;

    /*! \brief Query for the available sites for this process.
     *         Convenient when testing other functionality of the class.
     *  \return : The available sites.
     */
    const std::vector<int> & sites() const { return sites_; }

    /*! \brief Query for the configuration as a vector of match list entries.
     *  \return : The stored match list.
     */
    const ProcessMatchList & matchList() const { return match_list_; }

    /*! \brief Query for the configuration as a vector of match list entries.
     *  \return : A reference to the stored match list.
     */
    ProcessMatchList & matchList() { return match_list_; }

    /*! \brief Query for the latest affected indices.
     *  \return : The affected indices from the last time the process was
     *            performed on a calculation.
     */
    const std::vector<int> & affectedIndices() const { return affected_indices_; }

    /*! \brief Query for the latest affected indices.
     *  \return : The affected indices from the last time the process was
     *            performed on a calculation.
     */
    std::vector<int> & affectedIndices() { return affected_indices_; }

    /*! \brief Query for the basis sites.
     *  \return : The basis sites at which this process is applicable.
     */
    const std::vector<int> & basisSites() const { return basis_sites_; }

    /*! \brief Query for the atom id moves.
     *  \return : The pairs of match list indices corresponding to atom id moves.
     */
    const std::vector< std::pair<int,int> > & idMoves() const { return id_moves_; }

    /*! \brief Non-const query for the atom id moves.
     *  \return : The pairs of match list indices corresponding to atom id moves.
     */
    std::vector< std::pair<int,int> > & idMoves() { return id_moves_; }

    /*! \brief Query for the cutoff distance.
     *  \return : The cutoff radius for the process.
     */
    double cutoff() const { return cutoff_; }

    /*! \brief Query for the range.
     *  \return : The range for the process in number of cells.
     */
    int range() const { return range_; }

    /*! \brief Query for the process number.
     *  \return : The number of the process.
     */
    int processNumber() const { return process_number_; }

    /*! \brief Query for the site type setting flag.
     *  \return : Whether site type is set.
     */
    bool hasSiteTypes() const { return has_site_types_; }

    /*! \brief Query for the fast process flag.
     *  \return : Whether the process is fast process.
     */
    bool fast() const { return fast_; }

    /*! \brief Query for the redistribution process flag.
     *  \return : Whether the process is used for redistribution.
     */
    bool redistribution() const { return redistribution_; }

    /*! \brief Query for the corresponding species for redistribution.
     *  \return : The corresponding species for redistribution.
     */
    const std::string & redistSpecies() const { return redist_species_; }

protected:

    /// The process number.
    int process_number_;

    /// The range in primitive cells.
    int range_;

    /// The rate in Hz.
    double rate_;

    /// The cutoff radius primitive unit-cell fractional units.
    double cutoff_;

    /// The available sites for this process.
    std::vector<int> sites_;

    /// The match list for comparing against local configurations.
    ProcessMatchList match_list_;

    /*! \brief: The configuration indices that were affected last time
     *          the process was used to update a configuration.
     */
    std::vector<int> affected_indices_;

    /// The basis sites to which this process can be applied.
    std::vector<int> basis_sites_;

    /// The atom id moves.
    std::vector< std::pair<int,int> > id_moves_;

    /// The flag whether site type is set.
    bool has_site_types_;

    /// The flag whether process is fast process.
    bool fast_;

    /// The flag for whether the process is used for redistribution.
    bool redistribution_;

    /// The corresponding species name to be re-scattered.
    std::string redist_species_;

private:

};


#endif // __PROCESS__

