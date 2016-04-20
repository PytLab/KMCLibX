/*
  Copyright (c)  2016-2019 Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


#ifndef __TEST_SITESMAP__
#define __TEST_SITESMAP__

#include <iostream>
#include <string>

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestRunner.h>

#include <cppunit/extensions/HelperMacros.h>

class Test_SitesMap : public CppUnit::TestCase {

public:

    CPPUNIT_TEST_SUITE( Test_SitesMap );
    CPPUNIT_TEST( testConstructionAndQuery );
    CPPUNIT_TEST( testMatchList );
    CPPUNIT_TEST( testInitMatchList );
    CPPUNIT_TEST_SUITE_END();

    void testConstructionAndQuery();
    void testMatchList();
    void testInitMatchList();

};

#endif
