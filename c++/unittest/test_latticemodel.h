/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


#ifndef __TEST_LATTICEMODEL__
#define __TEST_LATTICEMODEL__

#include <iostream>
#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestRunner.h>

#include <cppunit/extensions/HelperMacros.h>

class Test_LatticeModel : public CppUnit::TestCase {

public:

    CPPUNIT_TEST_SUITE( Test_LatticeModel );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testSetupAndQuery );
    CPPUNIT_TEST( testSingleStepFunction );
    CPPUNIT_TEST( testSingleStepFunction2D );
    CPPUNIT_TEST( testAffectedIndicesRematching );
    CPPUNIT_TEST( testRedistribute );
    CPPUNIT_TEST( testProcessRedistribute );
    CPPUNIT_TEST( testSingleStepWithRedistribution );
    CPPUNIT_TEST( testTiming );
    CPPUNIT_TEST_SUITE_END();

    void testConstruction();
    void testSetupAndQuery();
    void testSingleStepFunction();
    void testSingleStepFunction2D();
    void testAffectedIndicesRematching();
    void testRedistribute();
    void testProcessRedistribute();
    void testSingleStepWithRedistribution();
    void testTiming();

};

#endif

