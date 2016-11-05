/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


// Include the test definition.
#include "test_matcher.h"

// Include the file to test.
#include "matcher.h"
#include "matchlist.h"
#include "matchlistentry.h"
#include "configuration.h"
#include "latticemap.h"
#include "process.h"
#include "interactions.h"
#include "random.h"
#include "sitesmap.h"


static const double epsilon__ = 1e-10;


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testConstruction()
{
    // Construct.
    Matcher m;
}

// -------------------------------------------------------------------------- //
//
void Test_Matcher::testIsMatchMatchList()
{
    // {{{

    // Construct.
    Matcher matcher;

    // Setup two match lists to check.
    ProcessMatchList process_match_list;
    ProcessMatchListEntry m;
    m.match_type  = 3;
    m.update_type = 2;
    m.distance     = 1.2;
    m.coordinate   = Coordinate(0.1,2.0,0.34);
    process_match_list.push_back(m);

    m.match_type  = 1;
    m.update_type = 4;
    m.distance     = 1.6;
    m.coordinate   = Coordinate(1.1,2.0,0.34);
    process_match_list.push_back(m);

    m.match_type  = 8;
    m.update_type = 2;
    m.distance     = 1.9;
    m.coordinate   = Coordinate(0.1,5.2,0.34);
    process_match_list.push_back(m);

    ConfigMatchList index_match_list;
    ConfigMatchListEntry n;
    n.match_type  = 3;
    n.distance     = 1.2;
    n.coordinate   = Coordinate(0.1,2.0,0.34);
    index_match_list.push_back(n);

    n.match_type  = 1;
    n.distance     = 1.6;
    n.coordinate   = Coordinate(1.1,2.0,0.34);
    index_match_list.push_back(n);

    n.match_type  = 8;
    n.distance     = 1.9;
    n.coordinate   = Coordinate(0.1,5.2,0.34);
    index_match_list.push_back(n);

    // These two are equal.
    CPPUNIT_ASSERT( whateverMatch(process_match_list, index_match_list) );

    // It works also if the index match list is longer.
    n.match_type  = 8;
    n.distance     = 1.93;
    n.coordinate   = Coordinate(0.13,5.2,0.34);
    index_match_list.push_back(n);

    // These two are still equal.
    CPPUNIT_ASSERT( whateverMatch(process_match_list, index_match_list) );

    // If we add another not matching element to the process_match_list vecctor
    // we get a non-match.
    m.match_type  = 3;
    m.distance     = 1.93;
    m.coordinate   = Coordinate(0.13,5.2,0.34);
    process_match_list.push_back(m);

    // These two are no longer equal.
    CPPUNIT_ASSERT( !whateverMatch(process_match_list, index_match_list) );

    // But if they differ in the update index they are still equal.
    m.match_type  = 8;
    m.distance     = 1.93;
    m.coordinate   = Coordinate(0.13,5.2,0.34);
    process_match_list[3] = m;

    // These two are again equal.
    CPPUNIT_ASSERT( whateverMatch(process_match_list, index_match_list) );

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testIsMatchWildcard()
{
    // {{{

    // Construct.
    Matcher matcher;

    // Setup two match lists to check.
    ProcessMatchList process_match_list;
    ProcessMatchListEntry m;
    m.match_type  = 3;
    m.update_type = 2;
    m.distance     = 1.2;
    m.coordinate   = Coordinate(0.1,2.0,0.34);
    process_match_list.push_back(m);

    // Add a wildcard, 0.
    m.match_type  = 0;
    m.update_type = 4;
    m.distance     = 1.6;
    m.coordinate   = Coordinate(1.1,2.0,0.34);
    process_match_list.push_back(m);

    m.match_type  = 8;
    m.update_type = 2;
    m.distance     = 1.9;
    m.coordinate   = Coordinate(0.1,5.2,0.34);
    process_match_list.push_back(m);

    ConfigMatchList index_match_list;
    ConfigMatchListEntry n;
    n.match_type  = 3;
    n.distance     = 1.2;
    n.coordinate   = Coordinate(0.1,2.0,0.34);
    index_match_list.push_back(n);

    // Note that no wildcard here.
    n.match_type  = 1;
    n.distance     = 1.6;
    n.coordinate   = Coordinate(1.1,2.0,0.34);
    index_match_list.push_back(n);

    n.match_type  = 8;
    n.distance     = 1.9;
    n.coordinate   = Coordinate(0.1,5.2,0.34);
    index_match_list.push_back(n);

    // These two are equal.
    CPPUNIT_ASSERT( whateverMatch(process_match_list, index_match_list) );

    // But the wildcard is only skipped in the process match list, so swapping
    // place creates a mismatch.
    CPPUNIT_ASSERT( !whateverMatch(index_match_list, process_match_list) );

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testIsMatchIndexListMinimal()
{
    // {{{

    // Construct.
    Matcher m;

    // ---------------------------------------------------------------------
    // Setup a valid configuration.
    std::vector<std::vector<double> > coords(2, std::vector<double>(3, 0.0));

    // One cell with two atoms.
    coords[0][0] = 0.0;
    coords[0][1] = 0.0;
    coords[0][2] = 0.0;
    coords[1][0] = 0.5;
    coords[1][1] = 0.5;
    coords[1][2] = 0.5;

    // Setup elements.
    std::vector<std::string> elements(2);
    elements[0] = "A";
    elements[1] = "B";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // ---------------------------------------------------------------------
    // Setup a non periodic cooresponding lattice map.

    const std::vector<int> repetitions(3, 1);
    const std::vector<bool> periodicity(3, false);
    const int basis = 2;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    LatticeMap lattice_map(basis, repetitions, periodicity);
    config.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Construct a processes and test.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("A");
        elements1.push_back("B");
        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("B");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
        process_coords[1][0] =  0.5;
        process_coords[1][1] =  0.5;
        process_coords[1][2] =  0.5;
        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This is a match.
        CPPUNIT_ASSERT( whateverMatch(process.matchList(), config.matchList(0)) );

        // This is not a match.
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(1)) );
    }

    // Construct another process that should match the second index.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("B");
        elements1.push_back("A");
        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("A");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
        process_coords[1][0] =  -0.5;
        process_coords[1][1] =  -0.5;
        process_coords[1][2] =  -0.5;
        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This is a match.
        CPPUNIT_ASSERT( whateverMatch(process.matchList(), config.matchList(1)) );

        // This is not a match.
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(0)) );
    }

    // This process does not match any. Note that the symmetry / direction is important.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("B");
        elements1.push_back("A");
        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("A");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
        process_coords[1][0] =  -0.5;
        process_coords[1][1] =  -0.5;
        process_coords[1][2] =   0.5;
        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // Not a match.
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(1)) );
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(0)) );
    }

    // This process does not match any site since it is to long.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("B");
        elements1.push_back("A");
        elements1.push_back("C");
        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("A");
        elements2.push_back("C");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(3,std::vector<double>(3,0.0));
        process_coords[1][0] =  -0.5;
        process_coords[1][1] =  -0.5;
        process_coords[1][2] =  -0.5;
        process_coords[2][0] =   0.5;
        process_coords[2][1] =   0.5;
        process_coords[2][2] =   0.5;
        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // Not a match.
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(1)) );
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(0)) );
    }

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testIsMatchIndexListMinimalPeriodic()
{
    // {{{
    // Note that a minimal periodic configuration is a 3x3x3 cell.
    // The reason for this is that it simplifies boundary condition
    // treatment alot, and it is highly unlikely that a real lattice KMC
    // simulation would be meaningfull on a smaller system any way.

    // Construct.
    Matcher m;

    // ---------------------------------------------------------------------
    // Setup a valid configuration.

    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;

    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("A");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
            }
        }
    }

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);
    config.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Construct a processes and test.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("A");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(9,std::vector<double>(3,0.0));
        process_coords[1][0] =  0.5;
        process_coords[1][1] =  0.5;
        process_coords[1][2] =  0.5;

        process_coords[2][0] = -0.5;
        process_coords[2][1] =  0.5;
        process_coords[2][2] =  0.5;

        process_coords[3][0] =  0.5;
        process_coords[3][1] = -0.5;
        process_coords[3][2] =  0.5;

        process_coords[4][0] =  0.5;
        process_coords[4][1] =  0.5;
        process_coords[4][2] = -0.5;

        process_coords[5][0] = -0.5;
        process_coords[5][1] = -0.5;
        process_coords[5][2] =  0.5;

        process_coords[6][0] = -0.5;
        process_coords[6][1] =  0.5;
        process_coords[6][2] = -0.5;

        process_coords[7][0] =  0.5;
        process_coords[7][1] = -0.5;
        process_coords[7][2] = -0.5;

        process_coords[8][0] = -0.5;
        process_coords[8][1] = -0.5;
        process_coords[8][2] = -0.5;

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This process should match all even numbered indices.
        for (int i = 0; i < 26; i += 2)
        {
            CPPUNIT_ASSERT(  whateverMatch(process.matchList(), config.matchList(i)) );
            CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(i+1)) );
        }
    }

    // ---------------------------------------------------------------------
    // Test an even larger local environment.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("A");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("A");
        elements1.push_back("A");
        elements1.push_back("A");
        elements1.push_back("A");
        elements1.push_back("A");
        elements1.push_back("A");

        std::vector<std::string> elements2;
        elements2.push_back("C");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(15,std::vector<double>(3,0.0));
        process_coords[1][0] =  0.5;
        process_coords[1][1] =  0.5;
        process_coords[1][2] =  0.5;

        process_coords[2][0] = -0.5;
        process_coords[2][1] =  0.5;
        process_coords[2][2] =  0.5;

        process_coords[3][0] =  0.5;
        process_coords[3][1] = -0.5;
        process_coords[3][2] =  0.5;

        process_coords[4][0] =  0.5;
        process_coords[4][1] =  0.5;
        process_coords[4][2] = -0.5;

        process_coords[5][0] = -0.5;
        process_coords[5][1] = -0.5;
        process_coords[5][2] =  0.5;

        process_coords[6][0] = -0.5;
        process_coords[6][1] =  0.5;
        process_coords[6][2] = -0.5;

        process_coords[7][0] =  0.5;
        process_coords[7][1] = -0.5;
        process_coords[7][2] = -0.5;

        process_coords[8][0] = -0.5;
        process_coords[8][1] = -0.5;
        process_coords[8][2] = -0.5;

        process_coords[9][0] =  1.0;
        process_coords[9][1] =  0.0;
        process_coords[9][2] =  0.0;

        process_coords[10][0] =  0.0;
        process_coords[10][1] =  1.0;
        process_coords[10][2] =  0.0;

        process_coords[11][0] =  0.0;
        process_coords[11][1] =  0.0;
        process_coords[11][2] =  1.0;

        process_coords[12][0] = -1.0;
        process_coords[12][1] =  0.0;
        process_coords[12][2] =  0.0;

        process_coords[13][0] =  0.0;
        process_coords[13][1] = -1.0;
        process_coords[13][2] =  0.0;

        process_coords[14][0] =  0.0;
        process_coords[14][1] =  0.0;
        process_coords[14][2] = -1.0;


        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This process should match all even numbered indices.
        for (int i = 0; i < 26; i += 2)
        {
            CPPUNIT_ASSERT(  whateverMatch(process.matchList(), config.matchList(i)) );
            CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(i+1)) );
        }
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testIsMatchIndexListComplicatedPeriodic()
{
    // {{{
    // Construct.
    Matcher m;

    // ---------------------------------------------------------------------
    // Setup a valid configuration.

    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("C");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
            }
        }
    }

    // Modify the elements such that the central cell has an A in center.
    elements[0] = "A";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);
    config.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Construct a processes and test.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("C");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        std::vector<std::string> elements2;
        elements2.push_back("D");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(9,std::vector<double>(3,0.0));
        process_coords[1][0] =  0.5;
        process_coords[1][1] =  0.5;
        process_coords[1][2] =  0.5;

        process_coords[2][0] = -0.5;
        process_coords[2][1] =  0.5;
        process_coords[2][2] =  0.5;

        process_coords[3][0] =  0.5;
        process_coords[3][1] = -0.5;
        process_coords[3][2] =  0.5;

        process_coords[4][0] =  0.5;
        process_coords[4][1] =  0.5;
        process_coords[4][2] = -0.5;

        process_coords[5][0] = -0.5;
        process_coords[5][1] = -0.5;
        process_coords[5][2] =  0.5;

        process_coords[6][0] = -0.5;
        process_coords[6][1] =  0.5;
        process_coords[6][2] = -0.5;

        process_coords[7][0] =  0.5;
        process_coords[7][1] = -0.5;
        process_coords[7][2] = -0.5;

        process_coords[8][0] = -0.5;
        process_coords[8][1] = -0.5;
        process_coords[8][2] = -0.5;

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This process should match all except the first the even numbered indices.
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(0)) );
        CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(1)) );

        for (int i = 2; i < 27; i += 2)
        {
            CPPUNIT_ASSERT(  whateverMatch(process.matchList(), config.matchList(i)) );
            CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(i+1)) );
        }
    }

    // ---------------------------------------------------------------------
    // Test an even larger local environment.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1;
        elements1.push_back("A");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("B");
        elements1.push_back("C");
        elements1.push_back("C");
        elements1.push_back("C");
        elements1.push_back("C");
        elements1.push_back("C");
        elements1.push_back("C");

        std::vector<std::string> elements2;
        elements2.push_back("F");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("B");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");
        elements2.push_back("A");

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords(15,std::vector<double>(3,0.0));
        process_coords[1][0] =  0.5;
        process_coords[1][1] =  0.5;
        process_coords[1][2] =  0.5;

        process_coords[2][0] = -0.5;
        process_coords[2][1] =  0.5;
        process_coords[2][2] =  0.5;

        process_coords[3][0] =  0.5;
        process_coords[3][1] = -0.5;
        process_coords[3][2] =  0.5;

        process_coords[4][0] =  0.5;
        process_coords[4][1] =  0.5;
        process_coords[4][2] = -0.5;

        process_coords[5][0] = -0.5;
        process_coords[5][1] = -0.5;
        process_coords[5][2] =  0.5;

        process_coords[6][0] = -0.5;
        process_coords[6][1] =  0.5;
        process_coords[6][2] = -0.5;

        process_coords[7][0] =  0.5;
        process_coords[7][1] = -0.5;
        process_coords[7][2] = -0.5;

        process_coords[8][0] = -0.5;
        process_coords[8][1] = -0.5;
        process_coords[8][2] = -0.5;

        process_coords[9][0] =  1.0;
        process_coords[9][1] =  0.0;
        process_coords[9][2] =  0.0;

        process_coords[10][0] =  0.0;
        process_coords[10][1] =  1.0;
        process_coords[10][2] =  0.0;

        process_coords[11][0] =  0.0;
        process_coords[11][1] =  0.0;
        process_coords[11][2] =  1.0;

        process_coords[12][0] = -1.0;
        process_coords[12][1] =  0.0;
        process_coords[12][2] =  0.0;

        process_coords[13][0] =  0.0;
        process_coords[13][1] = -1.0;
        process_coords[13][2] =  0.0;

        process_coords[14][0] =  0.0;
        process_coords[14][1] =  0.0;
        process_coords[14][2] = -1.0;


        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        Process process(config1, config2, rate, basis_sites);

        // This process should only match the first index.
        CPPUNIT_ASSERT(  whateverMatch(process.matchList(), config.matchList(0)) );

        for (int i = 1; i < 27; ++i)
        {
            CPPUNIT_ASSERT( !whateverMatch(process.matchList(), config.matchList(i)) );
        }
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testCalculateMatchingProcess()
{
    // {{{
    // Construct.
    Matcher m;

    // ---------------------------------------------------------------------
    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("C");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
            }
        }
    }

    // Modify the elements such that the central cell has an A in center.
    elements[0] = "A";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // Calculate the match lists.
    config.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Setup a process that matches all even indexed sites except the first.
    std::vector<std::string> elements1;
    elements1.push_back("C");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    elements1.push_back("B");
    std::vector<std::string> elements2;
    elements2.push_back("D");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    elements2.push_back("B");
    // Setup coordinates.
    std::vector<std::vector<double> > process_coords(9,std::vector<double>(3,0.0));
    process_coords[1][0] =  0.5;
    process_coords[1][1] =  0.5;
    process_coords[1][2] =  0.5;

    process_coords[2][0] = -0.5;
    process_coords[2][1] =  0.5;
    process_coords[2][2] =  0.5;

    process_coords[3][0] =  0.5;
    process_coords[3][1] = -0.5;
    process_coords[3][2] =  0.5;

    process_coords[4][0] =  0.5;
    process_coords[4][1] =  0.5;
    process_coords[4][2] = -0.5;

    process_coords[5][0] = -0.5;
    process_coords[5][1] = -0.5;
    process_coords[5][2] =  0.5;

    process_coords[6][0] = -0.5;
    process_coords[6][1] =  0.5;
    process_coords[6][2] = -0.5;

    process_coords[7][0] =  0.5;
    process_coords[7][1] = -0.5;
    process_coords[7][2] = -0.5;

    process_coords[8][0] = -0.5;
    process_coords[8][1] = -0.5;
    process_coords[8][2] = -0.5;

    // The configurations.
    const Configuration config1(process_coords, elements1, possible_types);
    const Configuration config2(process_coords, elements2, possible_types);

    // Construct the process.
    const double rate = 13.7;

    Process process(config1, config2, rate, basis_sites);

    // Put the process in a vector.
    std::vector<Process> processes(1, process);

    // Create an interactions object.
    Interactions  interactions(processes, false);

    // Make sure the process' available sites is empty.
    CPPUNIT_ASSERT( interactions.processes()[0]->sites().empty());

    // Match the index and process.
    std::vector<std::pair<int,int> > match_pairs(1,std::pair<int,int>(0,0));

    std::vector<RemoveTask> rt;
    std::vector<RateTask>   ut;
    std::vector<RateTask>   at;

    // Call the matching function.
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    // Check that the process' available sites is still empty.
    CPPUNIT_ASSERT( interactions.processes()[0]->sites().empty());

    // Call the matching function again, now with another matching index.
    match_pairs = std::vector<std::pair<int,int> >(1, std::pair<int,int>(2,0));
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    // Check that the index was added.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(interactions.processes()[0]->sites().size()), 1 );
    CPPUNIT_ASSERT( interactions.processes()[0]->isListed(2) );

    // Match again.
    rt = std::vector<RemoveTask>(0);
    ut = std::vector<RateTask>(0);
    at = std::vector<RateTask>(0);
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    // This should not have changed the availability of the index.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(interactions.processes()[0]->sites().size()), 1 );
    CPPUNIT_ASSERT( interactions.processes()[0]->isListed(2) );

    // Match another index.
    rt = std::vector<RemoveTask>(0);
    ut = std::vector<RateTask>(0);
    at = std::vector<RateTask>(0);
    match_pairs = std::vector<std::pair<int,int> >(1,std::pair<int,int>(4,0));
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    // This should not have changed the availability of the first index.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(interactions.processes()[0]->sites().size()), 2 );
    CPPUNIT_ASSERT( interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( interactions.processes()[0]->isListed(4) );

    // Change the configuration and match.
    elements[2] = "D";
    config = Configuration(coords, elements, possible_types);
    config.initMatchLists(lattice_map, 1);

    // Match.
    rt = std::vector<RemoveTask>(0);
    ut = std::vector<RateTask>(0);
    at = std::vector<RateTask>(0);
    match_pairs = std::vector<std::pair<int,int> >(1,std::pair<int,int>(2,0));
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    // Now index 2 should be removed but 4 still there.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(interactions.processes()[0]->sites().size()), 1 );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );

    // Match again - this should not change any thing.
    rt = std::vector<RemoveTask>(0);
    ut = std::vector<RateTask>(0);
    at = std::vector<RateTask>(0);
    match_pairs = std::vector<std::pair<int,int> >(1,std::pair<int,int>(2,0));
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    CPPUNIT_ASSERT_EQUAL( static_cast<int>(interactions.processes()[0]->sites().size()), 1 );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );

    // Match against a changed configuration. This removes index 4.
    elements[4] = "D";
    config = Configuration(coords, elements, possible_types);
    config.initMatchLists(lattice_map, 1);

    // Match.
    rt = std::vector<RemoveTask>(0);
    ut = std::vector<RateTask>(0);
    at = std::vector<RateTask>(0);
    match_pairs = std::vector<std::pair<int,int> >(1,std::pair<int,int>(4,0));
    m.matchIndicesWithProcesses(match_pairs, interactions, config, rt, ut, at);
    m.updateProcesses(rt, ut, at, interactions);

    CPPUNIT_ASSERT( interactions.processes()[0]->sites().empty() );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(4) );

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testUpdateProcesses()
{
    // {{{
    Matcher m;

    // Setup a list of processes and give it to an interactions object.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["C"] = 1;
    possible_types["B"] = 2;
    possible_types["D"] = 3;

    std::vector<std::string> elements1;
    elements1.push_back("C");
    elements1.push_back("B");
    std::vector<std::string> elements2;
    elements2.push_back("D");
    elements2.push_back("B");
    std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
    process_coords[1][0] =  0.5;
    process_coords[1][1] =  0.5;
    process_coords[1][2] =  0.5;

    const Configuration config1(process_coords, elements1, possible_types);
    const Configuration config2(process_coords, elements2, possible_types);
    const double rate = 13.7;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    {
        CustomRateProcess process(config1, config2, rate, basis_sites, 1.0);

        // Set up the interactions.
        std::vector<CustomRateProcess> processes(4, process);
        Interactions interactions(processes, false, RateCalculator());

        // Populate the processes with indices.
        interactions.processes()[0]->addSite(0, 1.1);
        interactions.processes()[0]->addSite(1, 2.2);
        interactions.processes()[0]->addSite(2, 3.3);

        interactions.processes()[1]->addSite(0, 11.0);
        interactions.processes()[1]->addSite(1, 1.0);

        interactions.processes()[2]->addSite(0, 13.7);

        // Setup a couple of valid add, remove and update tasks.
        std::vector<RemoveTask> rt;
        std::vector<RateTask> at;
        std::vector<RateTask> ut;

        RemoveTask rt1;
        rt1.index   = 1;
        rt1.process = 0;
        rt.push_back(rt1);

        RemoveTask rt2;
        rt2.index   = 0;
        rt2.process = 1;
        rt.push_back(rt2);

        RateTask at1;
        at1.index   = 4;
        at1.process = 2;
        at1.rate    = 123.456;
        at.push_back(at1);

        RateTask ut1;
        ut1.index   = 0;
        ut1.process = 2;
        ut1.rate    = 99.0;
        ut.push_back(ut1);

        // Perform the tasks.
        m.updateProcesses(rt, ut, at, interactions);

        // Check the result of the tasks.
        CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(0) );
        CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(1) );
        CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
        CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(0) );
        CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(1) );
        CPPUNIT_ASSERT(  interactions.processes()[2]->isListed(0) );
        CPPUNIT_ASSERT(  interactions.processes()[2]->isListed(4) );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( interactions.processes()[2]->totalRate(), 123.456 + 99.0, 1.0e-12 );

    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testCalculateMatchingInteractions()
{
    // {{{
    // Construct.
    Matcher m;

    // Setup the processes to test with.
    std::vector<Process> processes;

    // ---------------------------------------------------------------------
    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("C");
                site_types.push_back("M");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
                site_types.push_back("N");
            }
        }
    }

    // Modify the elements such that the central cell has an A in center.
    elements[0] = "A";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // Calculate the configuration match lists.
    config.initMatchLists(lattice_map, 1);

    // Calculate the site match lists.
    sitesmap.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Now the processes.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "C", "B", "B", "B", "B", "B", "B", "B", "B"
        };
        std::vector<std::string> elements2 = {
            "D", "B", "B", "B", "B", "B", "B", "B", "B"
        };

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const Process process(config1, config2, rate, basis_sites);
        processes.push_back(process);

    }

    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "A", "B", "B", "B", "B", "B", "B", "B", "B",
            "C", "C", "C", "C", "C", "C"
        };
        std::vector<std::string> elements2 = {
            "F", "B", "B", "B", "B", "B", "B", "B", "B",
            "A", "A", "A", "A", "A", "A"
        };

        // Setup coordinates.
        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
            { 1.0,  0.0,  0.0}, { 0.0,  1.0,  0.0}, { 0.0,  0.0,  1.0},
            {-1.0,  0.0,  0.0}, { 0.0, -1.0,  0.0}, { 0.0,  0.0, -1.0},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const Process process(config1, config2, rate, basis_sites);
        processes.push_back(process);
    }

    // ---------------------------------------------------------------------

    // Setup the interactions object.
    Interactions interactions(processes, true);

    // Setup a list of indices to test. Their order should not matter.
    std::vector<int> indices = {0, 2, 4, 3, 14, 9};

    // Call the matching function and check that all processes have been
    // updated correctly.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    // The first process should match all even indices except the first.
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );

    // The second process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(5) );

    // Call the matching function again does not change any thing.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(5) );

    // Change the configuration such that the first proces
    // match with the first (0:th) index, and the second process
    // does not match any.
    elements[0] = "C";
    config = Configuration(coords, elements, possible_types);
    config.initMatchLists(lattice_map, 1);
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );
    CPPUNIT_ASSERT( interactions.processes()[1]->sites().empty() );

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testCalculateMatchingInteractionsWithSiteTypes()
{
    // {{{
    // Construct.
    Matcher m;

    // Setup the processes to test with.
    std::vector<Process> processes;

    // ---------------------------------------------------------------------
    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("C");
                site_types.push_back("M");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
                site_types.push_back("N");
            }
        }
    }

    // Modify the elements such that the central cell has an A in center.
    elements[0] = "A";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;
    possible_site_types["K"] = 3;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // Calculate the configuration match lists.
    config.initMatchLists(lattice_map, 1);

    // Calculate the site match lists.
    sitesmap.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Now the processes.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "C", "B", "B", "B", "B", "B", "B", "B", "B"
        };
        std::vector<std::string> elements2 = {
            "D", "B", "B", "B", "B", "B", "B", "B", "B"
        };

        // Setup the site types.
        std::vector<int> site_types = {
            1, 2, 2, 2, 2, 2, 2, 2, 2
            //"M", "N", "N", "N", "N", "N", "N", "N", "N"
        };

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const std::vector<int> move_origins(0);
        const std::vector<Coordinate> move_vectors(0);
        const int process_number = 0;
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, site_types);
        processes.push_back(process);

    }

    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "A", "B", "B", "B", "B", "B", "B", "B", "B",
            "C", "C", "C", "C", "C", "C"
        };
        std::vector<std::string> elements2 = {
            "F", "B", "B", "B", "B", "B", "B", "B", "B",
            "A", "A", "A", "A", "A", "A"
        };

        // Setup the site types.
        const std::vector<int> site_types = {
            1, 2, 2, 2, 2, 2, 2, 2, 2,
            1, 1, 1, 1, 1, 1
        };

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
            { 1.0,  0.0,  0.0}, { 0.0,  1.0,  0.0}, { 0.0,  0.0,  1.0},
            {-1.0,  0.0,  0.0}, { 0.0, -1.0,  0.0}, { 0.0,  0.0, -1.0},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const std::vector<int> move_origins(0);
        const std::vector<Coordinate> move_vectors(0);
        const int process_number = 0;
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, site_types);
        processes.push_back(process);
    }

    // ---------------------------------------------------------------------

    // Setup the interactions object.
    Interactions interactions(processes, true);

    // Setup a list of indices to test. Their order should not matter.
    std::vector<int> indices = {0, 2, 4, 3, 14, 9};

    // Call the matching function and check that all processes have been
    // updated correctly.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    // The first process should match all even indices except the first.
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );

    // The second process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(5) );

    // Call the matching function again does not change any thing.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(5) );

    // Change the configuration such that the first proces
    // match with the first (0:th) index, and the second process
    // does not match any.
    elements[0] = "C";
    config = Configuration(coords, elements, possible_types);
    config.initMatchLists(lattice_map, 1);
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(14) );
    CPPUNIT_ASSERT( interactions.processes()[1]->sites().empty() );

    // }}}
}


// -------------------------------------------------------------------------- //
// Use different site type from above test functin.
//
void Test_Matcher::testCalculateMatchingInteractionsWithSiteTypes2()
{
    // {{{
    // Construct.
    Matcher m;

    // Setup the processes to test with.
    std::vector<Process> processes;

    // ---------------------------------------------------------------------
    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                std::vector<double> coord = basis[0];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("C");
                site_types.push_back("M");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                elements.push_back("B");
                site_types.push_back("N");
            }
        }
    }

    // Modify the elements such that the central cell has an A in center.
    elements[0] = "A";

    // Modify the first site type.
    site_types[0] = "K";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;
    possible_site_types["K"] = 3;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // Calculate the configuration match lists.
    config.initMatchLists(lattice_map, 1);

    // Calculate the site match lists.
    sitesmap.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Now the processes.
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "A", "B", "B", "B", "B", "B", "B", "B", "B"
        };
        std::vector<std::string> elements2 = {
            "D", "B", "B", "B", "B", "B", "B", "B", "B"
        };

        // Setup the site types.
        std::vector<int> site_types = {
            3, 2, 2, 2, 2, 2, 2, 2, 2
            //"M", "N", "N", "N", "N", "N", "N", "N", "N"
        };

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const std::vector<int> move_origins(0);
        const std::vector<Coordinate> move_vectors(0);
        const int process_number = 0;
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, site_types);
        processes.push_back(process);

    }

    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {
            "A", "B", "B", "B", "B", "B", "B", "B", "B",
            "C", "C", "C", "C", "C", "C"
        };
        std::vector<std::string> elements2 = {
            "F", "B", "B", "B", "B", "B", "B", "B", "B",
            "A", "A", "A", "A", "A", "A"
        };

        // Setup the site types.
        const std::vector<int> site_types1 = {
            3, 2, 2, 2, 2, 2, 2, 2, 2,
            1, 1, 1, 1, 1, 1
        };

        const std::vector<int> site_types2 = {
            1, 2, 2, 2, 2, 2, 2, 2, 2,
            1, 1, 1, 1, 1, 1
        };

        // Setup coordinates.
        std::vector<std::vector<double> > process_coords = {
            { 0.0,  0.0,  0.0}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
            { 0.5, -0.5,  0.5}, { 0.5,  0.5, -0.5}, {-0.5, -0.5,  0.5},
            {-0.5,  0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
            { 1.0,  0.0,  0.0}, { 0.0,  1.0,  0.0}, { 0.0,  0.0,  1.0},
            {-1.0,  0.0,  0.0}, { 0.0, -1.0,  0.0}, { 0.0,  0.0, -1.0},
        };

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const double rate = 13.7;
        const std::vector<int> move_origins(0);
        const std::vector<Coordinate> move_vectors(0);
        const int process_number = 0;
        const Process process1(config1, config2, rate, basis_sites, move_origins,
                               move_vectors, process_number, site_types1);
        const Process process2(config1, config2, rate, basis_sites, move_origins,
                               move_vectors, process_number, site_types2);
        processes.push_back(process1);
        processes.push_back(process2);
    }

    // ---------------------------------------------------------------------

    // Setup the interactions object.
    Interactions interactions(processes, true);

    // Setup a list of indices to test. Their order should not matter.
    std::vector<int> indices = {0, 2, 4, 3, 14, 9};

    // Call the matching function and check that all processes have been
    // updated correctly.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    // The first process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(14) );

    // The second process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(14) );

    // The third process matches no index.
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(14) );

    // Call the matching function again does not change any thing.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    // The first process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[0]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[0]->isListed(14) );

    // The second process should match the first index only.
    CPPUNIT_ASSERT(  interactions.processes()[1]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[1]->isListed(14) );

    // The third process matches no index.
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(0) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(2) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(3) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(4) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(9) );
    CPPUNIT_ASSERT( !interactions.processes()[2]->isListed(14) );

    // }}}
}


// -------------------------------------------------------------------------- //
// Test matching calculation in 2D grid.
void Test_Matcher::testCalculateMatchingInteractionsWithSiteTypes2D()
{
    // {{{
    Matcher m;

    // Setup processes to test with.
    std::vector<Process> processes;

    // ------------------------------------------------------------
    // 10x10 2D grid.
    std::vector<std::vector<double> > basis = {{0.0, 0.0, 0.0}};
    std::vector<int> basis_sites = {0};

    // Setup elements.
    std::vector<std::string> elements = {
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "A", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B"
    };

    // Setup site types.
    std::vector<std::string> site_types = {
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "a", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b",
        "b", "b", "b", "b", "b", "b", "b", "b", "b", "b" 
    };

    // Setup coordinates.
    std::vector<std::vector<double> > coords;
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            std::vector<double> coord = basis[0];
            coord[0] += i;
            coord[1] += j;
            coords.push_back(coord);
        }
    }

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["a"] = 1;
    possible_site_types["b"] = 2;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions = {10, 10, 1};
    const std::vector<bool> periodicity = {true, true, false};
    const int basis_size = 1;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // ---------------------------------------------------------------------
    // Now the processes.

    // Site types for all processes.
    std::vector<int> process_site_types = {1, 2, 2, 2, 2, 2};

    // Coordinates for all processes.
    std::vector<std::vector<double> > process_coords = {
        {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}, {2.0, 2.0, 0.0}
    };

    // Other parameters for all processes.
    const double rate = 13.7;
    const std::vector<int> move_origins(0);
    const std::vector<Coordinate> move_vectors(0);
    const int process_number = 0;

    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {"A", "B", "B", "B", "B", "A"};
        std::vector<std::string> elements2 = {"B", "B", "A", "B", "B", "A"};

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, process_site_types);
        processes.push_back(process);
    }
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {"A", "B", "B", "B", "B", "B"};
        std::vector<std::string> elements2 = {"B", "B", "A", "B", "B", "B"};

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, process_site_types);
        processes.push_back(process);
    }
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {"A", "B", "B", "B", "B", "B"};
        std::vector<std::string> elements2 = {"B", "B", "B", "A", "B", "B"};

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, process_site_types);
        processes.push_back(process);
    }
    {
        // Setup the two configurations.
        std::vector<std::string> elements1 = {"A", "B", "B", "B", "B", "B"};
        std::vector<std::string> elements2 = {"B", "B", "B", "B", "A", "B"};

        // The configurations.
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        // Construct the process.
        const Process process(config1, config2, rate, basis_sites, move_origins,
                              move_vectors, process_number, process_site_types);
        processes.push_back(process);
    }

    // Setup the interactions object.
    Interactions interactions(processes, true);

    // Initialize the match lists.
    config.initMatchLists(lattice_map, interactions.maxRange());
    sitesmap.initMatchLists(lattice_map, interactions.maxRange());

    interactions.updateProcessMatchLists(config, lattice_map);

    // Indices to test.
    std::vector<int> indices;
    for (int i = 0; i < 100; ++i)
    {
        indices.push_back(i);
    }

    // Call the matching function.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT(interactions.processes()[1]->isListed(37));
    CPPUNIT_ASSERT(interactions.processes()[2]->isListed(37));
    CPPUNIT_ASSERT(interactions.processes()[3]->isListed(37));

    CPPUNIT_ASSERT_EQUAL(interactions.totalAvailableSites(), 3);

    // Call the matching function again, nothing change.
    m.calculateMatching(interactions, config, sitesmap, lattice_map, indices);

    CPPUNIT_ASSERT(interactions.processes()[1]->isListed(37));
    CPPUNIT_ASSERT(interactions.processes()[2]->isListed(37));
    CPPUNIT_ASSERT(interactions.processes()[3]->isListed(37));

    CPPUNIT_ASSERT_EQUAL(interactions.totalAvailableSites(), 3);
    // }}}
}


// -------------------------------------------------------------------------- //
// This proxy class is part needed for the UpdateRates test below.
class CustRateCalc : public RateCalculator {
public:
    virtual ~CustRateCalc() {}
    virtual double backendRateCallback(const std::vector<double> geometry,
                                       const int len,
                                       const std::vector<std::string> & types_before,
                                       const std::vector<std::string> & types_after,
                                       const double rate_constant,
                                       const int process_number,
                                       const double global_x,
                                       const double global_y,
                                       const double global_z) const
        {
            // Return.
            return std::sqrt(rate_constant);
        }
};

// -------------------------------------------------------------------------- //
//
void Test_Matcher::testUpdateRates()
{
    // {{{
    // Generate a list of tasks to run.
    std::vector<RateTask> tasks;

    seedRandom(3452321, true);
    const double ref_rate1 = randomDouble01();
    const double ref_rate2 = randomDouble01();

    RateTask t1;
    t1.index   = 0;
    t1.process = 0;
    t1.rate    = 0.0;
    tasks.push_back(t1);

    RateTask t2;
    t2.index   = 1;
    t2.process = 3;
    t2.rate    = 0.0;
    tasks.push_back(t2);

    // Here is the matcher to test.
    Matcher m;

    // Setup a list of processes and give it to an interactions object.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["C"] = 1;
    possible_types["B"] = 2;
    possible_types["D"] = 3;
    possible_types["A"] = 4;

    std::vector<std::string> elements1;
    elements1.push_back("C");
    elements1.push_back("B");
    std::vector<std::string> elements2;
    elements2.push_back("D");
    elements2.push_back("B");
    std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
    process_coords[1][0] =  0.5;
    process_coords[1][1] =  0.5;
    process_coords[1][2] =  0.5;

    const Configuration config1(process_coords, elements1, possible_types);
    const Configuration config2(process_coords, elements2, possible_types);

    // Here is the process.
    std::vector<int> basis_sites;
    basis_sites.push_back(0);
    CustomRateProcess process1(config1, config2, ref_rate1, basis_sites, 1.0);
    CustomRateProcess process2(config1, config2, ref_rate2, basis_sites, 1.0);

    // Set up the interactions object.
    std::vector<CustomRateProcess> processes(4, process1);
    processes[3] = process2;
    CustRateCalc rate_calculator;
    Interactions interactions(processes, false, rate_calculator );

    // Setup a valid configuration.
    std::vector<std::vector<double> > coords(2, std::vector<double>(3, 0.0));

    // One cell with two atoms.
    coords[0][0] = 0.0;
    coords[0][1] = 0.0;
    coords[0][2] = 0.0;
    coords[1][0] = 0.5;
    coords[1][1] = 0.3;
    coords[1][2] = 0.1;

    // Setup elements.
    std::vector<std::string> elements(2);
    elements[0] = "A";
    elements[1] = "B";

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Setup a cooresponding lattice map.
    const std::vector<int> repetitions(3, 1);
    const std::vector<bool> periodicity(3, false);
    const int basis = 2;

    LatticeMap lattice_map(basis, repetitions, periodicity);
    config.initMatchLists(lattice_map, 1);

    // Send the interactions object down for update
    // together with the processes and a configuration.
    std::vector<double> rates(tasks.size(), 0.0);
    m.updateRates(rates, tasks, interactions, config);

    // Check that the rates were correctly updated.
    CPPUNIT_ASSERT_DOUBLES_EQUAL( rates[0], std::sqrt(ref_rate1), 1.0e-12 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( rates[1], std::sqrt(ref_rate2), 1.0e-12 );

    // }}}
}


// -------------------------------------------------------------------------- //
// This proxy class is part of the UpdateSingleRate test below.
class CustomRateCalculator : public RateCalculator {
public:
    // {{{
    CustomRateCalculator() {}
    virtual ~CustomRateCalculator() {}
    virtual double backendRateCallback(const std::vector<double> geometry,
                                       const int len,
                                       const std::vector<std::string> & types_before,
                                       const std::vector<std::string> & types_after,
                                       const double rate_constant,
                                       const int process_number,
                                       const double global_x,
                                       const double global_y,
                                       const double global_z) const
    {
        // Test the geometry.
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[0], 0.0, 1.0e-12 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[1], 0.0, 1.0e-12 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[2], 0.0, 1.0e-12 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[3],-0.5, 1.0e-12 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[4],-0.3, 1.0e-12 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( geometry[5],-0.1, 1.0e-12 );

        // Test the length.
        CPPUNIT_ASSERT_EQUAL(len, 2);

        // Test the types before.
        CPPUNIT_ASSERT_EQUAL(static_cast<int>(types_before.size()), 2);
        CPPUNIT_ASSERT_EQUAL(std::string("B"), types_before[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("A"), types_before[1]);

        // Test the types after.
        CPPUNIT_ASSERT_EQUAL(static_cast<int>(types_after.size()), 2);
        CPPUNIT_ASSERT_EQUAL(std::string("C"), types_after[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("A"), types_after[1]);

        // Test the process number.
        CPPUNIT_ASSERT_EQUAL( process_number, 917 );

        // Return.
        return std::pow(rate_constant,3.14159);
    }
    // }}}
};

// -------------------------------------------------------------------------- //
//
void Test_Matcher::testUpdateSingleRate()
{
    // {{{
    // Setup a valid configuration.
    std::vector<std::vector<double> > coords(2, std::vector<double>(3, 0.0));

    // One cell with two atoms.
    coords[0][0] = 0.0;
    coords[0][1] = 0.0;
    coords[0][2] = 0.0;
    coords[1][0] = 0.5;
    coords[1][1] = 0.3;
    coords[1][2] = 0.1;

    // Setup elements.
    std::vector<std::string> elements(2);
    elements[0] = "A";
    elements[1] = "B";

    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["C"] = 3;
    possible_types["D"] = 4;
    possible_types["E"] = 5;
    possible_types["F"] = 6;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Setup a non periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 1);
    const std::vector<bool> periodicity(3, false);
    const int basis = 2;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

    LatticeMap lattice_map(basis, repetitions, periodicity);
    config.initMatchLists(lattice_map, 13);

    // Construct a process that should match the second index.

    // Setup the two configurations.
    std::vector<std::string> elements1;
    elements1.push_back("B");
    elements1.push_back("A");
    std::vector<std::string> elements2;
    elements2.push_back("C");
    elements2.push_back("A");
    // Setup coordinates.
    std::vector<std::vector<double> > process_coords(2,std::vector<double>(3,0.0));
    process_coords[1][0] =  -0.5;
    process_coords[1][1] =  -0.5;
    process_coords[1][2] =  -0.5;
    // The configurations.
    const Configuration config1(process_coords, elements1, possible_types);
    const Configuration config2(process_coords, elements2, possible_types);

    // Construct the process with a random rate.
    seedRandom(19, true);
    const double rate = 13.7*randomDouble01();
    CustomRateProcess process(config1, config2, rate, basis_sites, 12.0, std::vector<int>(0), std::vector<Coordinate>(0), 917);

    // Put the process in a vector.
    std::vector<CustomRateProcess> processes(1, process);

    // Create an interactions object.
    Interactions interactions(processes, false, CustomRateCalculator());

    // Get the matcher to test and a custom rate calculator.
    const Matcher m;
    const CustomRateCalculator crc;
    const int index = 1;
    const double ret_rate = m.updateSingleRate(index, *interactions.processes()[0], config, crc);

    // Test against the known reference.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ret_rate, std::pow(rate, 3.14159), 1.0e-12);

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_Matcher::testClassifyConfiguration()
{
    // {{{

    Matcher matcher;

    // Construct a lattice with 27 cells with 2 basis sites in one cell.
    std::vector<std::vector<double> > basis = {{0.0, 0.0, 0.0},
                                               {0.5, 0.5, 0.5}};

    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;

    // Initialize element types and site types.
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                for (int b = 0; b < 2; ++b)
                {
                    std::vector<double> coord = basis[b];
                    coord[0] += i;
                    coord[1] += j;
                    coord[2] += k;
                    coords.push_back(coord);
                    elements.push_back("V");
                    site_types.push_back("P");
                }
            }
        }
    }
    // Change some elements.
    elements[0]  = "A";
    elements[1]  = "B";
    elements[2]  = "A";
    elements[3]  = "B";
    elements[18] = "B";
    elements[36] = "A";

    // Setup the mapping from element to interger.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Default fast species.
    const std::vector<std::string> fast_elements = {"V"};

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["P"] = 1;

    // Construct the configuration.
    Configuration config(coords, elements, possible_types);

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ------------------------------------------------------------------
    // Construct a lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;
    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // ------------------------------------------------------------------
    // Setup processes to test with.
    std::vector<Process> processes;

    // Rate for all processes.
    const double rate = 1.0;

    // A diffusion upwards at basis 0.
    {
        std::vector<std::string> elements1 = {"A", "V"};
        std::vector<std::string> elements2 = {"V", "A"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.0, 0.0, 1.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, true);
        processes.push_back(process);
    }
    // A diffusion upwards at basis 1.
    {
        std::vector<std::string> elements1 = {"A", "V"};
        std::vector<std::string> elements2 = {"V", "A"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.0, 0.0, 1.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {1}, true);
        processes.push_back(process);
    }
    // B diffusion upwards at basis 0.
    {
        std::vector<std::string> elements1 = {"B", "V"};
        std::vector<std::string> elements2 = {"V", "B"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.0, 0.0, 1.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, true);
        processes.push_back(process);
    }
    // B diffusion upwards at basis 1.
    {
        std::vector<std::string> elements1 = {"B", "V"};
        std::vector<std::string> elements2 = {"V", "B"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.0, 0.0, 1.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {1}, true);
        processes.push_back(process);
    }
    // A + B.
    {
        std::vector<std::string> elements1 = {"A", "B"};
        std::vector<std::string> elements2 = {"V", "V"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.5, 0.5, 0.5}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, false);
        processes.push_back(process);
    }

    // ------------------------------------------------------------------
    // Create an interaction object.
    Interactions interactions(processes, true);

    // Initialize configuration & sitemap match lists.
    config.initMatchLists(lattice_map, interactions.maxRange());
    sitesmap.initMatchLists(lattice_map, interactions.maxRange());

    interactions.updateProcessMatchLists(config, lattice_map);

    // Match all centers.
    std::vector<int> indices;
    for (size_t i = 0; i < config.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher.calculateMatching(interactions, config, sitesmap,
                              lattice_map, indices);

    // Check configuration fast flags before classification.
    const std::vector<bool> & slow_flags = config.slowFlags();
    for (const bool slow_flag : slow_flags)
    {
        CPPUNIT_ASSERT(slow_flag);
    }

    // Classify configuration.
    matcher.classifyConfiguration(interactions,
                                  config,
                                  sitesmap,
                                  lattice_map,
                                  indices,
                                  fast_elements);
    const std::vector<bool> & classified_slow_flags = config.slowFlags();

    // Check classification result.
    for (size_t i = 0; i < classified_slow_flags.size(); ++i)
    {
        if (i == 0 or i == 1)
        {
            CPPUNIT_ASSERT(classified_slow_flags[i]);
        }
        else
        {
            CPPUNIT_ASSERT(!classified_slow_flags[i]);
        }
    }

    // }}}
}

