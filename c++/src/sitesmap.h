/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/

/* ****************************************************************************
 * file   : sitesmap.h
 * brief  : Class for defining the sites information used in a KMC simulation to
 *          use for communicating site types and positions to and from python.
 * author : zjshao <shaozhengjiang@gmail.com>
 * date   : 2016-04-08
 *
 * history:
 * <author>   <time>       <version>    <desc>
 * ------------------------------------------------------
 * zjshao     2016-04-08   2.0          Initial creation.
 *
 * ------------------------------------------------------
 * ****************************************************************************/

#ifndef __SITESMAP__
#define __SITESMAP__

#include <vector>
#include <string>
#include <map>

// Forward declaration.
class LatticeMap;
class Coordinate;
class SiteMatchListEntry;

// Typedef.
typedef std::vector<SiteMatchListEntry> SiteMatchList;


class SitesMap 
{
public:

    /*! \brief Constructor for setting up the sites map.
     *  \param coordinates   : The coordinates of all sites.
     *  \param sites         : Type strings for all sites.
     *  \param possible_types: A global mapping from site type string to 
     *                         number(site type id)
     */
    SitesMap(const std::vector< std::vector<double> > & coordinates,
             const std::vector<std::string> & sites,
             const std::map<std::string, int> & possible_types);

    /*! \brief Initiate the calculation of the match lists.
     *  \param lattice_map : The lattice map needed to get coordinates wrapped.
     *  \param range       : The number of shells to include.
     */
    void initMatchLists(const LatticeMap & lattice_map, const int range);

    /*! \brief Construct and return the sitesmap match list for the
     *         given list of indices.
     *  \param origin_index : The index to treat as the origin.
     *  \param indices      : The indices to get the match list for.
     *  \param lattice_map  : The lattice map needed for calculating distances
     *                        using correct boundaries.
     *  \return : The sitesmap match list.
     */
    const SiteMatchList & matchList(const int origin_index,
                                    const std::vector<int> & indices,
                                    const LatticeMap & lattice_map) const;

    /*! \brief Return the cached match list without update.
     *  \param index : The index to get the match list for.
     *  \return : The match list.
     */
    const SiteMatchList & matchList(const int index) const { return match_lists_[index]; }

    /*! \brief Const query for the site coordinates.
     *  \return : The coordinates of all sites on lattice.
     */
    const std::vector<Coordinate> & coordinates() { return coordinates_; }

    /*! \brief Const query for the site type string.
     *  \return : The site type strings of all sites on lattice.
     */
    const std::vector<std::string> & sites() { return sites_; }

    /*! \brief Const query for the site type numbers.
     *  \return : The site type numbers of all sites on lattice.
     */
    const std::vector<int> & types() { return types_; }

private:

    /// The site coordinates on lattice.
    std::vector<Coordinate> coordinates_;

    /// All site types on lattice presented in int.
    std::vector<int> types_;

    /// All site types on lattice presented in string.
    std::vector<std::string> sites_;

    /// Mapping from type string to type int.
    std::map<std::string, int> possible_types_;

    /// Site match lists.
    std::vector<SiteMatchList> match_lists_;

};

#endif  // __SITESMAP__
