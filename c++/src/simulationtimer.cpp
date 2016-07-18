/*
  Copyright (c)  2012  Mikael Leetmaa
  Copyright (c)  2016-2019 Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  simulationtimer.cpp
 *  \brief File for the implementation code of the SimulationTimer class.
 */

#include "simulationtimer.h"
#include "random.h"
#include <cmath>


// -----------------------------------------------------------------------------
//
SimulationTimer::SimulationTimer(double start_time) :
    simulation_time_(start_time),
    delta_time_(0.0)
{
    // NOTHING HERE
}


// -----------------------------------------------------------------------------
//
void SimulationTimer::propagateTime(const double total_rate)
{
    // Propagate the time of the system.
    const double rnd = randomDouble01();
    const double dt  = -std::log(rnd)/total_rate;
    delta_time_ = dt;
    simulation_time_ += dt;
}

