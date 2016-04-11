/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matchlistentry.h
 *  brief  : File for the MatchListEntry class definition.
 *  author : zjshao
 *  date   : 2016-04-09
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-09   1.2          Initial creation.
 *
 *  ------------------------------------------------------
 * ******************************************************************
 */

#ifndef __MATCHLISTENTRY__
#define __MATCHLISTENTRY__

#include <cmath>
#include <stdexcept>

#include "coordinate.h"
#include "exceptions.h"


/// The base class for the match list entries.
class MinimalMatchListEntry {

public:
    /// The distance.
    double distance;

    /// The Coordinate.
    Coordinate coordinate;

    /// The match type integer.
    int match_type;

    /* Class functions. */
    
    /*! \brief 'less than' operator overloading for sorting matchlists.
     */
    virtual bool operator<(const MinimalMatchListEntry & other) const;

    /*! \brief 'equal' operator overloading.
     *  NOTE: This == operator dose not compare match types.
     */
    virtual bool operator==(const MinimalMatchListEntry & other) const;

    /*! \brief 'not equal' operator overloading.
     *  NOTE: This function compare type firstly,
     *        then compare distance and coordinate
     */
    virtual bool operator!=(const MinimalMatchListEntry & other) const;
};


/// Match list entry for configuration match list.
class ConfigMatchListEntry : public MinimalMatchListEntry {

public:
    /// The index in the global structure.
    int index;
};


/// Match list entry for process match list.
class ProcessMatchListEntry : public MinimalMatchListEntry {

public:

    /// Flag for indicating if we have a move coordinate.
    bool has_move_coordinate;

    /// The move vector cell component in the a direction.
    int move_cell_i;

    /// The move vector cell component in the b direction.
    int move_cell_j;

    /// The move vector cell component in the c direction.
    int move_cell_k;

    /// The basis component of the move vector.
    int move_basis;

    /// The move coordinate.
    Coordinate move_coordinate;

    /// The update type.
    int update_type;
};

#endif  // __MATHCLISTENTRY__

