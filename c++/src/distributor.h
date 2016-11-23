/*
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLibX project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : distributor.h
 *  brief  : File for the Distributor class definition.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-10-23   1.4          Initial creation.
 *  zjshao     2016-11-22   1.4          Add new redistribute method.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#ifndef __DISTRIBUTOR__
#define __DISTRIBUTOR__

#include <vector>
#include <string>

// Forward declarations.
class Configuration;
class SubConfiguration;
class Matcher;
class LatticeMap;
class SitesMap;
class Interactions;

/*! \brief Class for configuration/geometries redistribution.
 *  NOTE: The class is a friend class of Configuration/SubConfiguration.
 */
class BaseDistributor {

public:

    /*! \brief Default constructor.
     */
    BaseDistributor() {}

    /*! \brief Destructor.
     */
    virtual ~BaseDistributor() {}

    /*! \brief Pure virtual memeber function which would be overrode by
     *         other derived classes.
     *  \param configuration : The reference of the configuration to be
     *                         redistributed.
     */
    virtual std::vector<int> redistribute(Configuration & configuration) const = 0;

protected:

private:

};


/*! \brief Random distributor class definition.
 */
class RandomDistributor : public BaseDistributor {

public:

    /*! \brief Default constructor.
     */
    RandomDistributor() {}

    /*! \brief Destructor.
     */
    virtual ~RandomDistributor() {}

    /*! \brief Re-distribute the configuration randomly completely.
     *  \param configuration : The reference of the configuration to be
     *                         redistributed.
     *  \return affected_indices : The list of global indices affected by
     *                             redistribution.
     *                       
     *  NOTE: The configuration must be passed after classification.
     */
    virtual std::vector<int> redistribute(Configuration & configuration) const;

    virtual std::vector<int> processRedistribute(Configuration & configuration,
                                                 Interactions & interactions,
                                                 const SitesMap & sitesmap,
                                                 const LatticeMap & latticemap,
                                                 const Matcher & matcher,
                                                 const std::string & replace_species) const;
};


/*! \brief Distributor for a partial random distribution.
*/
class PartialRandomDistributor : public RandomDistributor {

public:

    /*! \brief Default constructor.
     */
    PartialRandomDistributor() {}

    /*! \brief Destructor.
     */
    virtual ~PartialRandomDistributor() {}

    /*! \brief Re-distribute the configuration by spliting and redistributing
     *         randomly.
     *  \param configuration : The reference of the configuration to be
     *                         redistributed.
     *  \return affected_indices : The list of global indices affected by
     *                             redistribution.
     *
     *  NOTE: The configuration must be passed after classification.
     */
    std::vector<int> splitRedistribute(Configuration & configuration,
                                       const LatticeMap & lattice_map,
                                       int x, int y, int z) const;

    /*! \brief Update local part of global Configuration using sub-configuration.
     *  \param global_config (in/out) : The global configuration object.
     *  \param sub_config    (in)     : The local configuration object.
     */
    void updateLocalFromSubConfig(Configuration & global_config,
                                  const SubConfiguration & sub_config) const;

};

#endif

