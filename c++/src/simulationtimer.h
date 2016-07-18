/*
  Copyright (c)  2012  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  simulationtimer.h
 *  \brief File for the KMCClock class definition.
 */

#ifndef __SIMULATIONTIMER__
#define __SIMULATIONTIMER__

/*! \brief Class for keeping track of simulation (KMC) time.
 */
class SimulationTimer {

public:

    /*! \brief Default constructor.
     */
    SimulationTimer(double start_time = 0.0);

    /*! \brief Propagate the time.
     *  \param total_rate: The total rate of the system.
     */
    void propagateTime(const double total_rate);

    /*! \brief Query for the simulation time.
     *  \return : The current simulation time.
     */
    double simulationTime() const { return simulation_time_; }

    /*! \brief Query for the delta time.
     *  \return : The change of time in last step.
     */
    double deltaTime() const { return delta_time_; }

protected:

private:

    /// The time of the KMC simulation.
    double simulation_time_;

    /// The change of time.
    double delta_time_;

};


#endif // __SIMULATIONTIMER__

