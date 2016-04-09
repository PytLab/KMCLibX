/*
  Copyright (c)  2012-2013  Mikael Leetmaa

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

#include "coordinate.h"
#include <cmath>

/// The base class for the match list entries.
class MinimalMatchListEntry {

public:
    /// The index in the global structure.
    int index;

    /// The distance.
    double distance;

    /// The Coordinate.
    Coordinate coordinate;

    /// The match type integer.
    int match_types;

    /* Class functions. */
    
    /*! \brief 'less than' operator overloading for sorting matchlists.
     */
    virtual bool operator<(const MinimalMatchListEntry & other) const;

    /*! \brief 'equal' operator overloading.
     *  NOTE: This == operator dose not compare match types.
     */
    virtual bool operator==(const MinimalMatchListEntry & other) const;

    /*! \brief 'not equal' operator overloading.
     */
    virtual bool operator==(const MinimalMatchListEntry & other) const;
};

#endif // __MATCHLISTENTRY__

