/*
  Copyright (c)  2012-2015  Mikael Leetmaa
  Copyright (c)  2016-2019  Shao Zhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : random.h
 *  brief  : File for the random number generator interface.
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------------------
 *  zjshao     2016-11-22   1.4          Add random select method.
 *
 *  ------------------------------------------------------------------
 * ******************************************************************
 */

#ifndef __RANDOM__
#define __RANDOM__

#include <algorithm>
#include <vector>

// Forward declarations.
class Process;

/// The supported random number generator types.
enum RNG_TYPE {MT, MINSTD, RANLUX24, RANLUX48, DEVICE};


/*! \brief Set the type of random number generator to use.
 *  \param rng_type : The type of random number generator to use.
 */
bool setRngType(const RNG_TYPE rng_type);


/*! \brief Set the seed to the random number generator.
 *  \param time_seed : If true the random number generator will be seeded with the
 *                     given seed value plus the present time.
 *  \param seed : An integer to use as seed.
 */
void seedRandom(const bool time_seed, int seed);


/*! \brief Get a pseudo random number between 0.0 and 1.0 using the
 *         std::mt19937 random number generator.
 *  \return : A pseudo random number on the interval (0.0,1.0)
 */
double randomDouble01();


/*! \brief Function to shuffle a integer vector.
 *  \param v: The integer vector to be shuffled.
 */
void shuffleIntVector(std::vector<int> & v);

/*! \brief Function to shuffle a Process pointer vector.
 *  \param v: The Process pointer vector to be shuffled.
 */
void shuffleProcessPtrVector(std::vector<Process *> & v);


/*! \brief Function template to pick an elements from container randomly.
 */
template<typename Iterator, typename RandomGenerator>
Iterator randomPick(Iterator begin, Iterator end, RandomGenerator & g)
{
    std::uniform_int_distribution<> dis(0, std::distance(begin, end) - 1);
    std::advance(begin, dis(g));
    return begin;
}


/*! \brief Function to pick an integer from an integer vector.
 *  \param v: The integer vector from which the integer is picked.
 */
int randomPickInt(const std::vector<int> & v);

#endif // __RANDOM__

