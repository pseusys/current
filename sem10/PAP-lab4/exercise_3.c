/*****************************************************
    AUTHOR  : Sébastien Valat
    MAIL    : sebastien.valat@univ-grenoble-alpes.fr
    LICENSE : BSD
    YEAR    : 2021
    COURSE  : Parallel Algorithms and Programming
*****************************************************/

//////////////////////////////////////////////////////
//
// Goal: Implement non-blocking 1D communication scheme
//       along X axis.
//
// SUMMARY:
//     - 1D splitting along X
// NEW:
//     - >>> Non-blocking communications <<<
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex3(lbm_comm_t * comm, int total_width, int total_height)
{
	//we use the same implementation then ex1
	lbm_comm_init_ex1(comm, total_width, total_height);
}

/****************************************************/
void lbm_comm_ghost_exchange_ex3(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	int tag = 0;

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x != comm->nb_x - 1) {
		MPI_Isend(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, comm->requests);
		MPI_Irecv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, comm->requests);
	}
	if (comm->rank_x != 0) {
		MPI_Isend(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, comm->requests + 1);
		MPI_Irecv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, comm->requests + 1);
	}

	if (comm->rank_x == comm->nb_x - 1) MPI_Wait(comm->requests + 1, MPI_STATUS_IGNORE);
	else if (comm->rank_x == 0) MPI_Wait(comm->requests, MPI_STATUS_IGNORE);
	else MPI_Waitall(2, comm->requests, MPI_STATUSES_IGNORE);
}
