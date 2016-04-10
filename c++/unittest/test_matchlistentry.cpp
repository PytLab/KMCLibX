/*
  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : test_matchlistentry.cpp
 *  brief  : File for all match list entry related unit test functions.
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

// Include the test definition.
#include "test_matchlistentry.h"

// Include the files to test.
#include "matchlistentry.h"


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryConstruction()
{
    // Construct.
    MinimalMatchListEntry m;
    m.match_type = 1324;
    m.distance = 1.2;
    m.coordinate = Coordinate(0.1,0.2,0.34);
    m.index = 123;

    // Check the member data.
    CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
    CPPUNIT_ASSERT_EQUAL(m.index, 123);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));

}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryEqualOperator()
{
    // Two equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( m1 == m2 );
    }

    // Different type but same distance and coordinate, should be equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 132;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( m1 == m2 );
    }
    
    // Different distance, same type and coordinate, should be unequal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.1;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 == m2) );
    }
    
    // Different coordinate(x), same type and distance, should be unequal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.3,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 == m2) );
    }

    // Different coordinate(y), same type and distance, should be unequal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.3,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 == m2) );
    }

    // Different coordinate(z), same type and distance, should be unequal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.37);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 == m2) );
    }
}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryNotEqualOperator()
{
    // Two equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 != m2) );
    }

    // Two equal - by wildcard.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 0;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 != m2) );
        CPPUNIT_ASSERT(  (m2 != m1) );
    }

    // Second is wildcard, but if the coordinate is different,
    // an coordinates_unmatched_error would be expected.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 0;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.2,0.2,0.34);

        CPPUNIT_ASSERT_THROW( !(m1 != m2), coordinates_unmatched_error );
        CPPUNIT_ASSERT(  (m2 != m1) );
    }

    // Two not equal in index, should equate to equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 2;

        CPPUNIT_ASSERT( !(m1 != m2) );
    }

    // Two not equal in match type should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );

    }

    // Two not equal in distance should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.23;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1322;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );
    }

    // If the distance difference is below eps_ hardcoded on the class
    // the difference is not visible.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.200000001;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.200000000;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 != m2) );

    }

    // But if the difference is larger it becomes visible.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2001;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.200000000;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );

    }

    // Two not equal in coordinate should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1001,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );

    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.4,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );
    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.24);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( (m1 != m2) );
    }

    // But if the difference is smaller than eps_ the value is
    // regarded as equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.100001,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 != m2) );

    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.200002,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 != m2) );
    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.3400001);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 != m2) );
    }

}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryLessOperator()
{
   // Two equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in index -> equal
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 3;

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in match type, thus equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in update type
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );

    }

    // e1 smaller than e2 in x
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.11,0.2,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );

    }

    // e1 smaller than e2 in y
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.21,0.34);
        m2.index = 1;

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in z
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.341);
        m2.index = 1;

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in z, also when the difference is tiny.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);
        m1.index = 1;

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.340000000000001);
        m2.index = 1;

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }
}


// ---------------------------------------------------------------------------
//
void Test_MatchListEntry::testProcessMatchListEntryConstruction()
{
    // Construct.
    ProcessMatchListEntry m;

    m.has_move_coordinate = true;
    m.move_cell_i = 1;
    m.move_cell_j = 2;
    m.move_cell_k = 8;
    m.move_basis = 1;
    m.match_type = 1324;
    m.update_type = 123;
    m.distance = 1.2;
    m.coordinate = Coordinate(0.1,0.2,0.34);
    m.index = 123;

    // Check the member data.
    CPPUNIT_ASSERT(m.has_move_coordinate);
    CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
    CPPUNIT_ASSERT_EQUAL(m.update_type, 123);
    CPPUNIT_ASSERT_EQUAL(m.move_cell_i, 1);
    CPPUNIT_ASSERT_EQUAL(m.move_cell_j, 2);
    CPPUNIT_ASSERT_EQUAL(m.move_cell_k, 8);
    CPPUNIT_ASSERT_EQUAL(m.move_basis, 1);
    CPPUNIT_ASSERT_EQUAL(m.index, 123);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));

}


// --------------------------------------------------------------------------
//
void Test_MatchListEntry::testConfigMatchListEntryConstruction()
{
    // Construct.
    ConfigMatchListEntry m;
    m.match_type = 1324;
    m.distance = 1.2;
    m.coordinate = Coordinate(0.1,0.2,0.34);
    m.index = 123;

    // Check the member data.
    CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
    CPPUNIT_ASSERT_EQUAL(m.index, 123);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));

}
