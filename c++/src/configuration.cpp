/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : configuration.cpp
 *  brief  : File for the implementation code of the Configuration class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-11   1.2          Modify match list presentation.
 *  zjshao     2016-10-15   1.4          Add fast/slow flags.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#include <cstdio>
#include <algorithm>

#include "configuration.h"
#include "latticemap.h"
#include "process.h"
#include "coordinate.h"

// Temporary data for the match list return.
static ConfigMatchList tmp_config_match_list__(0);

// -----------------------------------------------------------------------------
//
Configuration::Configuration(std::vector<std::vector<double> > const & coordinates,
                             std::vector<std::string> const & elements,
                             const std::map<std::string,int> & possible_types) :
    n_moved_(0),
    elements_(elements),
    atom_id_elements_(elements),
    match_lists_(elements_.size()),
    fast_flags_(elements_.size(), false)
{
    // {{{

    // Setup the coordinates and initial atom ids.
    for (size_t i = 0; i < coordinates.size(); ++i)
    {
        coordinates_.push_back( Coordinate(coordinates[i][0],
                                           coordinates[i][1],
                                           coordinates[i][2]));
        atom_id_.push_back(i);
    }

    // Set the atom id coordinates to the same as the coordinates to start with.
    atom_id_coordinates_ = coordinates_;

    // Loop through the possible types map and find out what the maximum is.
    std::map<std::string,int>::const_iterator it1 = possible_types.begin();
    int max_type = 0;
    for ( ; it1 != possible_types.end(); ++it1)
    {
        if (it1->second > max_type)
        {
            max_type = it1->second;
        }
    }

    // Set the size of the type names list.
    type_names_ = std::vector<std::string>(max_type+1);

    // Fill the list.
    it1 = possible_types.begin();
    for ( ; it1 != possible_types.end(); ++it1 )
     {
         type_names_[it1->second] = it1->first;
     }

    // Setup the types from the elements strings.
    for (const std::string & element : elements)
    {
        std::map<std::string, int>::const_iterator it = possible_types.find(element);
        types_.push_back(it->second);
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
void Configuration::initMatchLists( const LatticeMap & lattice_map,
                                    const int range )
{
    // {{{

    // Store the max size of minimal_match_list_
    size_t max_size = 0;
    size_t tmp_size;

    // Loop over all lattice sites.
    for (size_t i = 0; i < types_.size(); ++i)
    {
        // Calculate and store the match list.
        const int origin_index = i;
        const std::vector<int> neighbourhood = lattice_map.neighbourIndices(origin_index, range);
        match_lists_[i] = matchList(origin_index, neighbourhood, lattice_map);

        // Store the maximum size
        tmp_size = match_lists_[i].size();
        if ( tmp_size > max_size )
        {
            max_size = tmp_size;
        }
    }

    // Now that we know the size of the match lists we can allocate
    // memory for the moved_atom_ids_ vector.
    moved_atom_ids_.resize(max_size);
    recent_move_vectors_.resize(max_size);

    // }}}
}


// -----------------------------------------------------------------------------
//
void Configuration::updateMatchList(const int index)
{
    // {{{

    // Update the match list's types information.
    ConfigMatchList::iterator it1   = match_lists_[index].begin();
    const ConfigMatchList::const_iterator end = match_lists_[index].end();
    for ( ; it1 != end; ++it1 )
    {
        (*it1).match_type = types_[(*it1).index];
    }

    // }}}
}


// -----------------------------------------------------------------------------
//
const ConfigMatchList & Configuration::matchList(const int origin_index,
                                                 const std::vector<int> & indices,
                                                 const LatticeMap & lattice_map) const
{
    // {{{

    // Setup the return data.
    tmp_config_match_list__.resize(indices.size());

    // Extract the coordinate of the first index.
    const Coordinate center = coordinates_[origin_index];

    // Setup the needed iterators.
    std::vector<int>::const_iterator it_index  = indices.begin();
    const std::vector<int>::const_iterator end = indices.end();
    ConfigMatchList::iterator it_match_list = tmp_config_match_list__.begin();

    const bool periodic_a = lattice_map.periodicA();
    const bool periodic_b = lattice_map.periodicB();
    const bool periodic_c = lattice_map.periodicC();

    // Since we know the periodicity outside the loop we can make the
    // logics outside also.

    // Periodic a-b-c
    if (periodic_a && periodic_b && periodic_c)
    {
        // Loop, calculate and add to the return list.
        for ( ; it_index != end; ++it_index, ++it_match_list)
        {
            // All coordinates in match list are relative to origin.
            Coordinate c = coordinates_[(*it_index)] - center;

            // Wrap with coorect periodicity.
            lattice_map.wrap(c, 0);
            lattice_map.wrap(c, 1);
            lattice_map.wrap(c, 2);

            // Get the distance.
            const double distance = c.distanceToOrigin();

            // Get the type.
            const int match_type = types_[(*it_index)];

            // Save in the match list.
            (*it_match_list).match_type  = match_type;
            (*it_match_list).distance    = distance;
            (*it_match_list).coordinate  = c;
            (*it_match_list).index       = (*it_index);
        }
    }
    // Periodic a-b
    else if (periodic_a && periodic_b)
    {
        // Loop, calculate and add to the return list.
        for ( ; it_index != end; ++it_index, ++it_match_list)
        {
            // All coordinates in match list are relative to origin.
            Coordinate c = coordinates_[(*it_index)] - center;

            // Wrap with correct periodicity.
            lattice_map.wrap(c, 0);
            lattice_map.wrap(c, 1);

            // Get the distance.
            const double distance = c.distanceToOrigin();

            // Get the type.
            const int match_type = types_[(*it_index)];

            // Save in the match list.
            (*it_match_list).match_type  = match_type;
            (*it_match_list).distance    = distance;
            (*it_match_list).coordinate  = c;
            (*it_match_list).index       = (*it_index);
        }
    }
    else {
        // The general case fore wrapping all directions.
        // Periodic b-c
        // Periodic a-c
        // Periodic a
        // Periodic b
        // Periodic c

        // Loop, calculate and add to the return list.
        for ( ; it_index != end; ++it_index, ++it_match_list)
        {
            // All coordinates in match list are relative to origin.
            Coordinate c = coordinates_[(*it_index)] - center;

            // Wrap with correct periodicity.
            lattice_map.wrap(c);

            const double distance = c.distanceToOrigin();

            // Get the type.
            const int match_type = types_[(*it_index)];

            // Save in the match list.
            (*it_match_list).match_type  = match_type;
            (*it_match_list).distance    = distance;
            (*it_match_list).coordinate  = c;
            (*it_match_list).index       = (*it_index);
        }
    }

    // Sort and return.
    std::sort(tmp_config_match_list__.begin(), tmp_config_match_list__.end());

    return tmp_config_match_list__;

    // }}}
}


// -----------------------------------------------------------------------------
//
void Configuration::performProcess(Process & process, const int site_index)
{
    // {{{

    // PERFORMME
    // Need to time and optimize the new parts of the routine.

    // Get the proper match lists.
    const ProcessMatchList & process_match_list = process.matchList();
    const ConfigMatchList & config_match_list = matchList(site_index);

    // Iterators to the match list entries.
    ProcessMatchList::const_iterator proc_it = process_match_list.begin();
    ConfigMatchList::const_iterator conf_it = config_match_list.begin();

    // Iterators to the info storages.
    std::vector<int>::iterator affected_it = process.affectedIndices().begin();
    std::vector<int>::iterator atom_it = moved_atom_ids_.begin();
    std::vector<Coordinate>::iterator vect_it = recent_move_vectors_.begin();

    // Reset the moved counter.
    n_moved_ = 0;

    // Loop over the match lists and get the types and indices out.
    for( ; proc_it != process_match_list.end(); ++proc_it, ++conf_it)
    {
        // Get the type out of the process match list.
        const int update_type = (*proc_it).update_type;

        // Get the index out of the configuration match list.
        const int index = (*conf_it).index;

        // NOTE: The > 0 is needed for handling the wildcard match.
        if (types_[index] != update_type && update_type > 0)
        {
            // Get the atom id to apply the move vector to.
            const int atom_id = atom_id_[index];

            // Apply the move vector to the atom coordinate.
            atom_id_coordinates_[atom_id] += (*proc_it).move_coordinate;

            // Set the type at this index.
            types_[index]    = update_type;
            elements_[index] = type_names_[update_type];

            // Update the atom id element.
            if (!(*proc_it).has_move_coordinate)
            {
                atom_id_elements_[atom_id] = elements_[index];
            }

            // Mark this index as affected.
            (*affected_it) = index;
            ++affected_it;

            // Mark this atom_id as moved
            // (include the replaced atom_id).
            (*atom_it) = atom_id;
            ++atom_it;
            ++n_moved_;

            // Save this move vector
            // (include the replace move Coordinate(0.0, 0.0, 0.0)).
            (*vect_it) = (*proc_it).move_coordinate;
            ++vect_it;
        }
    }

    // Perform the moves on all involved atom-ids.
    const std::vector< std::pair<int, int> > & process_id_moves = process.idMoves();

    // Local vector to store the atom id updates in.
    std::vector<std::pair<int,int> > id_updates(process_id_moves.size());

    // Setup the id updates list.
    for (size_t i = 0; i < process_id_moves.size(); ++i)
    {
        const int match_list_index_from = process_id_moves[i].first;
        const int match_list_index_to   = process_id_moves[i].second;

        const int lattice_index_from = config_match_list[match_list_index_from].index;
        const int lattice_index_to   = config_match_list[match_list_index_to].index;

        id_updates[i].first  = atom_id_[lattice_index_from];
        id_updates[i].second = lattice_index_to;
    }

    // Apply the id updates on the configuration.
    for (size_t i = 0; i < id_updates.size(); ++i)
    {
        const int id    = id_updates[i].first;
        const int index = id_updates[i].second;

        // Set the atom id at this lattice site index.
        atom_id_[index] = id;
        atom_id_elements_[id] = elements_[index];

    }

    // }}}
}

