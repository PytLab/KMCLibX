/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : matchlist.cpp
 *  brief  : File for the implementation code of the match lists.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-10   1.2          Initial creation.
 *
 *  ------------------------------------------------------
 * ******************************************************************
 */

#include <algorithm>

#include "matchlist.h"
#include "configuration.h"


// -----------------------------------------------------------------------------
//
void configurationsToMatchList(const Configuration & first,
                               const Configuration & second,
                               int & range,
                               double & cutoff,
                               ProcessMatchList & match_list,
                               std::vector<int> & affected_indices,
                               const std::vector<int> & move_origins,
                               const std::vector<Coordinate> & move_vectors)
{
    // Get a handle to the coordinates and elements.
    const std::vector<Coordinate> & coords  = first.coordinates();

    // Get the first coordinate out to calculate the distance against.
    const Coordinate origin = coords[0];

    // Transform the configurations into match lists.
    for (size_t i = 0; i < first.elements().size(); ++i)
    {
        // Get the types as integers.
        const int first_type  = first.types()[i];
        const int second_type = second.types()[i];

        // Calculate the distance.
        const Coordinate coordinate = coords[i];
        const double distance = coordinate.distance(origin);

        // Save the cutoff.
        if (distance > cutoff)
        {
            cutoff = distance;
        }

        // Calculate the range based on the coordinates, such that all
        // needed coordinates are guarranteed to be included.
        const double x = coordinate.x();
        const int cmp_x = static_cast<int>( ( x < 0.0 ) ? (-1.0*x)+0.99999 : x );
        range = std::max(cmp_x, range);

        const double y = coordinate.y();
        const int cmp_y = static_cast<int>( ( y < 0.0 ) ? (-1.0*y)+0.99999 : y );
        range = std::max(cmp_y, range);

        const double z = coordinate.z();
        const int cmp_z = static_cast<int>( ( z < 0.0 ) ? (-1.0*z)+0.99999 : z );
        range = std::max(cmp_z, range);

        // Set up the match list.
        ProcessMatchListEntry m;
        m.match_type  = first_type;
        m.update_type = second_type;
        m.distance    = distance;
        m.coordinate  = coordinate;
        m.has_move_coordinate = false;
        m.move_coordinate = Coordinate(0.0, 0.0, 0.0);
        match_list.push_back(m);

        // If the first and second type differ increase the length of the
        // affected_sites list accordingly.
        if (first_type != second_type)
        {
            affected_indices.push_back(0);
        }
    }

    // Loop over the move vector origins and place the move vectors
    // on the match list entries before sorting.
    for (size_t i = 0; i < move_origins.size(); ++i)
    {
        const int move_origin = move_origins[i];
        match_list[move_origin].move_coordinate = move_vectors[i];
        match_list[move_origin].has_move_coordinate = true;
    }

    // Sort the match list.
    std::sort(match_list.begin(), match_list.end());
}
