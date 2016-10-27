/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>

#include "test_distributor.h"

#include "distributor.h"
#include "process.h"
#include "configuration.h"
#include "interactions.h"
#include "latticemap.h"
#include "sitesmap.h"
#include "matcher.h"


// ----------------------------------------------------------------------------
//
void Test_Distributor::testConstruction()
{
    // Construction.
    Distributor dis;
}


// ----------------------------------------------------------------------------
//
void Test_Distributor::testReDistribution()
{
    // {{{
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

    Matcher matcher;
    matcher.calculateMatching(interactions, config, sitesmap,
                              lattice_map, indices);

    // Classify configuration.
    matcher.classifyConfiguration(interactions,
                                  config,
                                  sitesmap,
                                  lattice_map,
                                  indices,
                                  fast_elements);

    // Redistribution.
    Distributor distributor;

    // Copy the original variables.
    auto ori_elements = config.elements();
    auto ori_types = config.types();
    auto ori_atom_id = config.atomID();

    distributor.reDistribute(config);

    // Check configuration after redistribution.
    // There should be no changes on the 0th and 1st elements.
    CPPUNIT_ASSERT_EQUAL(config.elements()[0], static_cast<std::string>("A"));
    CPPUNIT_ASSERT_EQUAL(config.elements()[1], static_cast<std::string>("B"));
    CPPUNIT_ASSERT_EQUAL(config.types()[0], 1);
    CPPUNIT_ASSERT_EQUAL(config.types()[1], 2);
    CPPUNIT_ASSERT_EQUAL(config.atomID()[0], 0);
    CPPUNIT_ASSERT_EQUAL(config.atomID()[1], 1);

    // Check other elements
    bool element_changed = false;
    bool type_changed = false;
    bool atom_id_changed = false;

    for (size_t i = 2; i < ori_elements.size(); ++i)
    {
        if (config.elements()[i] != ori_elements[i])
        {
            element_changed = true;
        }

        if (config.types()[i] != ori_types[i])
        {
            type_changed = true;
        }

        if (config.atomID()[i] != ori_atom_id[i])
        {
            atom_id_changed = true;
        }
    }

    CPPUNIT_ASSERT(element_changed);
    CPPUNIT_ASSERT(type_changed);
    CPPUNIT_ASSERT(atom_id_changed);

    // Sort the redistributed vectors and compare them to the original ones.
    auto new_elements = config.elements();
    auto new_types = config.types();
    auto new_atom_id = config.atomID();

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

    // }}}
}

