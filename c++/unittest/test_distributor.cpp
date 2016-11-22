/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <iostream>

#include "test_distributor.h"

#include "distributor.h"
#include "process.h"
#include "configuration.h"
#include "interactions.h"
#include "latticemap.h"
#include "sitesmap.h"
#include "matcher.h"
#include "random.h"


// ----------------------------------------------------------------------------
//
void Test_Distributor::testConstruction()
{
    // Construction.
    RandomDistributor random_dis;
    PartialRandomDistributor partial_random_dis;
}


// ----------------------------------------------------------------------------
//
void Test_Distributor::testRandomCompleteReDistribution()
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
    RandomDistributor distributor;

    // Copy the original variables.
    auto ori_elements = config.elements();
    auto ori_types = config.types();
    auto ori_atom_id = config.atomID();

    std::vector<int> && affected_indices = distributor.reDistribute(config);

    // Check affected indices.
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(affected_indices.size()), 52);

    //std::sort(affected_indices.begin(), affected_indices.end());
    for (size_t i = 0, j = 2; i < affected_indices.size(); ++i, ++j)
    {
        CPPUNIT_ASSERT_EQUAL(affected_indices[i], static_cast<int>(j));
    }

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

    if ( !(element_changed && atom_id_changed && type_changed) )
    {
        CPPUNIT_ASSERT(!element_changed);
        CPPUNIT_ASSERT(!atom_id_changed);
        CPPUNIT_ASSERT(!type_changed);
    }
    else
    {
        CPPUNIT_ASSERT(element_changed);
        CPPUNIT_ASSERT(atom_id_changed);
        CPPUNIT_ASSERT(type_changed);
    }

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


// -----------------------------------------------------------------------------
//
void Test_Distributor::testRandomSubConfigReDistribution()
{
    // {{{

    // Construct global configuration.
    int nI = 4, nJ = 4, nK = 4, nB = 2;
    std::vector<double> basis_coords = {0.0, 0.5};
    std::vector<std::string> basis_elem = {"A", "B"};
    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;
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

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap global_lattice(n_basis, repetitions, periodicity);

    // Initialize match lists.
    config.initMatchLists(global_lattice, interactions.maxRange());
    sitesmap.initMatchLists(global_lattice, interactions.maxRange());

    interactions.updateProcessMatchLists(config, global_lattice);

    // Match all centers.
    Matcher matcher;
    std::vector<int> indices;
    for (size_t i = 0; i < config.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher.calculateMatching(interactions, config, sitesmap,
                              global_lattice, indices);

    // Classify configuration.
    const std::vector<std::string> fast_elements = {"V"};
    matcher.classifyConfiguration(interactions,
                                  config,
                                  sitesmap,
                                  global_lattice,
                                  indices,
                                  fast_elements);

    // We have classified the species types in configuration, now split it.
    const auto && sub_lattices = global_lattice.split(2, 2, 2);
    const SubLatticeMap & sub_lattice = sub_lattices[0];

    // Extract sub-configuration.
    SubConfiguration && sub_config = config.subConfiguration(global_lattice,
                                                             sub_lattice);

    // Copy the sub-configuration.
    SubConfiguration sub_config_copy = sub_config;

    // Check sub_lattice slow flags.
    const auto slow_flags = sub_config.slowFlags();
    CPPUNIT_ASSERT(slow_flags[0]);
    CPPUNIT_ASSERT(slow_flags[1]);
    for (size_t i = 2; i < slow_flags.size(); ++i)
    {
        CPPUNIT_ASSERT(!slow_flags[i]);
    }

    // Redistribution.
    RandomDistributor distributor;

    // Copy the original variables.
    auto ori_elements = sub_config.elements();
    auto ori_types = sub_config.types();
    auto ori_atom_id = sub_config.atomID();

    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ori_atom_id.size()), 16);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ori_types.size()), 16);
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(ori_elements.size()), 16);

    // Redistribute the sub-configuraiton.
    distributor.reDistribute(sub_config);
    distributor.reDistribute(sub_config_copy);

    // The new distributions of two sub-configuration should be different.
    bool elements_different = false;
    bool types_different = false;
    bool atom_id_different = false;

    for (size_t i = 2; i < ori_elements.size(); ++i)
    {
        if (sub_config.elements()[i] != sub_config_copy.elements()[i])
        {
            elements_different = true;
        }

        if (sub_config.types()[i] != sub_config_copy.types()[i])
        {
            types_different = true;
        }

        if (sub_config.atomID()[i] != sub_config_copy.atomID()[i])
        {
            atom_id_different = true;
        }
    }
    CPPUNIT_ASSERT(elements_different);
    CPPUNIT_ASSERT(types_different);
    CPPUNIT_ASSERT(atom_id_different);

    // Check other elements
    bool elements_changed = false;
    bool atom_id_changed = false;
    bool types_changed = false;

    for (size_t i = 2; i < ori_elements.size(); ++i)
    {
        if (sub_config.elements()[i] != ori_elements[i])
        {
            elements_changed = true;
        }

        if (sub_config.atomID()[i] != ori_atom_id[i])
        {
            atom_id_changed = true;
        }

        if (sub_config.types()[i] != ori_types[i])
        {
            types_changed = true;
        }
    }

    // NOTE: It is truly possible that the elements, types and atom_id
    //       distribution do not change, so...
    if ( !(elements_changed && atom_id_changed && types_changed) )
    {
        CPPUNIT_ASSERT(!elements_changed);
        CPPUNIT_ASSERT(!atom_id_changed);
        CPPUNIT_ASSERT(!types_changed);
    }
    else
    {
        CPPUNIT_ASSERT(elements_changed);
        CPPUNIT_ASSERT(atom_id_changed);
        CPPUNIT_ASSERT(types_changed);
    }

    // Sort the redistributed vectors and compare them to the original ones.
    auto new_elements = sub_config.elements();
    auto new_types = sub_config.types();
    auto new_atom_id = sub_config.atomID();

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


// -----------------------------------------------------------------------------
//
void Test_Distributor::testUpdateLocalFromSubConfig()
{
    // {{{
    // Construct global configuration.
    int nI = 4, nJ = 4, nK = 4, nB = 2;
    std::vector<double> basis_coords = {0.0, 0.5};
    std::vector<std::string> basis_elem = {"A", "B"};
    std::vector<std::string> elements;
    std::vector<std::string> site_types;
    std::vector<std::vector<double> > coords;
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

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap global_lattice(n_basis, repetitions, periodicity);

    // Initialize match lists.
    config.initMatchLists(global_lattice, interactions.maxRange());
    sitesmap.initMatchLists(global_lattice, interactions.maxRange());

    interactions.updateProcessMatchLists(config, global_lattice);

    // Match all centers.
    Matcher matcher;
    std::vector<int> indices;
    for (size_t i = 0; i < config.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher.calculateMatching(interactions, config, sitesmap,
                              global_lattice, indices);

    // Classify configuration.
    const std::vector<std::string> fast_elements = {"V"};
    matcher.classifyConfiguration(interactions,
                                  config,
                                  sitesmap,
                                  global_lattice,
                                  indices,
                                  fast_elements);

    // We have classified the species types in configuration, now split it.
    const auto && sub_lattices = global_lattice.split(2, 2, 2);
    const SubLatticeMap & sub_lattice = sub_lattices[0];

    // Extract sub-configuration.
    SubConfiguration && sub_config = config.subConfiguration(global_lattice,
                                                             sub_lattice);

    // Redistribute the sub-configuration.
    PartialRandomDistributor distributor;
    distributor.reDistribute(sub_config);

    // Distribution in sub-configuration.
    const auto & sub_types = sub_config.types();
    const auto & sub_elements = sub_config.elements();
    const auto & sub_atom_id = sub_config.atomID();

    // Update local part of global configuration.
    distributor.updateLocalFromSubConfig(config, sub_config);

    const auto & glob_types = config.types();
    const auto & glob_elements = config.elements();
    const auto & glob_atom_id = config.atomID();

    // Get local distribution in global configuration.
    const std::vector<int> global_indices = { 0,  1,  2,  3,  8,  9, 10, 11,
                                             32, 33, 34, 35, 40, 41, 42, 43};
    
    // Check global indices consistency.
    for (size_t i = 0; i < global_indices.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(global_indices[i], sub_config.globalIndices()[i]);
    }

    for (size_t i = 0; i < global_indices.size(); ++i)
    {
        int global_index = global_indices[i];

        CPPUNIT_ASSERT_EQUAL(glob_types[global_index], sub_types[i]);
        CPPUNIT_ASSERT_EQUAL(glob_elements[global_index], sub_elements[i]);
        CPPUNIT_ASSERT_EQUAL(glob_atom_id[global_index], sub_atom_id[i]);
    }

    // }}}
}


// ----------------------------------------------------------------------------
//
void Test_Distributor::testPartialRandomReDistribute()
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

    Interactions interactions(processes, true);

    // Construct a global lattice map.
    const std::vector<int> repetitions = {4, 4, 4};
    std::vector<bool> periodicity(3, true);
    const int n_basis = 2;

    LatticeMap global_lattice(n_basis, repetitions, periodicity);

    // Initialize match lists.
    config.initMatchLists(global_lattice, interactions.maxRange());
    sitesmap.initMatchLists(global_lattice, interactions.maxRange());

    interactions.updateProcessMatchLists(config, global_lattice);

    // Match all centers.
    Matcher matcher;
    std::vector<int> indices;
    for (size_t i = 0; i < config.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher.calculateMatching(interactions, config, sitesmap,
                              global_lattice, indices);

    // Classify configuration.
    const std::vector<std::string> fast_elements = {"V"};
    matcher.classifyConfiguration(interactions,
                                  config,
                                  sitesmap,
                                  global_lattice,
                                  indices,
                                  fast_elements);

    auto ori_elements = config.elements();
    auto ori_types = config.types();
    auto ori_atom_id = config.atomID();

    // Create distributor.
    PartialRandomDistributor distributor;

    // Re-distribution.
    std::vector<int> && affected_indices = distributor.reDistribute(config,
                                                                    global_lattice,
                                                                    2, 2, 2);

    // Check affected indices.
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(affected_indices.size()), 4*4*4*2-2);

    std::sort(affected_indices.begin(), affected_indices.end());
    for (size_t i = 0, j = 2; i < affected_indices.size(); ++i, ++j)
    {
        CPPUNIT_ASSERT_EQUAL(affected_indices[i], static_cast<int>(j));
    }

    // Check re-distributed configuration.
    auto new_elements = config.elements();
    auto new_types = config.types();
    auto new_atom_id = config.atomID();

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

    for (size_t i = 2; i < ori_elements.size(); ++i)
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
    // }}}
}

