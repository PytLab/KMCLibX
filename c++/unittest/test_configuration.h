/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


#ifndef __TEST_CONFIGURATION__
#define __TEST_CONFIGURATION__

#include <iostream>
#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestRunner.h>

#include <cppunit/extensions/HelperMacros.h>

class Test_Configuration : public CppUnit::TestCase {

public:

    CPPUNIT_TEST_SUITE( Test_Configuration );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testResetFastFlags );
    CPPUNIT_TEST( testMovedAtomIDsRecentMoveVectorsSize );
    CPPUNIT_TEST( testPerformProcess );
    CPPUNIT_TEST( testPerformProcessVectors );
    CPPUNIT_TEST( testAtomID );
    CPPUNIT_TEST( testMatchLists );
    CPPUNIT_TEST( testTypeNameQuery );
    CPPUNIT_TEST( testAtomIDElementsCoordinatesMovedIDs );
    CPPUNIT_TEST( testSubConfiguration );
    CPPUNIT_TEST( testSplit );
    CPPUNIT_TEST( testExtractFastSpecies );
    CPPUNIT_TEST( testFastIndices );
    CPPUNIT_TEST( testSlowIndices );
    CPPUNIT_TEST_SUITE_END();

    void testConstruction();
    void testResetFastFlags();
    void testMovedAtomIDsRecentMoveVectorsSize();
    void testPerformProcess();
    void testPerformProcessVectors();
    void testAtomID();
    void testMatchLists();
    void testAtomIDElementsCoordinatesMovedIDs();
    void testTypeNameQuery();
    void testSubConfiguration();
    void testSplit();
    void testExtractFastSpecies();
    void testFastIndices();
    void testSlowIndices();

};

#endif

