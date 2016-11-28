/*
  Copyright (c)  2013  Mikael Leetmaa
  Copyright (c)  2016-2019 ShaoZhengjiang

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  mpiroutines.cpp
 *  \brief File for the implementation code of the mpi utility functions.
 */


#include "mpiroutines.h"


// -------------------------------------------------------------------------- //
//
void distributeToAll(int & data,
                     const MPI::Intracomm & comm)
{
#if RUNMPI == true

    // Only one integer is sent.
    const int size = 1;

    // This is master communicating.
    const int root = 0;

    // Send and recieve.
    comm.Bcast(&data,         // The send and recieve buffer.
               size,          // The number of elements to communicate.
               MPI_INT,       // The type of data.
               root);         // The sender (master).

    // Done.
#endif
}


// -------------------------------------------------------------------------- //
//
void sumOverProcesses(int & data,
                      const MPI::Intracomm & comm)
{
#if RUNMPI == true
    const int size = 1;

    // Copy the data over to the send buffer.
    int send = data;

    comm.Allreduce(&send,       // Send buffer.
                   &data,       // Recieve buffer (overwrite)
                   size,        // Size of the buffers.
                   MPI_INT,     // Data type.
                   MPI_SUM);    // Operation to perform.
    // Done.
#endif
}

// -------------------------------------------------------------------------- //
//
void sumOverProcesses(std::vector<int> & data,
                      const MPI::Intracomm & comm)
{
#if RUNMPI == true
    const int size = data.size();

    // Copy the data over to the send buffer.
    std::vector<int> send(data);

    comm.Allreduce(&send[0],    // Send buffer.
                   &data[0],    // Recieve buffer (overwrite)
                   size,        // Size of the buffers.
                   MPI_INT,     // Data type.
                   MPI_SUM);    // Operation to perform.
    // Done.
#endif
}


// -------------------------------------------------------------------------- //
//
void sumOverProcesses(std::vector<double> & data,
                      const MPI::Intracomm & comm)
{
#if RUNMPI == true
    const int size = data.size();

    // Copy the data over to the send buffer.
    std::vector<double> send(data);

    comm.Allreduce(&send[0],    // Send buffer.
                   &data[0],    // Recieve buffer (overwrite)
                   size,        // Size of the buffers.
                   MPI_DOUBLE,  // Data type.
                   MPI_SUM);     // Operation to perform.
    // Done.
#endif
}


// -------------------------------------------------------------------------- //
//
std::vector< std::pair<int,int> > determineChunks(const int mpi_size,
                                                  const int vector_size)
{
    // Determine how many elements to take at minimum.
    const int take = vector_size/mpi_size;

    // Take the modulus to get the rest.
    const int rest = vector_size%mpi_size;

    // Calculate everyones chunk sizes.
    std::vector< std::pair<int,int> > chunks(mpi_size);

    int sum = 0;
    for (int i = 0; i < mpi_size; ++i)
    {

        // This is the take of process i including its part of the rest.
        const int i_take = (i < rest) ? (take + 1) : take;

        chunks[i].first  = sum;
        chunks[i].second = i_take;
        sum += i_take;
    }

    // Done.
    return chunks;
}

