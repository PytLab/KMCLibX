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
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#ifndef __DISTRIBUTOR__
#define __DISTRIBUTOR__

// Forward declarations.
class Configuration;
class SubConfiguration;
class Matcher;

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

    /*! \brief Pure virtual memeber function for inheritance from derived classes.
     *  \param configuration : The reference of the configuration to be
     *                         redistributed.
     */
    virtual void reDistribute(Configuration & configuration) const = 0;

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
     *                       
     *  NOTE: The configuration must be passed after classification.
     */
    virtual void reDistribute(Configuration & configuration) const;

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
     *
     *  NOTE: The configuration must be passed after classification.
     */

    /*! \brief Update local part of global Configuration using sub-configuration.
     *  \param global_config (in/out) : The global configuration object.
     *  \param sub_config    (in)     : The local configuration object.
     */
    void updateLocalFromSubConfig(Configuration & global_config,
                                  const SubConfiguration & sub_config) const;

};

#endif

