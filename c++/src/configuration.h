/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/



/* ******************************************************************
 *  file   : configuration.h
 *  brief  : File for the definition of the Configuration class and
 *           SubConfiguration class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-11   1.2          Modify match list presentation.
 *  zjshao     2016-10-15   1.4          Add fast slow classification.
 *  zjshao     2016-10-22   1.4          Add SubConfiguration class.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#ifndef __CONFIGURATION__
#define __CONFIGURATION__


#include <vector>
#include <string>
#include <map>

#include "matchlist.h"


// Forward declarations.
class LatticeMap;
class Process;
class Coordinate;
class SubLatticeMap;
class SubConfiguration;

/*! \brief Class for defining the configuration used in a KMC simulation to
 *         use for communicating elements and positions to and from python.
 */
class Configuration {

public:

    /*! \brief Constructor for setting up the configuration.
     *  \param coordinates   : The coordinates of the configuration.
     *  \param elements      : The elements of the configuration.
     *  \param possible_types: A global mapping from type string to number.
     */
    Configuration(const std::vector< std::vector<double> > & coordinates,
                  const std::vector<std::string> & elements,
                  const std::map<std::string,int> & possible_types);

    /*! \brief Descructor for configruation.
     */
    virtual ~Configuration() {}

    /*! \brief Initiate the calculation of the match lists.
     *  \param lattice_map : The lattice map needed to get coordinates wrapped.
     *  \param range       : The number of shells to include.
     */
    void initMatchLists(const LatticeMap & lattice_map, const int range);

    /*! \brief Const query for the coordinates.
     *  \return : The coordinates of the configuration.
     */
    const std::vector<Coordinate> & coordinates() const
    { return coordinates_; }

    /*! \brief Const query for the atom id coordinates.
     *  \return : The atom id coordinates of the configuration.
     */
    const std::vector<Coordinate> & atomIDCoordinates() const
    { return atom_id_coordinates_; }

    /*! \brief Const query for the elements.
     *  \return : The elements of the configuration.
     */
    const std::vector<std::string> & elements() const
    { return elements_; }

    /*! \brief Const query for the atom id types.
     *  \return : The atom id types of the configuration.
     */
    const std::vector<std::string> & atomIDElements() const
    { return atom_id_elements_; }

    /*! \brief Const query for the types.
     *  \return : The types of the configuration.
     */
    const std::vector<int> & types() const
    { return types_; }


    /*! \brief Const query for the possible types.
     *  \return : The possible types of the configuration.
     */
    const std::map<std::string, int> & possibleTypes() const
    { return possible_types_; }

    /*! \brief Const query for the moved atom ids.
     *  \return : A copy of the moved atom ids, resized to correct length.
     */
    inline
    std::vector<int> movedAtomIDs() const;

    /*! \brief Const query for the moved atoms move vectors, in the same order as the id's.
     *  \return : A copy of the recent move vectors, resized to correct length.
     */
    inline
    std::vector<Coordinate> recentMoveVectors() const;

    /*! \brief Construct and return the match list for the given list of
     *         indices.
     *  \param origin_index : The index to treat as the origin.
     *  \param indices      : The indices to get the match list for.
     *  \param lattice_map  : The lattice map needed for calculating distances
     *                        using correct boundaries.
     *  \return : The match list.
     */
    const ConfigMatchList & matchList(const int origin_index,
                                      const std::vector<int> & indices,
                                      const LatticeMap & lattice_map) const;

    /*! \brief Update the cached match list for the given index.
     *  \param index : The index to update the match list for.
     */
    void updateMatchList(const int index);

    /*! \brief Return the cached match list without update.
     *  \param index : The index to get the match list for.
     *  \return : The match list.
     */
    const ConfigMatchList & matchList(const int index) const
    { return match_lists_[index]; }

    /*! \brief Perform the given process.
     *  \param process : The process to perform, which will be updated with
     *                   the affected indices.
     *  \param site_index : The index of the site where the process should be
     *                      performed.
     *  \param lattice_map : The lattice map needed for proper move vector
     *                       indexing.
     */
    void performProcess(Process & process, const int site_index);

    /*! \brief Extract a sub-configuration from a global configuration.
     *  \param lattice_map : The global lattie map.
     *  \param sub_lattice_map : The corresponding sub-lattice map of the
     *                           extracted sub-configuration.
     *  \return : Sub-configuration object.
     */
    SubConfiguration subConfiguration(const LatticeMap & lattice_map,
                                      const SubLatticeMap & sub_lattice_map) const;

    /*! \brief Query for the type name.
     *  \param type: The type integer to get the name for.
     *  \return : The string representation of the type integer.
     */
    const std::string & typeName(const int type) const
    { return type_names_[type]; }

    /*! \brief Get the atom id coordinates.
     *  \return : The list of atom id coordinates.
     */
    const std::vector<Coordinate> & atomIdCoordinates() const
    { return atom_id_coordinates_; }

    /*! \brief Get the atom id at each lattice site.
     *  \retrurn : The list of atom ids for the lattice sites.
     */
    const std::vector<int> & atomID() const
    { return atom_id_; }

    /*! \brief Query for the species slow flags.
     *  \return : The list of slow species flags.
     *
     *  NOTE: The species flags in configuration are called **SLOW** flag
     *        which are different from flag in Process class, but the
     *        default flags of configuration are **TRUE** meaning slow
     *        species by default.
     *
     *        In a word, the process and species in configuration are all slow
     *        by default, once the species is found participating in fast process
     *        it is converted to fast species.
     */
    const std::vector<bool> & slowFlags() const
    { return slow_flags_; }

    /*! \brief Reset all species slow flags to be true.
     *  \param : The default fast element names.
     *  \return : The list of reseted species slow flags.
     */
    void resetSlowFlags(const std::vector<std::string> & fast_elements = {});

    /*! \brief Update specific slow flag in configuration.
     *  \param type: The index of flag in global struture.
     *  \param type: The flag value.
     */
    void updateSlowFlag(const int index, const bool value)
    { slow_flags_[index] = value; }

protected:

private:

    /// Counter for the number of moved atom ids the last move.
    int n_moved_;

    /// The lattice coordinates.
    std::vector<Coordinate> coordinates_;

    /// The coordinates for each atom id.
    std::vector<Coordinate> atom_id_coordinates_;

    /// The lattice elements.
    std::vector<std::string> elements_;

    /// The possible types.
    std::map<std::string, int> possible_types_;

    /// The elements per atom id.
    std::vector<std::string> atom_id_elements_;

    /// The the lattice elements in integer representation.
    std::vector<int> types_;

    /// The atom id for each lattice point.
    std::vector<int> atom_id_;

    /// The first n_moved_ elements hold the moved atom ids.
    std::vector<int> moved_atom_ids_;

    /// The first n_moved_ elements hold the moved atoms move vectors (listed on id).
    std::vector<Coordinate> recent_move_vectors_;

    /// The mapping from type integers to names.
    std::vector<std::string> type_names_;

    /// The match lists for all indices.
    std::vector<ConfigMatchList> match_lists_;

    /// The species fast/slow flags, true if slow.
    std::vector<bool> slow_flags_;

};


class SubConfiguration : public Configuration {

public:

    /*! \brief Constructor for setting up the sub-confguration.
     *  \param coordinates   : The coordinates of the sub-configuration.
     *  \param elements      : The elements of the sub-configuration.
     *  \param possible_types: A global mapping from type string to number.
     *  \param atom_ids      : The global atom ids for the sites in
     *                         sub-configurations
     */
    SubConfiguration(const std::vector<std::vector<double> > & coordinates,
                     const std::vector<std::string> & elements,
                     const std::map<std::string, int> & possible_types,
                     const std::vector<int> & atom_id);

    /*! brief Destructor for sub-configuration.
     */
    ~SubConfiguration() {}

    /*! \brief Query for the global atom ids.
     *  \return: The list of global atom ids.
     */
    const std::vector<int> & atomID() const { return atom_id_; }

private:

    /// The gloabl atom ids for the sites in sub-configuration.
    std::vector<int> atom_id_;

};


// -----------------------------------------------------------------------------
// Inlined function definitions follow.
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
std::vector<int> Configuration::movedAtomIDs() const
{
    // PERFORMME
    std::vector<int> moved(moved_atom_ids_);
    moved.resize(n_moved_);
    return moved;
}


// -----------------------------------------------------------------------------
//
std::vector<Coordinate> Configuration::recentMoveVectors() const
{
    // PERFORMME
    std::vector<Coordinate> move_vectors(recent_move_vectors_);
    move_vectors.resize(n_moved_);
    return move_vectors;
}


#endif // __CONFIGURATION__

