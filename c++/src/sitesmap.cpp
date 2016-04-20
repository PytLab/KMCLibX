/*
  Copyright (c)  2016-2019 Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*  ***************************************************************
 *  file   : sitesmap.cpp
 *  brief  : File for the implementation code of the SitesMap class.
 *  author : zjshao <shaozhengjiang@gmail.com>
 *  date   : 2016-04-08
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-08   2.0          Initial creation.
 *
 *  ------------------------------------------------------
 *  ****************************************************************/

#include "sitesmap.h"

#include <algorithm>

#include "matchlist.h"
#include "latticemap.h"

// Temporary data for the match list return.
static SiteMatchList tmp_site_match_list__(0);

// -------------------------------------------------------------------------------
//
SitesMap::SitesMap(const std::vector< std::vector<double> > & coordinates,
                   const std::vector<std::string> & sites,
                   const std::map<std::string, int> & possible_types) :
    sites_(sites),
    possible_types_(possible_types),
    match_lists_(sites.size())
{
    // Initialize all site coordinates.
    for (const std::vector<double> & c : coordinates)
    {
        coordinates_.push_back(Coordinate(c.at(0), c.at(1), c.at(2)));        
    }

    // Setup the types from site strings.
    for (const std::string & site : sites)
    {
        const int type = possible_types.find(site)->second;
        types_.push_back(type);
    }
}


// -------------------------------------------------------------------------------
//
void SitesMap::initMatchLists(const LatticeMap & lattice_map,
                              const int range)
{
    // {{{
    
    // Loop over all lattice sites to initialize the match lists.
    for (size_t i = 0; i < types_.size(); ++i)
    {
        // Calculate and store the match list.
        const int origin_index = i;
        const std::vector<int> neighbourhood = lattice_map.neighbourIndices(origin_index,
                                                                            range);
        match_lists_[i] = matchList(origin_index, neighbourhood, lattice_map);
    }

    // }}}
}


// -------------------------------------------------------------------------------
//
const SiteMatchList & SitesMap::matchList(const int origin_index,
                                          const std::vector<int> & indices,
                                          const LatticeMap & lattice_map) const
{
    // {{{

    // Setup the return data.
    tmp_site_match_list__.resize(indices.size());

    // Extract the coordinate of the first index.
    const Coordinate center = coordinates_[origin_index];

    // Setup the needed iterators.
    std::vector<int>::const_iterator it_index  = indices.begin();
    const std::vector<int>::const_iterator end = indices.end();
    SiteMatchList::iterator it_match_list = tmp_site_match_list__.begin();

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
    std::sort(tmp_site_match_list__.begin(), tmp_site_match_list__.end());

    return tmp_site_match_list__;

    // }}}
}

