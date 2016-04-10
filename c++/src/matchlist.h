/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matchlist.h
 *  brief  : File for the MatchList definition and utility functions.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-10   1.2          Initial creation.
 *
 *  ------------------------------------------------------
 * ******************************************************************
 */

#ifndef __MATCHLIST__
#define __MATCHLIST__

#include <vector>

#include "matchlistentry.h"


// Forward declarations, if any.
class Configuration;


/// Define the match lists.
typedef std::vector<ProcessMatchListEntry> ProcessMatchList;
typedef std::vector<ConfigMatchListEntry> ConfigMatchList;


/*! \brief Set up a process matchlist from two local configurations.
 *  \param first            : (in) The first (before) configuration.
 *  \param second           : (in) The second (after) configuration.
 *  \param range            : (in/out) The rang of the process.
 *  \param cutoff           : (in/out) The cutoff of the process.
 *  \param match_list       : (in/out) The matchlist to construct.
 *  \param affected_indices : (in/out) The affected indices to set the size of.
 *  \param move_origins     : (in/out) The local indices from which move vectors originate.
 *  \param move_vector      : (in/out) The vector for each moved atom.
 */
void configurationsToMatchList(const Configuration & first,
                               const Configuration & second,
                               int & range,
                               double & cutoff,
                               ProcessMatchList & match_list,
                               std::vector<int> & affected_indices,
                               const std::vector<int> & move_origins = {},
                               const std::vector<Coordinate> & move_vectors = {});

#endif  // __MATCHLIST__

