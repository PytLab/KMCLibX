/*
  Copyright (c)  2012  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : latticemodel.h
 *  brief  : File for the LatticeModel class definition.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-11-01   1.4          Add redistribution.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#ifndef __LATTICEMODEL__
#define __LATTICEMODEL__


#include "latticemap.h"
#include "interactions.h"
#include "matcher.h"
#include "distributor.h"

// Forward declarations.
class Configuration;
class SitesMap;
class SimulationTimer;
class Process;

/// Class for defining and running a lattice KMC model.
class LatticeModel {

public:

    /*! \brief Constructor for setting up the model.
     *  \param configuration    : The configuration to run the simulation on.
     *  \param simulation_timer : The timer for the simulation.
     *  \param lattice_map      : A lattice map object describing the lattice.
     *  \param interactions     : An interactions object describing all interactions
     *                            and possible processes in the system.
     */
    LatticeModel(Configuration & configuration,
                 SitesMap & sitesmap,
                 SimulationTimer & simulation_timer,
                 const LatticeMap & lattice_map,
                 Interactions & interactions);

    /*! \brief Function for taking one time step in the KMC lattice model.
     */
    void singleStep();

    /*! \brief Function for redistributing configuration completely randomly
     *                  in KMC iteration.
     *  \param fast_species : The list of default fast species.
     *  \param slow_indices : The indices on which the species is slow.
     *  \param x : The split number on x axis.
     *  \param y : The split number on y axis.
     *  \param z : The split number on z axis.
     *  \return  : The affected indices/the fast indices before the redistribution.
     */
    const std::vector<int> redistribute(const std::vector<std::string> & fast_species = {},
                                        const std::vector<int> & slow_indices = {},
                                        int x = 1, int y = 1, int z = 1);

    /*! \brief Function for redistributing configuration using process matching
     *                  in KMC iteration.
     *  \param replace_elements : The name of substitution element for the removed
     *                            elements(it is usually an emtpy type).
     *  \param fast_species : The list of default fast species.
     *  \param slow_indices : The indices on which the species is slow.
     *  \param x : The split number on x axis.
     *  \param y : The split number on y axis.
     *  \param z : The split number on z axis.
     *  \return  : The affected indices/the fast indices before the redistribution.
     */
    const std::vector<int> processRedistribute(const std::string & replace_elements,
                                               const std::vector<std::string> & fast_species = {},
                                               const std::vector<int> & slow_indices = {},
                                               int x = 1, int y = 1, int z = 1);

    /*! \brief Query for the interactions.
     *  \return : A handle to the interactions stored on the class.
     */
    const Interactions & interactions() const { return interactions_; }

    /*! \brief Query for the configuration.
     *  \return : A handle to the configuration stored on the class.
     */
    const Configuration & configuration() const { return configuration_; }

    /*! \brief Query for the lattice map.
     *  \return : A handle to the lattice map stored on the class.
     */
    const LatticeMap & latticeMap() const { return lattice_map_; }

    /*! \brief Query for the sites map.
     *  \return : A handle to the sites map stored on the class.
     */
    const SitesMap & sitesMap() const { return sitesmap_; }

protected:

private:

    /*! \brief Private helper function to initiate matching of all
     *         processes with all indices in the configuration.
     */
    void calculateInitialMatching();
    
    /// A reference to the configuration given at construction.
    Configuration & configuration_;

    /// A reference to the sites map given at construction.
    SitesMap & sitesmap_;

    /// A reference to the timer given at construction.
    SimulationTimer & simulation_timer_;

    /// A description of the lattice.
    LatticeMap lattice_map_;

    /// The reference to the description of all interactions in the system.
    Interactions & interactions_;

    /// The Matcher to use for calculating matches and update the process lists.
    Matcher matcher_;

    /// The random Distributor for re-distributing configuration.
    ConstrainedRandomDistributor distributor_;
};


#endif // __LATTICEMODEL__

