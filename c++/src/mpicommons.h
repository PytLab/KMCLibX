/*
  Copyright (c)  2013  Mikael Leetmaa
  Copyright (c)  2016-2019 ShaoZhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  mpicommons.h
 *  \brief Intrerfaces of the common MPI routines.
 */



#ifndef __MPICOMMONS__
#define __MPICOMMONS__

#include "mpih.h"

/// Struct for handling MPI functions to be wrapped.
struct MPICommons {

    /*! \brief Wrapps MPI_INIT
     */
    static void init();

    /*! \brief Wrap MPI::Is_initialized
     */
    static bool initialized();

    /*! \brief Wrapps MPI_FINALIZE
     */
    static void finalize();

    /*! \brief Wrap MPI::Is_finialized
     */
    static bool finalized();

    /*! \brief Wrapps MPI_COMM_RANK
     *  \param comm: The communicator to use.
     *  \return: The rank of this process withing the given communicator.
     */
    static int myRank(const MPI::Intracomm & comm=MPI::COMM_WORLD);

    /*! \brief Wrapps MPI_COMM_SIZE
     *  \param comm: The communicator to use.
     *  \return: The sise of the communicator (the total number of processes).
     */
    static int size(const MPI::Intracomm & comm=MPI::COMM_WORLD);

    /*! \brief Wrapps MPI_BARRIER, syncronizing processes.
     *  \param comm: The communicator to use.
     */
    static void barrier(const MPI::Intracomm & comm=MPI::COMM_WORLD);

    /*! \brief Returns true if the calling process is the master.
     */
    static bool isMaster(const MPI::Intracomm & comm=MPI::COMM_WORLD)
    { return (myRank(comm) == 0); }

};


#endif // __MPICOMMONS__

