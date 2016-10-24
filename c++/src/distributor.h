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
 *  zjshao     2016-10-23   1.4          Inital creation.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */


#ifndef __DISTRIBUTOR__
#define __DISTRIBUTOR__

// Forward declarations.
class Configuration;
class Matcher;

/*! \brief Class for configuration/geometries redistribution.
 */
class Distributor {

public:
    /*! \brief Default constructor.
     */
    Distributor() {}

    /*! \brief Re-distribute the configuration.
     *  \param configuration : The reference of the configuration to be
     *                         redistributed.
     *                       
     *  NOTE: 1. This is a friend funciton of Cofiguration class.
     *        2. The configuration must be passed after classification.
     */
    void reDistribute(Configuration & configuration) const; 

protected:

private:

};

#endif

