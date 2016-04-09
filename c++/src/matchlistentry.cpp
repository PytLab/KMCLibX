/*
  Copyright (c)  2016-2019 Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matchlistentry.cpp
 *  brief  : File for the implementation code of the match list entries.
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

#include "matchlistentry.h"


/*! \brief The epsilon value for comparing lattice positions.
 */
static double epsi__ = 1.0e-5;

/*! \brief operator overloading inline funtion implementation.
 */

// -------------------------------------------------------------------------
//
bool MinimalMatchListEntry::operator==(const MinimalMatchListEntry & other) const
{
    // Check the distance.
    if (std::fabs(distance - other.distance) > epsi__)
    {
        return false;
    }
    // Check the coordinate.
    else if (std::fabs(coordinate.x() - other.coordinate.x()) > epsi__)
    {
        return false;
    }
    else if (std::fabs(coordinate.y() - other.coordinate.y()) > epsi__)
    {
        return false;
    }
    else if (std::fabs(coordinate.z() - other.coordinate.z()) > epsi__)
    {
        return false;
    }
    return true;
}

// -------------------------------------------------------------------------
//
bool MinimalMatchListEntry::operator!=(const MinimalMatchListEntry & other) const
{
    // Handle the wildcard case.
    if (other.match_type == 0)
    {
        return false;
    }
    // Check the type.
    else if (match_type != other.match_type)
    {
        return true;
    }
    else
    {
        return !(*this == other);
    }
}


/*! \brief 'less than' for sorting matchlists.
 */
bool MinimalMatchListEntry::operator<(const MinimalMatchListEntry & other)
{

    // Sort along distance and coordinate.
    if (std::fabs(distance - other.distance) < epsi__)
    {
        // If the distances are practically the same,
        // check the coordinate.
        return (other.coordinate < coordinate);
    }
    else
    {
        // Sort wrt. distance.
        return (other.distance < distance);
    }
}
