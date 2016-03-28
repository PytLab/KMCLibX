/*
  Copyright (c)  2012-2013  Mikael Leetmaa

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/



/*! \file  configuration.h
 *  \brief File for the Configuration class definition.
 */

#ifndef __CONFIGURATION__
#define __CONFIGURATION__


#include <vector>
#include <string>
#include <map>
#include "matchlistentry.h"
#include "coordinate.h"

// Forward declarations.
class LatticeMap;
class Process;

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
    Configuration(std::vector< std::vector<double> > const & coordinates,
                  std::vector<std::string> const & elements,
                  const std::map<std::string,int> & possible_types);

    /*! \brief Initiate the calculation of the match lists.
     *  \param lattice_map : The lattice map needed to get coordinates wrapped.
     *  \param range       : The number of shells to include.
     */
    void initMatchLists(const LatticeMap & lattice_map, const int range);

    /*! \brief Const query for the coordinates.
     *  \return : The coordinates of the configuration.
     */
    const std::vector<Coordinate> & coordinates() const { return coordinates_; }

    /*! \brief Const query for the atom id coordinates.
     *  \return : The atom id coordinates of the configuration.
     */
    const std::vector<Coordinate> & atomIDCoordinates() const { return atom_id_coordinates_; }

    /*! \brief Const query for the elements.
     *  \return : The elements of the configuration.
     */
    const std::vector<std::string> & elements() const { return elements_; }

    /*! \brief Const query for the atom id types.
     *  \return : The atom id types of the configuration.
     */
    const std::vector<std::string> & atomIDElements() const { return atom_id_elements_; }

    /*! \brief Const query for the types.
     *  \return : The types of the configuration.
     */
    const std::vector<int> & types() const { return types_; }

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
    const std::vector<MinimalMatchListEntry> & minimalMatchList(const int origin_index,
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
    const std::vector<MinimalMatchListEntry> & minimalMatchList(const int index) const { return match_lists_[index]; }

    /*! \brief Perform the given process.
     *  \param process : The process to perform, which will be updated with the affected
     *                   indices.
     *  \param site_index : The index of the site where the process should be performed.
     *  \param lattice_map : The lattice map needed for proper move vector indexing.
     */
    void performProcess(Process & process,
                        const int site_index,
                        const LatticeMap & lattice_map);

    /*! \brief Query for the type name.
     *  \param type: The type integer to get the name for.
     *  \return : The string representation of the type integer.
     */
    const std::string & typeName(const int type) const { return type_names_[type]; }

    /*! \brief Get the atom id coordinates.
     *  \return : The list of atom id coordinates.
     */
    const std::vector<Coordinate> & atomIdCoordinates() const { return atom_id_coordinates_; }

    /*! \brief Get the atom id at each lattice site.
     *  \retrurn : The list of atom ids for the lattice sites.
     */
    const std::vector<int> & atomID() const { return atom_id_; }

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
    std::vector< std::vector<MinimalMatchListEntry> > match_lists_;

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

