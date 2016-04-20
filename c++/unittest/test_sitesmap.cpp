/*
  Copyright (c)  2016-2019 Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/

/* \file   test_sitesmap.cpp
 * \brief  Test all functions in SitesMap class.
 * \author PytLab<shaozhengjiang@gmail.com>
 * \date   2016-04-08
 */

// Include the test definition.
#include "test_sitesmap.h"

// Include the files to test.
#include "sitesmap.h"

#include "coordinate.h"

// -------------------------------------------------------------------------//
//
void Test_SitesMap::testConstructionAndQuery()
{
    // Setup site coordinates.
    std::vector<std::vector<double> > coords(5, std::vector<double>(3, 0.0));
    coords[0][0]  = 0.1;
    coords[0][1]  = 0.2;
    coords[0][2]  = 0.3;
    coords[1][0]  = 0.4;
    coords[1][1]  = 0.5;
    coords[1][2]  = 0.6;
    coords[2][0]  = 0.7;
    coords[2][1]  = 0.8;
    coords[2][2]  = 0.9;
    coords[3][0]  = 1.1;
    coords[3][1] = 1.2;
    coords[3][2] = 1.3;
    coords[4][0] = 3.6;
    coords[4][1] = 3.5;
    coords[4][2] = 3.4;

    // Setup site types.
    std::vector<std::string> sites(5);
    sites[0] = "A";
    sites[1] = "B";
    sites[2] = "D";
    sites[3] = "H";
    sites[4] = "J";


    // Setup the mapping from type to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["D"] = 3;
    possible_types["H"] = 4;
    possible_types["J"] = 5;
    possible_types["G"] = 6;

    // Construct the sites map.
    SitesMap smap(coords, sites, possible_types);


    // Extract the member data and check that it is the same as what whent in.
    const std::vector<Coordinate> & ret_coords = smap.coordinates();
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ret_coords.size()),
                         static_cast<int>(coords.size()));

    for (size_t i = 0; i < coords.size(); ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(ret_coords[i][0], coords[i][0], 1.0e-12);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(ret_coords[i][1], coords[i][1], 1.0e-12);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(ret_coords[i][2], coords[i][2], 1.0e-12);
    }

    // Check the size of the sites.
    const std::vector<std::string> & ret_sites = smap.sites();
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ret_coords.size()),
                         static_cast<int>(ret_sites.size()));

    for (size_t i = 0; i < ret_sites.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(ret_sites[i], sites[i]);
    }

    // Check types.
    const std::vector<int> & ret_types = smap.types();
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ret_coords.size()),
                         static_cast<int>(ret_types.size()));

    const int types[5] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < ret_types.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(ret_types[i], types[i]);
    }

    // DONE

}

