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
    /*! brief constructor function
     */
    inline MinimalMatchListEntry();

    /*! \brief virtual destructor function
     */
    virtual ~MinimalMatchListEntry() {}
    
    /*! \brief 'less than' operator overloading for sorting matchlists.
     */
    virtual bool operator<(const MinimalMatchListEntry & other) const;

    /*! \brief 'equal' for comparing points(positions) of matchlist entry.
     *  NOTE: This == operator dose not compare match types.
     */
    virtual bool samePoint(const MinimalMatchListEntry & other) const;

    /*! \brief 'equal' for comparing type and positions.
     *  NOTE: This function compare type firstly,
     *        then compare distance and coordinate
     */
    virtual bool match(const MinimalMatchListEntry & other) const;
};


/// Forward declaration.
class ProcessMatchListEntry;

/// Match list entry for configuration match list.
class ConfigMatchListEntry : public MinimalMatchListEntry {

public:
    /// The index in the global structure.
    int index;

    /* Class functions.*/

    /* \brief Default constructor.
     */
    inline ConfigMatchListEntry();

    /* \brief Defualt destructor.
     */
    virtual ~ConfigMatchListEntry() {}
    
    /*! \brief explicit type coversion
     *         ProcessMatchListEntry -> ConfigMatchListEntry.
     */
    inline explicit ConfigMatchListEntry(const ProcessMatchListEntry & pe);

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

    /* Class functions.*/
    
    /* \brief Default constructor.
     */
    inline ProcessMatchListEntry();

    /* \brief Defualt destructor.
     */
    virtual ~ProcessMatchListEntry() {}

    /*! \brief explicit type coversion
     *         ConfigMatchListEntry -> ProcessMatchListEntry.
     */
    inline explicit ProcessMatchListEntry(const ConfigMatchListEntry & ce);

};


/// Match list entry for sitesmap match list.

class SitesMapMatchListEntry : public MinimalMatchListEntry {

public:
    /// The index in the global structrue.
    int index;

    /* Class functions.*/

    /* \brief Default constructor.
     */
    inline SitesMapMatchListEntry();

    /* \brief Defualt destructor.
     */
    virtual ~SitesMapMatchListEntry() {}

};


// --------------------------------------------------------------- //
// INLINE FUNCITON IMPLIMENTATION CODE
// --------------------------------------------------------------- //


// ---------------------------------------------------------------
//
MinimalMatchListEntry::MinimalMatchListEntry() :
    distance(0.0),
    coordinate(Coordinate(0.0, 0.0, 0.0)),
    match_type(0)
{
    // NOTHING HERE.
}


// ---------------------------------------------------------------
//
ConfigMatchListEntry::ConfigMatchListEntry() :
    index(0)
{
    // NOTHING HERE.
}


ConfigMatchListEntry::ConfigMatchListEntry(const ProcessMatchListEntry & pe) :
    MinimalMatchListEntry(pe)
{
    // NOTHING HERE.
}


// ---------------------------------------------------------------
//
ProcessMatchListEntry::ProcessMatchListEntry() :
    has_move_coordinate(false),
    move_cell_i(0),
    move_cell_j(0),
    move_cell_k(0),
    move_basis(0),
    move_coordinate(Coordinate(0.0, 0.0, 0.0)),
    update_type(0)
{
    // NOTHING HERE.
}


ProcessMatchListEntry::ProcessMatchListEntry(const ConfigMatchListEntry & ce) :
    MinimalMatchListEntry(ce)
{
    // NOTHING HERE.
}


// ---------------------------------------------------------------
//
SitesMapMatchListEntry::SitesMapMatchListEntry() :
    index(0)
{
    // NOTHING HERE.
}
#endif  // __MATHCLISTENTRY__

