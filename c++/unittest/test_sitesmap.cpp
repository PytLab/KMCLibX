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
#include "process.h"
#include "configuration.h"
#include "matchlist.h"
#include "latticemap.h"

// -------------------------------------------------------------------------//
//
void Test_SitesMap::testConstructionAndQuery()
{
    // {{{

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

    // }}}
}


// -------------------------------------------------------------------------//
//
void Test_SitesMap::testMatchList()
{
    // {{{

    // Setup a realistic system.
    std::vector< std::vector<double> > basis(3, std::vector<double>(3,0.0));
    basis[1][0] = 0.25;
    basis[1][1] = 0.25;
    basis[1][2] = 0.25;
    basis[2][0] = 0.75;
    basis[2][1] = 0.75;
    basis[2][2] = 0.75;

    std::vector<int> basis_sites(3);
    basis_sites[0] = 0;
    basis_sites[1] = 1;
    basis_sites[2] = 2;
    std::vector<std::string> basis_type(3);
    basis_type[0] = "A";
    basis_type[1] = "B";
    basis_type[2] = "B";

    // Make a 10x10x10 structure.
    const int nI = 4;
    const int nJ = 4;
    const int nK = 4;
    const int nB = 3;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> site_types;

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                for (int b = 0; b < nB; ++b)
                {
                    std::vector<double> c(3);
                    c[0] = i + basis[b][0];
                    c[1] = j + basis[b][1];
                    c[2] = k + basis[b][2];
                    coordinates.push_back(c);
                    site_types.push_back(basis_type[b]);
                }
            }
        }
    }

    site_types[128] = "V";


    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Setup the sitesmap.
    SitesMap sitesmap(coordinates, site_types, possible_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity(3, true);
    LatticeMap lattice_map(nB, repetitions, periodicity);

    // Use the (1, 1, 1) site as the center.
    const int center = lattice_map.indicesFromCell(1, 1, 1)[0];

    // Get the neighbor indices of the center, shell = 1.
    std::vector<int> neighbour_indices = lattice_map.neighbourIndices(center, 1);

    // Get match list wrt. the center site.
    const SiteMatchList match_list = sitesmap.matchList(center,
                                                        neighbour_indices,
                                                        lattice_map);

    // Check the size of match list.
    CPPUNIT_ASSERT_EQUAL(match_list.size(), neighbour_indices.size());

    // Check content of certain match list entries.
    // The first entry.
    const SiteMatchListEntry se1 = match_list[0];
    CPPUNIT_ASSERT_EQUAL(se1.match_type, 1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(se1.distance, 0.0, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(se1.coordinate, Coordinate(0.0, 0.0, 0.0));
    CPPUNIT_ASSERT_EQUAL(se1.index, 63);

    // The second entry.
    const SiteMatchListEntry se2 = match_list[1];
    CPPUNIT_ASSERT_EQUAL(se2.match_type, 2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(se2.distance, 0.4330127018922193, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(se2.coordinate, Coordinate(-0.25, -0.25, -0.25));
    CPPUNIT_ASSERT_EQUAL(se2.index, 2);

    // The last entry.
    const SiteMatchListEntry se3 = match_list[80];
    CPPUNIT_ASSERT_EQUAL(se3.match_type, 3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(se3.distance, 3.031088913245535, 1.0e-12);
    CPPUNIT_ASSERT_EQUAL(se3.coordinate, Coordinate(1.75, 1.75, 1.75));
    CPPUNIT_ASSERT_EQUAL(se3.index, 128);
    // }}}
}


// -------------------------------------------------------------------------//
//
void Test_SitesMap::testInitMatchList()
{
    // {{{

    // Setup a realistic system.
    std::vector< std::vector<double> > basis(3, std::vector<double>(3,0.0));
    basis[1][0] = 0.25;
    basis[1][1] = 0.25;
    basis[1][2] = 0.25;
    basis[2][0] = 0.75;
    basis[2][1] = 0.75;
    basis[2][2] = 0.75;

    std::vector<int> basis_sites(3);
    basis_sites[0] = 0;
    basis_sites[1] = 1;
    basis_sites[2] = 2;
    std::vector<std::string> basis_type(3);
    basis_type[0] = "A";
    basis_type[1] = "B";
    basis_type[2] = "B";

    // Make a 10x10x10 structure.
    const int nI = 4;
    const int nJ = 4;
    const int nK = 4;
    const int nB = 3;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> site_types;

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                for (int b = 0; b < nB; ++b)
                {
                    std::vector<double> c(3);
                    c[0] = i + basis[b][0];
                    c[1] = j + basis[b][1];
                    c[2] = k + basis[b][2];
                    coordinates.push_back(c);
                    site_types.push_back(basis_type[b]);
                }
            }
        }
    }

    site_types[128] = "V";


    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Setup the sitesmap.
    SitesMap sitesmap(coordinates, site_types, possible_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity(3, true);
    LatticeMap lattice_map(nB, repetitions, periodicity);

    // Use the (1, 1, 1) site as the center.
    const int center = lattice_map.indicesFromCell(1, 1, 1)[0];

    // Initialize the match lists of the sitesmap.
    // Get the match list of the given index.
    sitesmap.initMatchLists(lattice_map, 1);
    const SiteMatchList ret_match_list = sitesmap.matchList(center);

    // Get the match list by initial filling.
    std::vector<int> neighbour_indices = lattice_map.neighbourIndices(center, 1);
    const SiteMatchList match_list = sitesmap.matchList(center,
                                                        neighbour_indices,
                                                        lattice_map);
    // Compare the two match list, expected to be the same.
    SiteMatchList::const_iterator it1 = ret_match_list.begin();
    SiteMatchList::const_iterator it2 = match_list.begin();
    const SiteMatchList::const_iterator end = ret_match_list.end();

    for ( ; it1 != end; ++it1, ++it2)
    {
        CPPUNIT_ASSERT_EQUAL(it1->index, it2->index);
        CPPUNIT_ASSERT_EQUAL(it1->coordinate, it2->coordinate);
        CPPUNIT_ASSERT_EQUAL(it1->match_type, it2->match_type);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(it1->distance, it2->distance, 1.0e-12);
    }

    // }}}
}


// -------------------------------------------------------------------------//
//
void Test_SitesMap::testMatchListMatching()
{
    // {{{
    // Construct a sites map object and initialize the match lists.
    // 27 cells cell with two atoms in the basis.
    std::vector<std::vector<double> > basis(2, std::vector<double>(3,0.0));
    basis[1][0] = 0.5;
    basis[1][1] = 0.5;
    basis[1][2] = 0.5;
    std::vector<int> basis_sites;
    basis_sites.push_back(1);
    basis_sites.push_back(0);

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
                site_types.push_back("M");

                coord = basis[1];
                coord[0] += i;
                coord[1] += j;
                coord[2] += k;
                coords.push_back(coord);
                site_types.push_back("N");
            }
        }
    }

    // Setup the mapping from site type to integer.
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

    // Construct the sitesmap.
    SitesMap sitesmap(coords, site_types, possible_site_types);

    // ---------------------------------------------------------------------
    // Setup a periodic cooresponding lattice map.
    const std::vector<int> repetitions(3, 3);
    const std::vector<bool> periodicity(3, true);
    const int basis_size = 2;

    LatticeMap lattice_map(basis_size, repetitions, periodicity);

    // Calculate the site match lists.
    sitesmap.initMatchLists(lattice_map, 1);

    // ---------------------------------------------------------------------
    // Now the processes.
    // Setup the two configurations.
    std::vector<std::string> elements1 = {
        "C", "B", "B", "B", "B", "B", "B", "B", "B"
    };
    std::vector<std::string> elements2 = {
        "D", "B", "B", "B", "B", "B", "B", "B", "B"
    };

    // Setup the site types.
    std::vector<int> process_site_types = {
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
                          move_vectors, process_number, process_site_types);

    // ---------------------------------------------------------------------
    // Check the site type in match list.
    const int even_site_types[9] = {1, 2, 2, 2, 2, 2, 2, 2, 2};
    const int odd_site_types[9] = {2, 1, 1, 1, 1, 1, 1, 1, 1};
    const SiteMatchList & even_match_list = sitesmap.matchList(26);
    const SiteMatchList & odd_match_list = sitesmap.matchList(1);
    for (int i = 0; i < 9; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(even_site_types[i], even_match_list[i].match_type);
        CPPUNIT_ASSERT_EQUAL(odd_site_types[i], odd_match_list[i].match_type);
    }

    // Check match list matching.
    const ProcessMatchList & process_match_list = process.matchList();
    CPPUNIT_ASSERT(whateverMatch(process_match_list, even_match_list));
    CPPUNIT_ASSERT(!whateverMatch(process_match_list, odd_match_list));
    
    // }}}
}

