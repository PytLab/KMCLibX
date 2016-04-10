/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


// Include the test definition.
#include "test_matchlist.h"

// Include the files to test.
#include "matchlist.h"
#include "configuration.h"

// -------------------------------------------------------------------------- //
//
void Test_MatchList::testCall()
{
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

}


// -------------------------------------------------------------------------- //
//
void Test_MatchList::test_ConfigurationsToMatchList()
{
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

}
