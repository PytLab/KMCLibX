/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : process.cpp
 *  brief  : File for the implementation code of the Process class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-04-10   1.2          Modify match list presentation.
 *  zjshao     2016-04-10   1.3          Add site type.
 *  zjshao     2016-10-14   1.4          Add slow process flag.
 *  zjshao     2016-11-21   1.4          Add redistribution process flag.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#include <algorithm>
#include <cstdio>
#include <stdexcept>

#include "process.h"
#include "random.h"
#include "configuration.h"

// -----------------------------------------------------------------------------
//
Process::Process(const Configuration & first,
                 const Configuration & second,
                 const double rate,
                 const std::vector<int> & basis_sites,
                 const std::vector<int> & move_origins,
                 const std::vector<Coordinate> & move_vectors,
                 const int process_number,
                 const std::vector<int> & site_types,
                 const bool fast,
                 const bool redistribution,
                 const std::string & redist_species) :
    process_number_(process_number),
    range_(1),
    rate_(rate),
    cutoff_(0.0),
    sites_(0),
    affected_indices_(0),
    basis_sites_(basis_sites),
    id_moves_(0),
    fast_(fast),
    redistribution_(redistribution),
    redist_species_(redist_species)
{
    // {{{

    // Transform the configurations into match list.
    configurationsToMatchList(first,
                              second,
                              range_,
                              cutoff_,
                              match_list_,
                              affected_indices_,
                              move_origins,
                              move_vectors);

    // Find out which index in the match list each move vector
    // points to.
    for (size_t i = 0; i < match_list_.size(); ++i)
    {
        if (match_list_[i].has_move_coordinate)
        {
            // If this move vector is different from zero we go on and try to find
            // which index in the sorted match list it points to.

            // Get the move vector out.
            const Coordinate & move_vector = match_list_[i].move_coordinate;

            // Setup the destination coordinate.
            const Coordinate destination = match_list_[i].coordinate + move_vector;

            for (size_t j = 0; j < match_list_.size(); ++j)
            {
                // We can only move to a coordinate which also has a
                // move coordinate.
                if (match_list_[j].has_move_coordinate && (j != i) )
                {
                    // If the difference is small enough we have a match.
                    const Coordinate diff = match_list_[j].coordinate - destination;

                    if (diff.norm() < 1.0e-6)
                    {
                        id_moves_.push_back(std::pair<int,int>(i,j));
                        break;
                    }
                }
            }
        }
    }

    // Add site type to matchlist.
    ProcessMatchList::iterator proc_it;
    const ProcessMatchList::const_iterator end_it = match_list_.end();

    // Fill match list with site types passed in.
    // If no site types passed in, default value 0 will be used.
    if (site_types.size() == 0)
    {
        has_site_types_ = false;        
    }
    else if (site_types.size() != 0)
    {
        // Set site type for each process match list entry.
        std::vector<int>::const_iterator site_it = site_types.begin();
        for (proc_it = match_list_.begin(); proc_it != end_it; ++proc_it, ++site_it)
        {
            proc_it->site_type = *site_it;
        }

        // Set site type setting flag.
        has_site_types_ = true;
    }

    // Check the redist_species.
    if ( redistribution_ && redist_species_.empty() )
    {
        std::string msg = "The redist_species is an empty string.";
        throw std::invalid_argument(msg);
    }

    // }}}
}

// -----------------------------------------------------------------------------
//
Process::Process(const Configuration & first,
                 const Configuration & second,
                 const double rate,
                 const std::vector<int> & basis_sites,
                 const bool fast,
                 const bool redistribution,
                 const std::string & redist_species) :
    Process(first, second, rate, basis_sites, {}, {}, -1, {},
            fast, redistribution, redist_species)
{
    // NOTHING HERE.
}

// -----------------------------------------------------------------------------
//
void Process::addSite(const int index, const double rate)
{
    sites_.push_back(index);
}

// -----------------------------------------------------------------------------
//
void Process::removeSite(const int index)
{
    // Find the index to remove.
    std::vector<int>::iterator it1 = std::find(sites_.begin(),
                                               sites_.end(),
                                               index);
    // Swap the index to remove with the last index.
    std::vector<int>::iterator last = sites_.end()-1;
    std::swap((*it1), (*last));
    // Remove the last index from the list.
    sites_.pop_back();
}

// -----------------------------------------------------------------------------
//
int Process::pickSite() const
{
    // Draw an integer between 0 and sites_.size() - 1
    const int rnd = static_cast<int>(randomDouble01() * sites_.size());
    return sites_[rnd];
}

// -----------------------------------------------------------------------------
//
bool Process::isListed(const int index) const
{
    // Search in the list to find out if it is added.
    return std::find(sites_.begin(), sites_.end(), index) != sites_.end();
}

