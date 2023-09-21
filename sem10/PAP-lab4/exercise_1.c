/*****************************************************
    AUTHOR  : Sébastien Valat
    MAIL    : sebastien.valat@univ-grenoble-alpes.fr
    LICENSE : BSD
    YEAR    : 2021
    COURSE  : Parallel Algorithms and Programming
*****************************************************/

//////////////////////////////////////////////////////
//
//
// GOAL: Implement a 1D communication scheme along
//       X axis with blocking communications.
//
// SUMMARY:
//     - 1D splitting along X
//     - Blocking communications
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex1(lbm_comm_t * comm, int total_width, int total_height)
{
	//get infos
	int rank;
	int comm_size;
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &comm_size );

	// calculate the number of tasks along X axis and Y axis.
	comm->nb_x = comm_size;
	comm->nb_y = 1;

	// calculate the current task position in the splitting
	comm->rank_x = rank;
	comm->rank_y = 0;

	// calculate the local sub-domain size (do not forget the 
	// ghost cells). Use total_width & total_height as starting 
	// point.
	comm->width = total_width / comm_size + 2;
	comm->height = total_height + 2;

	// calculate the absolute position in the global mesh.
	// without accounting the ghost cells
	// (used to setup the obstable & initial conditions).
	comm->x = total_width / comm_size * rank;
	comm->y = 0;

	//if debug print comm
	#ifndef NDEBUG
	lbm_comm_print( comm );
	#endif
}

/****************************************************/
void lbm_comm_ghost_exchange_ex1(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	int tag = 0;

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1) {
		MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD);
		MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD);
}
