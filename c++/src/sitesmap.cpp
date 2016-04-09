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

// -------------------------------------------------------------------------------
//
SitesMap::SitesMap(const std::vector< std::vector<double> > & coordinates,
                   const std::vector<std::string> & sites,
                   const std::map<std::string, int> & possible_types) :
    sites_(sites),
    possible_types_(possible_types)
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

