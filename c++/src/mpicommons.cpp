/*
  Copyright (c)  2013  Mikael Leetmaa
  Copyright (c)  2016-2019 ShaoZhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  mpicommons.cpp
 *  \brief File for the implementation code of the common MPI routines.
 */


#include "mpicommons.h"

bool inited__    = false;
bool finalized__ = false;

// -----------------------------------------------------------------------------
//
void MPICommons::init()
{
   if (inited__)
    {
        return;
    }

    // Switch for using MPI.
#if RUNMPI == true
    // Dummy args.
    int argc = 0;
    char** argv;

    // Make the init call.
    MPI::Init(argc, argv);

    // Set the flag to prevent further calls.
    inited__ = true;
#endif
}


// -----------------------------------------------------------------------------
//
void MPICommons::finalize()
{
    if (finalized__)
    {
        return;
    }

#if RUNMPI == true
    MPI::Finalize();
    finalized__ = true;
#endif
}


// -----------------------------------------------------------------------------
//
int MPICommons::myRank(const MPI::Intracomm comm)
{
#if RUNMPI == true
    int rank;
    //MPI_Comm_rank( comm, &rank );
    rank = comm.Get_rank();
    return rank;
#else
    return 0;
#endif
}


// -----------------------------------------------------------------------------
//
int MPICommons::size(const MPI::Intracomm comm)
{
#if RUNMPI == true
    int size;
    //MPI_Comm_size( comm, &size );
    size = comm.Get_size();
    return size;
#else
    return 1;
#endif
}


// -----------------------------------------------------------------------------
//
void MPICommons::barrier(const MPI::Intracomm comm)
{
#if RUNMPI == true
    MPI_Barrier(comm);
#endif
}

