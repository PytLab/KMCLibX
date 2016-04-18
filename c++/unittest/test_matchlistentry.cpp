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
    // {{{

    // Construct.
    MinimalMatchListEntry m;
    m.match_type = 1324;
    m.distance = 1.2;
    m.coordinate = Coordinate(0.1,0.2,0.34);

    // Check the member data.
    CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntrySamePoint()
{
    // {{{
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

        CPPUNIT_ASSERT( m1.samePoint(m2) );
        CPPUNIT_ASSERT( m2.samePoint(m1) );
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

        CPPUNIT_ASSERT( m1.samePoint(m2) );
        CPPUNIT_ASSERT( m2.samePoint(m1) );
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

        CPPUNIT_ASSERT( !(m1.samePoint(m2)) );
        CPPUNIT_ASSERT( !(m2.samePoint(m1)) );
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

        CPPUNIT_ASSERT( !(m1.samePoint(m2)) );
        CPPUNIT_ASSERT( !(m2.samePoint(m1)) );
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

        CPPUNIT_ASSERT( !(m1.samePoint(m2)) );
        CPPUNIT_ASSERT( !(m2.samePoint(m1)) );
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

        CPPUNIT_ASSERT( !(m1.samePoint(m2)) );
        CPPUNIT_ASSERT( !(m2.samePoint(m1)) );
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryMatch()
{
    // {{{
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

        CPPUNIT_ASSERT(m1.match(m2));
        CPPUNIT_ASSERT(m2.match(m1));
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

        CPPUNIT_ASSERT(m1.match(m2));
        CPPUNIT_ASSERT(!m2.match(m1));
    }

    // Two not equal in match type should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));

    }

    // Two not equal in distance should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.23;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1322;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));
    }

    // If the distance difference is below eps_ hardcoded on the class
    // the difference is not visible.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.200000001;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.200000000;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.match(m2));

    }

    // But if the difference is larger it becomes visible.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2001;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.200000000;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));

    }

    // Two not equal in coordinate should equate to not equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1001,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));

    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.4,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));
    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.24);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(!m1.match(m2));
    }

    // But if the difference is smaller than eps_ the value is
    // regarded as equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.100001,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.match(m2));

    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.200002,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.match(m2));
    }
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.3400001);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.match(m2));
    }

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_MatchListEntry::testMinimalMatchListEntryLessOperator()
{
    // {{{

    // Two equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in index -> equal
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in match type, thus equal.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1322;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in update type
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT( !(m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );

    }

    // e1 smaller than e2 in x
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.11,0.2,0.34);

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );

    }

    // e1 smaller than e2 in y
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.21,0.34);

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in z
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.341);

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // e1 smaller than e2 in z, also when the difference is tiny.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.20;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        MinimalMatchListEntry m2;
        m2.match_type = 1324;
        m2.distance = 1.20;
        m2.coordinate = Coordinate(0.1,0.2,0.340000000000001);

        CPPUNIT_ASSERT(  (m1 < m2) );
        CPPUNIT_ASSERT( !(m2 < m1) );
    }

    // }}}
}


// ---------------------------------------------------------------------------
//
void Test_MatchListEntry::testProcessMatchListEntryConstruction()
{
    // {{{
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

        // Check the member data.
        CPPUNIT_ASSERT(m.has_move_coordinate);
        CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
        CPPUNIT_ASSERT_EQUAL(m.update_type, 123);
        CPPUNIT_ASSERT_EQUAL(m.move_cell_i, 1);
        CPPUNIT_ASSERT_EQUAL(m.move_cell_j, 2);
        CPPUNIT_ASSERT_EQUAL(m.move_cell_k, 8);
        CPPUNIT_ASSERT_EQUAL(m.move_basis, 1);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
        CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));
    }
    // Test type conversion function.
    {
        ConfigMatchListEntry ce;

        ce.match_type = 1324;
        ce.distance = 1.2;
        ce.coordinate = Coordinate(0.1,0.2,0.34);
        ce.index = 123;
    
        ProcessMatchListEntry pe(ce);

        CPPUNIT_ASSERT_EQUAL(pe.match_type, 1324);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pe.distance, 1.2, 1.0e-12);
        CPPUNIT_ASSERT_EQUAL(pe.coordinate, Coordinate(0.1, 0.2, 0.34));
    }
    /*}}}*/
}


// ---------------------------------------------------------------------------
//
void Test_MatchListEntry::testProcessMatchListEntrySamePoint()
{
    //{{{
    // MinimalMatchListEntry && ProcessMatchListEntry.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        ProcessMatchListEntry m2;
        m2.match_type = 1324;
        m2.update_type = 123;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.samePoint(m2));
        CPPUNIT_ASSERT(m2.samePoint(m1));
    }
    // ConfigMatchList && ProcessMatchList
    {
        ConfigMatchListEntry m1;
        m1.index = 1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        ProcessMatchListEntry m2;
        m2.match_type = 1324;
        m2.update_type = 123;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.samePoint(m2));
        CPPUNIT_ASSERT(m2.samePoint(m1));
    }
    //}}}
}


// --------------------------------------------------------------------------
//
void Test_MatchListEntry::testConfigMatchListEntryConstruction()
{
    //{{{

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
    // Test type conversion function.
    {   
        ProcessMatchListEntry pe;

        pe.has_move_coordinate = true;
        pe.move_cell_i = 1;
        pe.move_cell_j = 2;
        pe.move_cell_k = 8;
        pe.move_basis = 1;
        pe.match_type = 1324;
        pe.update_type = 123;
        pe.distance = 1.2;
        pe.coordinate = Coordinate(0.1,0.2,0.34);
    
        ConfigMatchListEntry ce(pe);

        // Check the member data.
        CPPUNIT_ASSERT_EQUAL(ce.match_type, 1324);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(ce.distance, 1.2, 1.0e-12);
        CPPUNIT_ASSERT_EQUAL(ce.coordinate, Coordinate(0.1, 0.2, 0.34));
    }

    //}}}
}


// ---------------------------------------------------------------------------
//
void Test_MatchListEntry::testConfigMatchListEntrySamePoint()
{
    //{{{
    // MinimalMatchListEntry && ConfigMatchListEntry.
    {
        MinimalMatchListEntry m1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        ConfigMatchListEntry m2;
        m2.index = 1;
        m2.match_type = 1324;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.samePoint(m2));
        CPPUNIT_ASSERT(m2.samePoint(m1));
    }
    // ConfigMatchList && ProcessMatchList
    {
        ConfigMatchListEntry m1;
        m1.index = 1;
        m1.match_type = 1324;
        m1.distance = 1.2;
        m1.coordinate = Coordinate(0.1,0.2,0.34);

        ProcessMatchListEntry m2;
        m2.match_type = 1324;
        m2.update_type = 123;
        m2.distance = 1.2;
        m2.coordinate = Coordinate(0.1,0.2,0.34);

        CPPUNIT_ASSERT(m1.samePoint(m2));
        CPPUNIT_ASSERT(m2.samePoint(m1));
    }
    //}}}
}


// ---------------------------------------------------------------------------
//
void Test_MatchListEntry::testSitesMapMatchListEntryConstruction()
{
    // {{{

    // Construct.
    SitesMapMatchListEntry m;
    m.match_type = 1324;
    m.distance = 1.2;
    m.coordinate = Coordinate(0.1,0.2,0.34);
    m.index = 123;

    // Check the member data.
    CPPUNIT_ASSERT_EQUAL(m.match_type, 1324);
    CPPUNIT_ASSERT_EQUAL(m.index, 123);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m.distance, 1.2, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(m.coordinate, Coordinate(0.1, 0.2, 0.34));

    // }}}
}

