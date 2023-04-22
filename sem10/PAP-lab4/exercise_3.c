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

	int communicate_right = comm->rank_x != comm->nb_x - 1;
	int communicate_left = comm->rank_x != 0;

	if (communicate_right) {
		MPI_Isend(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, comm->requests);
		MPI_Irecv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, comm->requests + 1);
	}
	if (communicate_left) {
		MPI_Isend(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, comm->requests + 2);
		MPI_Irecv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, comm->requests + 3);
	}

	if (communicate_right && !communicate_left) MPI_Waitall(2, comm->requests, MPI_STATUS_IGNORE);
	else if (communicate_left && !communicate_right) MPI_Waitall(2, comm->requests + 2, MPI_STATUS_IGNORE);
	else if (communicate_right && communicate_left) MPI_Waitall(4, comm->requests, MPI_STATUSES_IGNORE);
}
