/*
  This file is part of the KMCLibX project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/* ******************************************************************
 *  file   : exceptions.h
 *  brief  : File for all exceptions class definition in KMCLibX.
 *  author : zjshao
 *  date   : 2016-04-11
 *
 *  history:
 *  <author>   <time>       <version>    <desc>
 *  ------------------------------------------------------
 *  zjshao     2016-04-11   1.2          Initial creation.
 *
 *  ------------------------------------------------------
 * ******************************************************************
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <stdexcept>

/// Exception class for element type error.
class element_type_error : public std::logic_error {

public:
    // Constructor.
    element_type_error(const std::string & msg) :
        std::logic_error(msg)
    {
        // NOTHING HERE
    }

};


/// Exception class for position unmatch.
class coordinates_unmatched_error : public std::logic_error {

public:
    // Inline constructor.
    coordinates_unmatched_error(const std::string & msg) :
        std::logic_error(msg)
    {
        // NOTHING HERE
    }

};

#endif  // EXCEPTIONS_H_
