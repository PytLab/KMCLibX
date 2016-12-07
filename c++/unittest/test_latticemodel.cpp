/*
  Copyright (c)  2012-2013  Mikael Leetmaa
  Copyright (c)  2016-2019  shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


// Include the test definition.
#include "test_latticemodel.h"

// Include the files to test.
#include "latticemodel.h"

// Other inclusions.
#include "configuration.h"
#include "latticemap.h"
#include "interactions.h"
#include "random.h"
#include "simulationtimer.h"
#include "matchlist.h"
#include "sitesmap.h"

#include <ctime>
#include <algorithm>

// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testConstruction()
{
    // {{{
    // Construct a configuration.
    std::vector<std::vector<double> > coords(2,std::vector<double>(3,0.0));
    coords[0][0] = 1.4;
    coords[0][1] = 2.5;
    coords[0][2] = 4.6;
    coords[1][0] = 5.7;
    coords[1][1] = 3.5;
    coords[1][2] = 2.1;

    std::vector<std::string> elements(2);
    elements[0] = "A";
    elements[1] = "V";

    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["V"] = 2;

    Configuration config(coords,elements,possible_types);

    // Construct a sitesmap.
    const std::vector<std::string> site_types = {"M", "N"};

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;

    SitesMap sitesmap(coords, site_types, possible_site_types);

    // And a corresponding lattice map.
    std::vector<int> rep(3, 1);
    rep[0] = 2;
    const std::vector<bool> per(3, true);
    LatticeMap lattice_map(1, rep, per);

    // We also need the interactions.
    const std::vector<Process> processes;
    Interactions interactions(processes, true);

    // And a timer.
    SimulationTimer timer;

    // construct.
    LatticeModel model(config, sitesmap, timer, lattice_map, interactions);

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testSetupAndQuery()
{
    // {{{
    // Setup a realistic system and check.
    std::vector< std::vector<double> > basis(3, std::vector<double>(3,0.0));
    basis[1][0] = 0.25;
    basis[1][1] = 0.25;
    basis[1][2] = 0.25;
    basis[2][0] = 0.75;
    basis[2][1] = 0.75;
    basis[2][2] = 0.75;

    std::vector<int> basis_sites;
    basis_sites.push_back(0);
    basis_sites.push_back(1);
    basis_sites.push_back(2);

    std::vector<std::string> basis_elements(3);
    basis_elements[0] = "A";
    basis_elements[1] = "B";
    basis_elements[2] = "B";

    const std::vector<std::string> basis_site_types = {"M", "N", "N"};

    // Make a 37x18x19 structure.
    const int nI = 37;
    const int nJ = 18;
    const int nK = 19;
    const int nB = 3;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> elements;
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
                    elements.push_back(basis_elements[b]);
                    site_types.push_back(basis_site_types[b]);
                }
            }
        }
    }

    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Possible site types.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;
    possible_site_types["K"] = 3;

    // Setup the configuration.
    Configuration configuration(coordinates, elements, possible_types);

    // Setup the sitesmap.
    SitesMap sitesmap(coordinates, site_types, possible_site_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity(3, true);
    LatticeMap lattice_map(nB, repetitions, periodicity);

    // Setup the interactions object.
    std::vector<Process> processes;

    // A process that independent of local environment swaps a "B" to "V"
    {
        const std::vector<std::string> process_elements1(1,"B");
        const std::vector<std::string> process_elements2(1,"V");
        const std::vector<std::vector<double> > process_coordinates(1, std::vector<double>(3, 0.0));
        const double rate = 1.234;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store twize.
        processes.push_back(p);
        processes.push_back(p);
    }

    // A process that finds an A between two B's in the 1,1,1 direction
    // and swap the A and the first B.
    {
        std::vector<std::string> process_elements1(3);
        process_elements1[0] = "A";
        process_elements1[1] = "B";
        process_elements1[2] = "B";

        std::vector<std::string> process_elements2(3);
        process_elements2[0] = "B";
        process_elements2[1] = "A";
        process_elements2[2] = "B";

        std::vector<std::vector<double> > process_coordinates(3, std::vector<double>(3, 0.0));

        process_coordinates[1][0] = -0.25;
        process_coordinates[1][1] = -0.25;
        process_coordinates[1][2] = -0.25;
        process_coordinates[2][0] =  0.25;
        process_coordinates[2][1] =  0.25;
        process_coordinates[2][2] =  0.25;

        const double rate = 13.7;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }

    // A process that finds a V between two A in the 1,1,1 direction
    // and turn the V into B.
    {
        std::vector<std::string> process_elements1(3);
        process_elements1[0] = "V";
        process_elements1[1] = "B";
        process_elements1[2] = "B";

        std::vector<std::string> process_elements2(3);
        process_elements2[0] = "B";
        process_elements2[1] = "A";
        process_elements2[2] = "B";

        std::vector<std::vector<double> > process_coordinates(3, std::vector<double>(3, 0.0));

        process_coordinates[1][0] = -0.25;
        process_coordinates[1][1] = -0.25;
        process_coordinates[1][2] = -0.25;
        process_coordinates[2][0] =  0.25;
        process_coordinates[2][1] =  0.25;
        process_coordinates[2][2] =  0.25;

        const double rate = 13.7;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }

    Interactions interactions(processes, false);

    // We need a timer.
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(configuration, sitesmap, timer, lattice_map, interactions);

    // Get the interactions out and check that they are setup as expected.
    const Interactions ret_interactions = lattice_model.interactions();
    const std::vector<Process*> ret_processes = ret_interactions.processes();

    // Check the number of processes.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_processes.size()), 4);

    // Check the number of listed sites at each process.
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions.processes()[0]->sites().size()), nI*nJ*nK*nB*2/3 );
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions.processes()[1]->sites().size()), nI*nJ*nK*nB*2/3 );
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions.processes()[2]->sites().size()), nI*nJ*nK*nB/3 );
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions.processes()[3]->sites().size()), 0 );

    // Check the first process.
    const Process & p0 = (*ret_processes[0]);

    // Check that all "B" sites are listed,
    // and that all "V" and "A" sites are not.
    for (int i = 0; i < nI*nJ*nK*nB; i += nB)
    {
        CPPUNIT_ASSERT(!p0.isListed(i)   );
        CPPUNIT_ASSERT( p0.isListed(i+1) );
        CPPUNIT_ASSERT( p0.isListed(i+2) );
    }

    // Check the second process.
    const Process & p1 = (*ret_processes[1]);

    // This process is identical to the first.
    for (int i = 0; i < nI*nJ*nK*nB; i += nB)
    {
        CPPUNIT_ASSERT(!p1.isListed(i)   );
        CPPUNIT_ASSERT( p1.isListed(i+1) );
        CPPUNIT_ASSERT( p1.isListed(i+2) );
    }

    // Check the third process has all "A" sites listed.
    const Process & p2 = (*ret_processes[2]);

    // This process is identical to the first.
    for (int i = 0; i < nI*nJ*nK*nB; i += nB)
    {
        CPPUNIT_ASSERT( p2.isListed(i)   );
        CPPUNIT_ASSERT(!p2.isListed(i+1) );
        CPPUNIT_ASSERT(!p2.isListed(i+2) );
    }

    // Check the last process has no sites listed.
    const Process & p3 = (*ret_processes[3]);
    CPPUNIT_ASSERT( p3.sites().empty() );

    // Introduce a few different typed sites.
    elements[0]    = "V";
    elements[216]  = "V";   // These affects process 0,1 and 3
    elements[1434] = "V";
    elements[2101] = "V";   // This affects process 0,1 and 2

    // Get a new configuration.
    configuration = Configuration(coordinates, elements, possible_types);

    // Get a new lattice model to test.
    LatticeModel lattice_model_2(configuration, sitesmap, timer, lattice_map, interactions);

    // Get the interactions out and check.
    const Interactions ret_interactions_2 = lattice_model_2.interactions();

    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions_2.processes()[0]->sites().size()), (nI*nJ*nK*nB*2/3)-1);
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions_2.processes()[1]->sites().size()), (nI*nJ*nK*nB*2/3)-1);
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions_2.processes()[2]->sites().size()), (nI*nJ*nK*nB/3)-4);
    CPPUNIT_ASSERT_EQUAL( static_cast<int>(ret_interactions_2.processes()[3]->sites().size()), 3 );

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testSingleStepFunction()
{
    // {{{
    // Setup a realistic system and check.
    std::vector< std::vector<double> > basis(3, std::vector<double>(3,0.0));
    basis[1][0] = 0.25;
    basis[1][1] = 0.25;
    basis[1][2] = 0.25;
    basis[2][0] = 0.75;
    basis[2][1] = 0.75;
    basis[2][2] = 0.75;

    std::vector<int> basis_sites;
    basis_sites.push_back(0);
    basis_sites.push_back(1);
    basis_sites.push_back(2);

    std::vector<std::string> basis_elements(3);
    basis_elements[0] = "A";
    basis_elements[1] = "B";
    basis_elements[2] = "B";

    const std::vector<std::string> basis_site_types = {"M", "N", "N"};

    // Make a 37x18x19 structure.
    const int nI = 37;
    const int nJ = 18;
    const int nK = 19;
    const int nB = 3;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> elements;
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
                    elements.push_back(basis_elements[b]);
                    site_types.push_back(basis_site_types[b]);
                }
            }
        }
    }

    // Introduce a few different typed sites.
    elements[0]    = "V";
    elements[216]  = "V";   // These affects process 0,1 and 3
    elements[1434] = "V";
    elements[2101] = "V";   // This affects process 0,1 and 2

    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Possible site types.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;
    possible_site_types["K"] = 3;

    // Setup the configuration.
    Configuration configuration(coordinates, elements, possible_types);

    // Setup the sitesmap.
    SitesMap sitesmap(coordinates, site_types, possible_site_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity(3, true);
    LatticeMap lattice_map(nB, repetitions, periodicity);
    configuration.initMatchLists(lattice_map, 1);

    // Setup the interactions object.
    std::vector<Process> processes;

    // A process that independent of local environment swaps a "B" to "V"
    {
        const std::vector<std::string> process_elements1(1,"B");
        const std::vector<std::string> process_elements2(1,"V");
        const std::vector<std::vector<double> > process_coordinates(1, std::vector<double>(3, 0.0));
        const double rate = 1.234;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store twize.
        processes.push_back(p);
        processes.push_back(p);
    }

    // A process that finds an A between two B's in the 1,1,1 direction
    // and swap the A and the first B.
    {
        std::vector<std::string> process_elements1(3);
        process_elements1[0] = "A";
        process_elements1[1] = "B";
        process_elements1[2] = "B";

        std::vector<std::string> process_elements2(3);
        process_elements2[0] = "B";
        process_elements2[1] = "A";
        process_elements2[2] = "B";

        std::vector<std::vector<double> > process_coordinates(3, std::vector<double>(3, 0.0));

        process_coordinates[1][0] = -0.25;
        process_coordinates[1][1] = -0.25;
        process_coordinates[1][2] = -0.25;
        process_coordinates[2][0] =  0.25;
        process_coordinates[2][1] =  0.25;
        process_coordinates[2][2] =  0.25;

        const double rate = 13.7;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }

    // A process that finds an A between two B's in the 1,1,1 direction
    // and swap the A and the first B.
    // Add site types to this process.
    {
        std::vector<std::string> process_elements1 = {"A", "B", "B"};
        std::vector<std::string> process_elements2 = {"B", "A", "B"};
        std::vector<int> process_site_types = {1, 2, 1};

        std::vector<std::vector<double> > process_coordinates = { 
            {0.0, 0.0, 0.0}, {-0.25, -0.25, -0.25}, {0.25, 0.25, 0.25} };

        const double rate = 13.7;
        const std::vector<int> move_origins = {};
        const std::vector<Coordinate> move_vectors = {};
        const int process_number = -1;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites, move_origins, move_vectors,
                  process_number, process_site_types);
        processes.push_back(p);
    }

    // A process that finds a V between two A in the 1,1,1 direction
    // and turn the V into B.
    {
        std::vector<std::string> process_elements1(3);
        process_elements1[0] = "V";
        process_elements1[1] = "B";
        process_elements1[2] = "B";

        std::vector<std::string> process_elements2(3);
        process_elements2[0] = "B";
        process_elements2[1] = "A";
        process_elements2[2] = "B";

        std::vector<std::vector<double> > process_coordinates(3, std::vector<double>(3, 0.0));

        process_coordinates[1][0] = -0.25;
        process_coordinates[1][1] = -0.25;
        process_coordinates[1][2] = -0.25;
        process_coordinates[2][0] =  0.25;
        process_coordinates[2][1] =  0.25;
        process_coordinates[2][2] =  0.25;

        const double rate = 13.7;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }

    Interactions interactions(processes, true);

    // Get a timer.
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(configuration, sitesmap, timer, lattice_map, interactions);

    // Call the single step function a couple of times to make sure it is
    // stable - the rest of the testing of this function should be done on
    // a higher level.
    const int n_loop = 1000;
    for (int i = 0; i < n_loop; ++i)
    {
        lattice_model.singleStep();

        // Check the picked index, can not be -1.
        CPPUNIT_ASSERT(interactions.pickedIndex() != -1);

        // Check the process available sites vector.
        const auto & available_sites = interactions.processAvailableSites();
        bool available = false;
        for (auto it = available_sites.begin(); it != available_sites.end(); ++it)
        {
            if(*it != 0)
            {
                available = true;
            }
        }
        CPPUNIT_ASSERT(available);
    }

    // }}}
}


// -------------------------------------------------------------------------
//
void Test_LatticeModel::testSingleStepFunction2D()
{
    // {{{
    // Setup a realistic system and check.
    std::vector< std::vector<double> > basis(1, std::vector<double>(3, 0.0));

    std::vector<int> basis_sites(1, 0);

    // Make a 10x10x1 structure.
    const int nI = 10;
    const int nJ = 10;
    const int nK = 1;
    
    // Coordinates.
    std::vector< std::vector<double> > coordinates;

    // Site types.
    std::vector<std::string> site_types;

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                std::vector<double> c(3);
                c[0] = static_cast<double>(i);
                c[1] = static_cast<double>(j);
                c[2] = static_cast<double>(k);
                coordinates.push_back(c);
                site_types.push_back("M");
            }
        }
    }

    std::vector<std::string> elements = {
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "A", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "A", "B", "B", "B", "B", "B", "A", "B", "B",
        "B", "B", "B", "B", "B", "B", "A", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "B", "B", "B",
        "B", "B", "B", "B", "B", "B", "B", "A", "B", "B",
        "A", "B", "B", "B", "B", "B", "B", "B", "B", "A"
    };

    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;

    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;

    // Setup the configuration.
    Configuration configuration(coordinates, elements, possible_types);

    // Setup the configuration.
    SitesMap sitesmap(coordinates, site_types, possible_site_types);

    // Setup the lattice map.
    std::vector<int> repetitions = {10, 10, 1};
    std::vector<bool> periodicity = {true, true, false};
    LatticeMap lattice_map(1, repetitions, periodicity);

    // Setup the interactions object.
    const std::vector<std::vector<double> > process_coordinates = {
        {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}, {2.0, 2.0, 0.0}
    };
    std::vector<Process> processes;
    const double rate = 13.2;

    // Get processes objects.
    {
        const std::vector<std::string> before = {"A", "B", "B", "B", "B", "A"};
        const std::vector<std::string> after = {"B", "B", "A", "B", "B", "A"};
        Configuration c1(process_coordinates, before, possible_types);
        Configuration c2(process_coordinates, after, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }
    {
        const std::vector<std::string> before = {"A", "B", "B", "B", "B", "B"};
        const std::vector<std::string> after = {"B", "B", "A", "B", "B", "B"};
        Configuration c1(process_coordinates, before, possible_types);
        Configuration c2(process_coordinates, after, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }
    {
        const std::vector<std::string> before = {"A", "B", "B", "B", "B", "B"};
        const std::vector<std::string> after = {"B", "B", "B", "A", "B", "B"};
        Configuration c1(process_coordinates, before, possible_types);
        Configuration c2(process_coordinates, after, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }
    {
        const std::vector<std::string> before = {"A", "B", "B", "B", "B", "B"};
        const std::vector<std::string> after = {"B", "B", "B", "B", "A", "B"};
        Configuration c1(process_coordinates, before, possible_types);
        Configuration c2(process_coordinates, after, possible_types);
        Process p(c1, c2, rate, basis_sites);
        processes.push_back(p);
    }

    {
        Interactions interactions(processes, false);

        // Calculate the configuration matchlists.
        configuration.initMatchLists(lattice_map, interactions.maxRange());

        // Update the interactions matchlists.
        interactions.updateProcessMatchLists(configuration, lattice_map);

        // Check the match list when implicit wildcard is not enabled.
        const ProcessMatchList & m = interactions.processes()[0]->matchList();
        const int match_types[6] = {1, 2, 2, 2, 2, 1};
        for (size_t i = 0; i < m.size(); ++i)
        {
            CPPUNIT_ASSERT_EQUAL(m[i].match_type, match_types[i]);
        }
    }

    Interactions interactions(processes, true);

    // Calculate the configuration matchlists.
    configuration.initMatchLists(lattice_map, interactions.maxRange());

    // Update the interactions matchlists.
    interactions.updateProcessMatchLists(configuration, lattice_map);

    // Check the match list when implicit wildcard is not enabled.
    const ProcessMatchList & m = interactions.processes()[0]->matchList();
    const int match_types[25] = {1, 2, 2, 2, 2,
                                 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 1};

    for (size_t i = 0; i < m.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(m[i].match_type, match_types[i]);
    }

    // Get a timer.
    SimulationTimer timer;

    // Construct a lattice model to test.
    LatticeModel lattice_model(configuration, sitesmap, timer, lattice_map, interactions);

    // Call the single step function a couple of times to make sure it is
    // stable - the rest of the testing of this function should be done on
    // a higher level.
    lattice_model.singleStep();

    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testAffectedIndicesRematching()
{
    // {{{
    // Setup a realistic system and check.
    std::vector<std::vector<double> > basis = {{0.000000e+00, 0.000000e+00, 0.000000e+00},
                                               {0.000000e+00, 5.000000e-01, 0.000000e+00},
                                               {5.000000e-01, 0.000000e+00, 0.000000e+00},
                                               {5.000000e-01, 5.000000e-01, 0.000000e+00}};
    std::vector<int> basis_sites = {0, 1, 2, 3};

    // Make a 10x10x1 structure.
    const int nI = 10;
    const int nJ = 10;
    const int nK = 1;
    const int nB = 4;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> elements;
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
                    elements.push_back("V");
                    site_types.push_back("P");
                }
            }
        }
    }

    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["O_u"] = 1;
    possible_types["C"] = 2;
    possible_types["O_s"] = 3;
    possible_types["O_r"] = 4;
    possible_types["O_d"] = 5;
    possible_types["V"] = 6;
    possible_types["O_l"] = 7;

    // Possible site types.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["P"] = 1;

    // Setup the configuration.
    Configuration configuration(coordinates, elements, possible_types);

    // Setup the sitesmap.
    SitesMap sitesmap(coordinates, site_types, possible_site_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity = {true, true, false};
    LatticeMap lattice_map(nB, repetitions, periodicity);

    // Setup the interactions object.
    std::vector<Process> processes;

    // process 0.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 1.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 7.067697e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 2.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 3.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 7.067697e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 4.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 5.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 9.424024e-10;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 6.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 7.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 8.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_u","V","V","V","V","V","V","V","V","O_d","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   5.000000e-01,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 9.
    {
        const std::vector<std::string> process_elements1 = {"O_u","V","V","V","V","V","V","V","V","O_d","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   5.000000e-01,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 10.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00},
            {   2.000000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 3.373446e-06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 11.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00},
            {   2.000000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.705354e-24;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 12.
    {
        const std::vector<std::string> process_elements1 = {"O_u","V","V","V","V","V","V","V","V","O_d","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","O_s","V","V","V","V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   0.000000e+00,   2.000000e+00,   0.000000e+00}
        };
        const double rate = 3.373446e-06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 13.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_u","V","V","V","V","V","V","V","V","O_d","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   5.000000e-01,   1.500000e+00,   0.000000e+00},
            {   0.000000e+00,   2.000000e+00,   0.000000e+00}
        };
        const double rate = 2.705354e-24;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 14.
    {
        const std::vector<std::string> process_elements1 = {"V","C","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","C","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 15.
    {
        const std::vector<std::string> process_elements1 = {"O_r","C","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","C","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 16.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V","V","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 17.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V","V","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 18.
    {
        const std::vector<std::string> process_elements1 = {"V","C","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","C","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 19.
    {
        const std::vector<std::string> process_elements1 = {"O_d","C","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","C","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 20.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V","V","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 21.
    {
        const std::vector<std::string> process_elements1 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V","V","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 1.379590e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 22.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 23.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 9.424024e-10;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 24.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","C","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 25.
    {
        const std::vector<std::string> process_elements1 = {"O_r","C","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 9.424024e-10;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 26.
    {
        const std::vector<std::string> process_elements1 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","C","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 27.
    {
        const std::vector<std::string> process_elements1 = {"O_d","C","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 9.424024e-10;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 28.
    {
        const std::vector<std::string> process_elements1 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 1.153555e+01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 29.
    {
        const std::vector<std::string> process_elements1 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 9.424024e-10;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 30.
    {
        const std::vector<std::string> process_elements1 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","O_s","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 8.707964e-01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 31.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","V","V","V","V","V","V","V","O_l","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 7.374637e-41;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 32.
    {
        const std::vector<std::string> process_elements1 = {"O_r","C","V","V","V","V","V","V","V","O_l","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","O_s","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 8.707964e-01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 33.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_r","V","C","V","V","V","V","V","V","V","O_l","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00},
            {   1.500000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   5.000000e-01,   0.000000e+00}
        };
        const double rate = 7.374637e-41;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 34.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","C","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 8.707964e-01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 35.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","C","V","V","V","V","V","O_u","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00}
        };
        const double rate = 7.374637e-41;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 36.
    {
        const std::vector<std::string> process_elements1 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 8.707964e-01;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 37.
    {
        const std::vector<std::string> process_elements1 = {"V","O_s","V","V","V","V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_d","V","V","V","V","V","V","V","V","O_u","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   0.000000e+00,  -1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -1.500000e+00,   0.000000e+00},
            {  -5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   5.000000e-01,  -1.500000e+00,   0.000000e+00},
            {   0.000000e+00,  -2.000000e+00,   0.000000e+00}
        };
        const double rate = 7.374637e-41;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {0};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 38.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 39.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 4.908398e-34;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 40.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.158534e+02;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 41.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 4.908398e-34;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 42.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","O_s","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.575288e+06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 43.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","O_s","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00}
        };
        const double rate = 3.878957e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 44.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","C","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00}
        };
        const double rate = 1.575288e+06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 45.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","C","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -1.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00}
        };
        const double rate = 3.878957e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 46.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","C","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00}
        };
        const double rate = 1.575288e+06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 47.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","C","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00}
        };
        const double rate = 3.878957e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 48.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","O_s","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00}
        };
        const double rate = 1.575288e+06;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 49.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","O_s","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {  -1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {  -5.000000e-01,  -1.000000e+00,   0.000000e+00}
        };
        const double rate = 3.878957e-14;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 50.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.100479e+03;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 51.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 3.365283e-09;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 52.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.100479e+03;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 53.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 3.365283e-09;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 54.
    {
        const std::vector<std::string> process_elements1 = {"C","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.100479e+03;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 55.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"C","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 3.365283e-09;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 56.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","C","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.100479e+03;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 57.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","C","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 3.365283e-09;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 58.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.209190e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 59.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","O_s","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   1.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   5.000000e-01,   1.000000e+00,   0.000000e+00},
            {   0.000000e+00,   1.500000e+00,   0.000000e+00}
        };
        const double rate = 2.209190e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {1};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 60.
    {
        const std::vector<std::string> process_elements1 = {"O_s","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.209190e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    // process 61.
    {
        const std::vector<std::string> process_elements1 = {"V","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::string> process_elements2 = {"O_s","V","V","V","V","V","V","V","V","V","V"};
        const std::vector<std::vector<double> > process_coordinates = {
            {   0.000000e+00,   0.000000e+00,   0.000000e+00},
            {  -5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   0.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   0.000000e+00,   5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   0.000000e+00,   0.000000e+00},
            {   5.000000e-01,  -5.000000e-01,   0.000000e+00},
            {   5.000000e-01,   5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   0.000000e+00,   0.000000e+00},
            {   1.000000e+00,  -5.000000e-01,   0.000000e+00},
            {   1.000000e+00,   5.000000e-01,   0.000000e+00},
            {   1.500000e+00,   0.000000e+00,   0.000000e+00}
        };
        const double rate = 2.209190e-07;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        std::vector<int> basis_site = {2};
        Process p(c1, c2, rate, basis_site);
        // Store twize.
        processes.push_back(p);
    }
    Interactions interactions(processes, true);
    // Seed the random number generator to make the test reproducible.
    seedRandom(false, 13996);
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(configuration, sitesmap, timer, lattice_map, interactions);

    // Do KMC loop to check affected indices re-matching.
    const int n_loop = 50;
    for (int i = 0; i < n_loop; ++i)
    {
        lattice_model.singleStep();
        if (i == 26)
        {
            // Check if site 228 is listed in process 28.
            const Interactions & interactions = lattice_model.interactions();
            const Process* p = interactions.processes()[28];
            CPPUNIT_ASSERT(!p->isListed(252));
            break;
        }
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testRedistribute()
{
    // {{{
    // Construct two global configurations.
    int nI = 4, nJ = 4, nK = 4, nB = 2;
    std::vector<double> basis_coords = {0.0, 0.5};
    std::vector<std::string> basis_elem = {"A", "B"};
    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    std::vector<std::string> site_types;
    std::vector<double> coord(3, 0.0);

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                for (int b = 0; b < nB; ++b)
                {
                    coord[0] = i + basis_coords[b];
                    coord[1] = j + basis_coords[b];
                    coord[2] = k + basis_coords[b];
                    coords.push_back(coord);
                    elements.push_back("V");
                    site_types.push_back("P");
                }
            }
        }
    }


    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Change one specific element.
    elements[0] = "A";
    elements[1] = "B";
    elements[32] = "B";
    elements[2] = "A";
    elements[3] = "B";
    elements[63] = "A";

    Configuration config(coords, elements, possible_types);

    // Setup sitesmap.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["P"] = 1;

    SitesMap sitesmap(coords, site_types, possible_site_types);

    // Setup interactions.
    std::vector<Process> processes;

    // Rate for all processes.
    const double rate = 1.0;

    // Processes definitions.
    // {{{
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
    // }}}

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap lattice_map(n_basis, repetitions, periodicity);

    // Get a timer.
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(config, sitesmap, timer, lattice_map, interactions);

    auto ori_elements = config.elements();
    auto ori_types = config.types();
    auto ori_atom_id = config.atomID();

    // Redistribute.
    const std::vector<std::string> fast_species = {"V"};
    const std::vector<int> slow_indices = {2};
    std::vector<int> affected_indices = lattice_model.redistribute(fast_species,
                                                                   slow_indices,
                                                                   2, 2, 2);

    // Check redistributed configuration.
    auto new_config = lattice_model.configuration();

    auto new_elements = new_config.elements();
    auto new_types = new_config.types();
    auto new_atom_id = new_config.atomID();

    // Check slow species.
    CPPUNIT_ASSERT_EQUAL(new_elements[0], static_cast<std::string>("A"));
    CPPUNIT_ASSERT_EQUAL(new_elements[1], static_cast<std::string>("B"));
    CPPUNIT_ASSERT_EQUAL(new_types[0], 1);
    CPPUNIT_ASSERT_EQUAL(new_types[1], 2);
    CPPUNIT_ASSERT_EQUAL(new_atom_id[0], 0);
    CPPUNIT_ASSERT_EQUAL(new_atom_id[1], 1);

    bool elements_changed = false;
    bool types_changed = false;
    bool atom_id_changed = false;

    for (size_t i = 3; i < ori_elements.size(); ++i)
    {
        if (new_elements[i] != ori_elements[i])
        {
            elements_changed = true;
        }

        if (new_types[i] != ori_types[i])
        {
            types_changed = true;
        }

        if (new_atom_id[i] != ori_atom_id[i])
        {
            atom_id_changed = true;
        }
    }

    CPPUNIT_ASSERT(elements_changed);
    CPPUNIT_ASSERT(types_changed);
    CPPUNIT_ASSERT(atom_id_changed);

    // Sort all data.
    std::sort(ori_elements.begin(), ori_elements.end());
    std::sort(ori_types.begin(), ori_types.end());
    std::sort(ori_atom_id.begin(), ori_atom_id.end());
    std::sort(new_elements.begin(), new_elements.end());
    std::sort(new_types.begin(), new_types.end());
    std::sort(new_atom_id.begin(), new_atom_id.end());

    for (size_t i = 0; i < ori_elements.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(ori_elements[i], new_elements[i]);
        CPPUNIT_ASSERT_EQUAL(ori_types[i], new_types[i]);
        CPPUNIT_ASSERT_EQUAL(ori_atom_id[i], new_atom_id[i]);
    }

    // Check the affected indices.
    std::sort(affected_indices.begin(), affected_indices.end());
    for (size_t i = 0, j = 3; i < affected_indices.size(); ++i, ++j)
    {
        CPPUNIT_ASSERT_EQUAL(affected_indices[i], static_cast<int>(j));
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testProcessRedistribute()
{
    // {{{
    // Construct two global configurations.
    int nI = 4, nJ = 4, nK = 4, nB = 2;
    std::vector<double> basis_coords = {0.0, 0.5};
    std::vector<std::string> basis_elem = {"A", "B"};
    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    std::vector<std::string> site_types;
    std::vector<double> coord(3, 0.0);

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                for (int b = 0; b < nB; ++b)
                {
                    coord[0] = i + basis_coords[b];
                    coord[1] = j + basis_coords[b];
                    coord[2] = k + basis_coords[b];
                    coords.push_back(coord);
                    elements.push_back("V");
                    site_types.push_back("P");
                }
            }
        }
    }


    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Change one specific element.
    elements[0] = "A";
    elements[1] = "B";
    elements[32] = "B";
    elements[2] = "A";
    elements[3] = "B";
    elements[63] = "A";

    Configuration config(coords, elements, possible_types);

    // Setup sitesmap.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["P"] = 1;

    SitesMap sitesmap(coords, site_types, possible_site_types);

    // Setup interactions.
    std::vector<Process> processes;

    // Rate for all processes.
    const double rate = 1.0;

    // Processes definitions.
    // {{{
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
    // A adsorption at basis site 0.
    {
        std::vector<std::string> elements1 = {"V"};
        std::vector<std::string> elements2 = {"A"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, true, true, "A");
        processes.push_back(process);
    }
    // A adsorption at basis site 1.
    {
        std::vector<std::string> elements1 = {"V"};
        std::vector<std::string> elements2 = {"A"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {1}, true, true, "A");
        processes.push_back(process);
    }
    // B adsorption at basis site 0.
    {
        std::vector<std::string> elements1 = {"V"};
        std::vector<std::string> elements2 = {"B"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, true, true, "B");
        processes.push_back(process);
    }
    // B adsorption at basis site 1.
    {
        std::vector<std::string> elements1 = {"V"};
        std::vector<std::string> elements2 = {"B"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {1}, true, true, "B");
        processes.push_back(process);
    }
    // }}}

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap lattice_map(n_basis, repetitions, periodicity);

    // Get a timer.
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(config, sitesmap, timer, lattice_map, interactions);

    auto ori_elements = config.elements();
    auto ori_types = config.types();

    // Redistribute.
    const std::string empty_species("V");
    const std::vector<int> slow_indices = {2};
    std::vector<int> affected_indices = lattice_model.processRedistribute(empty_species,
                                                                          {},
                                                                          slow_indices,
                                                                          2, 2, 2);

    // Check redistributed configuration.
    auto new_config = lattice_model.configuration();

    auto new_elements = new_config.elements();
    auto new_types = new_config.types();

    // Check slow species.
    CPPUNIT_ASSERT_EQUAL(new_elements[0], static_cast<std::string>("A"));
    CPPUNIT_ASSERT_EQUAL(new_elements[1], static_cast<std::string>("B"));
    CPPUNIT_ASSERT_EQUAL(new_types[0], 1);
    CPPUNIT_ASSERT_EQUAL(new_types[1], 2);

    bool elements_changed = false;
    bool types_changed = false;

    for (size_t i = 3; i < ori_elements.size(); ++i)
    {
        if (new_elements[i] != ori_elements[i])
        {
            elements_changed = true;
        }

        if (new_types[i] != ori_types[i])
        {
            types_changed = true;
        }
    }

    CPPUNIT_ASSERT(elements_changed);
    CPPUNIT_ASSERT(types_changed);

    // Sort all data.
    std::sort(ori_elements.begin(), ori_elements.end());
    std::sort(ori_types.begin(), ori_types.end());
    std::sort(new_elements.begin(), new_elements.end());
    std::sort(new_types.begin(), new_types.end());

    for (size_t i = 0; i < ori_elements.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(ori_elements[i], new_elements[i]);
        CPPUNIT_ASSERT_EQUAL(ori_types[i], new_types[i]);
    }
    // }}}
}


// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testSingleStepWithRedistribution()
{
    // {{{
    // Construct two global configurations.
    int nI = 4, nJ = 4, nK = 4, nB = 2;
    std::vector<double> basis_coords = {0.0, 0.5};
    std::vector<std::string> basis_elem = {"A", "B"};
    std::vector<std::string> elements;
    std::vector<std::vector<double> > coords;
    std::vector<std::string> site_types;
    std::vector<double> coord(3, 0.0);

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                for (int b = 0; b < nB; ++b)
                {
                    coord[0] = i + basis_coords[b];
                    coord[1] = j + basis_coords[b];
                    coord[2] = k + basis_coords[b];
                    coords.push_back(coord);
                    elements.push_back("V");
                    site_types.push_back("P");
                }
            }
        }
    }


    // Setup the mapping from element to integer.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["B"] = 2;
    possible_types["V"] = 3;

    // Change one specific element.
    elements[0] = "A";
    elements[1] = "B";
    elements[32] = "B";
    elements[2] = "A";
    elements[3] = "B";
    elements[68] = "A";

    Configuration config(coords, elements, possible_types);

    // Setup sitesmap.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["P"] = 1;

    SitesMap sitesmap(coords, site_types, possible_site_types);

    // Setup interactions.
    std::vector<Process> processes;

    // Rate for all processes.
    const double rate = 1.0;

    // Processes definitions.
    // {{{
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
    // V + V -> A + B.
    {
        std::vector<std::string> elements1 = {"V", "V"};
        std::vector<std::string> elements2 = {"A", "B"};
        std::vector<std::vector<double> > process_coords = {{0.0, 0.0, 0.0},
                                                            {0.5, 0.5, 0.5}};
        const Configuration config1(process_coords, elements1, possible_types);
        const Configuration config2(process_coords, elements2, possible_types);

        Process process(config1, config2, rate, {0}, false);
        processes.push_back(process);
    }
    // }}}

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap lattice_map(n_basis, repetitions, periodicity);

    // Get a timer.
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(config, sitesmap, timer, lattice_map, interactions);

    // Check single step with redistribution.
    const std::vector<std::string> fast_species = {"V"};
    const std::vector<int> slow_indices = {2};
    const int redis_interval = 10;
    int redis_counter = 1;
    const int n_loop = 1000;

    for (int i = 0; i < n_loop; ++i)
    {
        if (redis_counter % redis_interval == 0)
        {
            lattice_model.redistribute(fast_species, slow_indices, 2, 2, 2);
            redis_counter = 1;
        }
        else
        {
            lattice_model.singleStep();
            redis_counter++;
        }
    }
    // }}}
}

// -------------------------------------------------------------------------- //
//
void Test_LatticeModel::testTiming()
{
    // {{{
    // Possible types.
    std::map<std::string, int> possible_types;
    possible_types["*"] = 0;
    possible_types["A"] = 1;
    possible_types["V"] = 2;

    // Setup the interactions object.
    std::vector<Process> processes;

    // Get a basis sites list.
    std::vector<int> basis_sites;
    basis_sites.push_back(0);
    basis_sites.push_back(1);
    basis_sites.push_back(2);

    // This defines the moving directions.
    std::vector<std::vector<double> > process_coordinates(7, std::vector<double>(3, 0.0));
    process_coordinates[1][0] = -1.0;
    process_coordinates[2][0] =  1.0;
    process_coordinates[3][1] = -1.0;
    process_coordinates[4][1] =  1.0;
    process_coordinates[5][2] = -1.0;
    process_coordinates[6][2] =  1.0;

    // A process that moves a vacancy to the left with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "V";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }


    // A process that moves a vacancy to the right with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "V";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // A process that moves a vacancy to the front with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "V";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // A process that moves a vacancy to the back with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // donw
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "V";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // A process that moves a vacancy down with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "V";
        process_elements2[6] = "A";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // A process that moves a vacancy up with rate 10
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "V";

        const double rate = 10.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Processes that moves a vacancy away from other vacancies with rate 15.

    // Left.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "V";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "V";
        process_elements2[2] = "V";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Right.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "V";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "V";
        process_elements2[2] = "V";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Front.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "V";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "V";
        process_elements2[4] = "V";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Back.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "V";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // donw
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "V";
        process_elements2[4] = "V";
        process_elements2[5] = "A";
        process_elements2[6] = "A";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Down.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "A";  // down
        process_elements1[6] = "V";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "V";
        process_elements2[6] = "V";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Up.
    {
        std::vector<std::string> process_elements1(7);
        process_elements1[0] = "V";  // center
        process_elements1[1] = "A";  // left
        process_elements1[2] = "A";  // right
        process_elements1[3] = "A";  // front
        process_elements1[4] = "A";  // back
        process_elements1[5] = "V";  // down
        process_elements1[6] = "A";  // up

        std::vector<std::string> process_elements2(7);
        process_elements2[0] = "A";
        process_elements2[1] = "A";
        process_elements2[2] = "A";
        process_elements2[3] = "A";
        process_elements2[4] = "A";
        process_elements2[5] = "V";
        process_elements2[6] = "V";

        const double rate = 15.0;
        Configuration c1(process_coordinates, process_elements1, possible_types);
        Configuration c2(process_coordinates, process_elements2, possible_types);
        Process p(c1, c2, rate, basis_sites);
        // Store.
        processes.push_back(p);
    }

    // Setup a binary 3D system with voids and occupied.
    std::vector< std::vector<double> > basis(1, std::vector<double>(3,0.0));

    const int nI = 1000;
    const int nJ = 10;
    const int nK = 10;
    const int nB = 1;

    // Coordinates and elements.
    std::vector<std::vector<double> > coordinates;
    std::vector<std::string> elements;
    std::vector<std::string> site_types;

    // Seed the random number generator to make the test reproducible.
    seedRandom(false, 14159265);

    for (int i = 0; i < nI; ++i)
    {
        for (int j = 0; j < nJ; ++j)
        {
            for (int k = 0; k < nK; ++k)
            {
                std::vector<double> c(3);
                c[0] = i;
                c[1] = j;
                c[2] = k;
                coordinates.push_back(c);

                if (randomDouble01() < 0.05)
                {
                    elements.push_back("V");
                    site_types.push_back("M");
                }
                else
                {
                    elements.push_back("A");
                    site_types.push_back("N");
                }
            }
        }
    }

    // Setup the configuration.
    Configuration configuration(coordinates, elements, possible_types);

    // Setup the sitesmap.
    std::map<std::string, int> possible_site_types;
    possible_site_types["*"] = 0;
    possible_site_types["M"] = 1;
    possible_site_types["N"] = 2;

    SitesMap sitesmap(coordinates, site_types, possible_site_types);

    // Setup the lattice map.
    std::vector<int> repetitions(3);
    repetitions[0] = nI;
    repetitions[1] = nJ;
    repetitions[2] = nK;
    std::vector<bool> periodicity(3, true);
    LatticeMap lattice_map(nB, repetitions, periodicity);
    Interactions interactions(processes, false);
    SimulationTimer timer;

    // Construct the lattice model to test.
    LatticeModel lattice_model(configuration, sitesmap, timer, lattice_map, interactions);

    // Call the single step function a couple of times to make sure it is
    // stable - the rest of the testing of this function should be done on
    // a higher level.

    time_t seconds;
    time(&seconds);

    const int n_loop = 10000;
    for (int i = 0; i < n_loop; ++i)
    {
        lattice_model.singleStep();
    }

    time_t seconds2;
    time(&seconds2);

    int diff_t = static_cast<int>(seconds2-seconds);
    printf("\nTIMING: %i steps were performed in %i seconds (%f ms/step)\n",
           n_loop,
           diff_t,
           1000.0*static_cast<double>(diff_t)/n_loop);

    printf("        with %i processes (7 centers per process) for %i sites in the lattice.\n",
           static_cast<int>(processes.size()), nI*nJ*nK*nB);

    // }}}
}

