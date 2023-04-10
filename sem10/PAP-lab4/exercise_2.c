/*****************************************************
    AUTHOR  : Sébastien Valat
    MAIL    : sebastien.valat@univ-grenoble-alpes.fr
    LICENSE : BSD
    YEAR    : 2021
    COURSE  : Parallel Algorithms and Programming
*****************************************************/

//////////////////////////////////////////////////////
//
// Goal: Implement odd/even 1D blocking communication scheme 
//       along X axis.
//
// SUMMARY:
//     - 1D splitting along X
//     - Blocking communications
// NEW:
//     - >>> Odd/even communication ordering <<<<
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex2(lbm_comm_t * comm, int total_width, int total_height)
{
	//we use the same implementation then ex1
	lbm_comm_init_ex1(comm, total_width, total_height);
}

/****************************************************/
void lbm_comm_ghost_exchange_ex2(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	int tag = 0;

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x % 2 == 1) {
		if (comm->rank_x != comm->nb_x - 1) MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD);
		if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD);
		if (comm->rank_x != comm->nb_x - 1) MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else {
		if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (comm->rank_x != comm->nb_x - 1) MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, MPI_COMM_WORLD);
		if (comm->rank_x != comm->nb_x - 1) MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, MPI_COMM_WORLD);
	}
}
