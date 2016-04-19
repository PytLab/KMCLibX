/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : test_matchlist.cpp
 *  brief  : File for the unit test functions of the MatchList class.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-10   1.2          Initial creation.
 *
 *  -----------------------------------------------------
 * ******************************************************************
 */

#include <algorithm>

// Include the test definition.
#include "test_matchlist.h"

// Include the files to test.
#include "matchlist.h"
#include "configuration.h"

// -------------------------------------------------------------------------- //
//
void Test_MatchList::testCall()
{
    // {{{
    // Construct.
    ProcessMatchList m1(2);
    m1[0].match_type = 1;
    m1[0].distance = 1.2;
    m1[0].coordinate = Coordinate(0.1, 0.2, 0.3);
    m1[0].has_move_coordinate = true;
    m1[0].move_coordinate = Coordinate(0.0, 0.1, 0.1);

    m1[1].match_type = 3;
    m1[1].distance = 1.2;
    m1[1].coordinate = Coordinate(0.1, 0.2, 0.3);
    m1[1].has_move_coordinate = true;
    m1[1].move_coordinate = Coordinate(0.0, 0.1, 0.1);

    ConfigMatchList m2(2);
    m2[0].match_type = 1;
    m2[0].distance = 1.2;
    m2[0].index = 1;
    m2[0].coordinate = Coordinate(0.1, 0.2, 0.4);

    m2[1].match_type = 1;
    m2[1].distance = 1.3;
    m2[1].index = 7;
    m2[1].coordinate = Coordinate(0.4, 0.2, 0.4);

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchList::test_ConfigurationsToMatchList()
{
    // {{{
    // Configuration elements.
    const std::vector<std::string> elements1 = {
        "A", "B", "V", "A", "A",
        "A", "B", "V", "A", "A"
    };
    const std::vector<std::string> elements2 = {
        "B", "B", "V", "A", "A",
        "V", "B", "V", "A", "A"
    };

    // Configuration coordinates.
    const std::vector< std::vector<double> > process_coords = {
        { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5}, 
        { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
        {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        { 1.0,  0.0,  0.0}
    };

    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Construct two local configurations.
    const Configuration first(process_coords, elements1, possible_types);
    const Configuration second(process_coords, elements2, possible_types);

    // Output variables.
    int range = 0;
    double cutoff = 0.0;
    ProcessMatchList match_list(0);
    std::vector<int> affected_indices(0);

    // Get the corresponding match list.
    configurationsToMatchList(first, second, range, cutoff, 
                              match_list, affected_indices);

    CPPUNIT_ASSERT_EQUAL(range, 1);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(cutoff, 1.0, 1.0e-12);

    CPPUNIT_ASSERT_EQUAL(static_cast<int>(affected_indices.size()), 2);

    // Check sorted match list.
    std::vector<int> before = {1, 1, 1, 2, 3, 3, 1, 1, 2, 1};
    std::vector<int> after = {2, 1, 3, 2, 3, 3, 1, 1, 2, 1};

    for (int i = 0; i < 10; ++i)
    {
        const ProcessMatchListEntry & e = match_list[i];
        CPPUNIT_ASSERT_EQUAL(e.match_type, before[i]);
        CPPUNIT_ASSERT_EQUAL(e.update_type, after[i]);
    }

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchList::test_WhateverMatchProcessConfig()
{
    // {{{

    // Use configurationsToMatchList to get process match list.
    // Configuration elements.
    const std::vector<std::string> elements1 = {
        "A", "B", "V", "A", "A",
        "A", "B", "V", "A", "A"
    };
    const std::vector<std::string> elements2 = {
        "B", "B", "V", "A", "A",
        "V", "B", "V", "A", "A"
    };

    // Configuration coordinates.
    const std::vector< std::vector<double> > process_coords = {
        { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5}, 
        { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
        {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        { 1.0,  0.0,  0.0}
    };

    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Construct two local configurations.
    const Configuration first(process_coords, elements1, possible_types);
    const Configuration second(process_coords, elements2, possible_types);

    // Output variables.
    int range = 0;
    double cutoff = 0.0;
    ProcessMatchList process_match_list(0);
    std::vector<int> affected_indices(0);

    configurationsToMatchList(first, second, range, cutoff, 
                              process_match_list, affected_indices);

    // Get a configuration match list.
    ConfigMatchList config_match_list(0);
    for (size_t i = 0; i < elements1.size(); ++i)
    {
        ConfigMatchListEntry ce;

        ce.index = i;
        const std::vector<double> & coord = process_coords[i];
        ce.coordinate = Coordinate(coord[0], coord[1], coord[2]);
        ce.distance = ce.coordinate.distanceToOrigin();
        std::string element = elements1[i];
        ce.match_type = possible_types[element];

        config_match_list.push_back(ce);
    }

    // Sort the configuration match list.
    std::sort(config_match_list.begin(), config_match_list.end());

    // Check match.
    bool is_match = false;
    is_match = whateverMatch(process_match_list, config_match_list);
    CPPUNIT_ASSERT(is_match);

    // Change one type, should be unmatched.
    // 1 --> 3
    config_match_list[0].match_type = 3;
    // Check match.
    is_match = whateverMatch(process_match_list, config_match_list);
    CPPUNIT_ASSERT(!is_match);

    // Change the first entry of configuration to wildcard,
    // should be still ummatched.
    config_match_list[0].match_type = 0;
    // Check match.
    is_match = whateverMatch(process_match_list, config_match_list);
    CPPUNIT_ASSERT(!is_match);

    // Change the first entry of process to wildcard, should be matched.
    process_match_list[0].match_type = 0;
    // Check match.
    is_match = whateverMatch(process_match_list, config_match_list);
    CPPUNIT_ASSERT(is_match);

    // Change the coordinate of the 2nd entry, should be ummatched.
    process_match_list[1].coordinate = Coordinate(1.0, 1.0, 1.0);
    // Check match.
    is_match = whateverMatch(process_match_list, config_match_list);
    CPPUNIT_ASSERT(!is_match);

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchList::test_WhateverMatchProcessSite()
{
    // {{{

    // Use configurationsToMatchList to get process match list.
    // Configuration elements.
    const std::vector<std::string> elements1 = {
        "A", "B", "V", "A", "A",
        "A", "B", "V", "A", "A"
    };
    const std::vector<std::string> elements2 = {
        "B", "B", "V", "A", "A",
        "V", "B", "V", "A", "A"
    };

    // Configuration coordinates.
    const std::vector< std::vector<double> > process_coords = {
        { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5}, 
        { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
        {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        { 1.0,  0.0,  0.0}
    };

    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Construct two local configurations.
    const Configuration first(process_coords, elements1, possible_types);
    const Configuration second(process_coords, elements2, possible_types);

    // Output variables.
    int range = 0;
    double cutoff = 0.0;
    ProcessMatchList process_match_list(0);
    std::vector<int> affected_indices(0);

    configurationsToMatchList(first, second, range, cutoff, 
                              process_match_list, affected_indices);

    // --------------------------------------------------------
    // Now the site type in process match list is 0 (wildcard).
    // --------------------------------------------------------

    // Get a site match list.
    SiteMatchList site_match_list(0);
    const std::vector<int> site_types = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (size_t i = 0; i < elements1.size(); ++i)
    {
        SiteMatchListEntry se;

        se.index = i;
        const std::vector<double> & coord = process_coords[i];
        se.coordinate = Coordinate(coord[0], coord[1], coord[2]);
        se.distance = se.coordinate.distanceToOrigin();
        std::string element = elements1[i];
        se.match_type = site_types[i];

        site_match_list.push_back(se);
    }

    // Sort the configuration match list.
    std::sort(site_match_list.begin(), site_match_list.end());

    // Check match.
    bool is_match = false;
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(is_match);

    // Change one type, should still be matched.
    // 1 --> 3
    const int orig_type = site_match_list[0].match_type;
    site_match_list[0].match_type = 3;
    // Check match.
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(is_match);
    // Recover.
    site_match_list[0].match_type = orig_type;

    // NOTE : Change the coordinate of the 2nd entry, should still be matched.
    //        If we detect that the process entry is a wildcard, then
    //        we will not check the others e.g. coordinate and distance.
    //        We do not check data validity in C++ backend.
    const Coordinate orig_coord = process_match_list[1].coordinate;
    process_match_list[1].coordinate = Coordinate(1.0, 1.0, 1.0);
    // Check match.
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(is_match);
    // Recover.
    process_match_list[1].coordinate = orig_coord;

    // So does the different distance.
    const double orig_dist = process_match_list[1].distance;
    process_match_list[1].distance = 3.9;
    // Check match.
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(is_match);
    // Recover.
    process_match_list[1].distance = orig_dist;

    // --------------------------------------------------------
    // Now we add site types to process match list.
    // --------------------------------------------------------

    for (size_t i = 0; i < process_match_list.size(); ++i)
    {
        process_match_list[i].site_type = site_match_list[i].match_type;
    }

    // Check match.

    // Same site type, should be matched.
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(is_match);

    // Change site type of the first process match list entry.
    // Should not be matched.
    process_match_list[1].site_type = 11;
    is_match = whateverMatch(process_match_list, site_match_list);
    CPPUNIT_ASSERT(!is_match);

    // }}}
}

