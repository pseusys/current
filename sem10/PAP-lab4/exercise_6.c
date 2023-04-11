/*****************************************************
    AUTHOR  : Sébastien Valat
    MAIL    : sebastien.valat@univ-grenoble-alpes.fr
    LICENSE : BSD
    YEAR    : 2021
    COURSE  : Parallel Algorithms and Programming
*****************************************************/

//////////////////////////////////////////////////////
//
// Goal: Implement 2D grid communication with non-blocking
//       messages.
//
// SUMMARY:
//     - 2D splitting along X and Y
//     - 8 neighbors communications
//     - MPI type for non contiguous cells
// NEW:
//     - Non-blocking communications
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex6(lbm_comm_t * comm, int total_width, int total_height)
{
	//we use the same implementation than ex5
	lbm_comm_init_ex5(comm, total_width, total_height);
}

/****************************************************/
void lbm_comm_release_ex6(lbm_comm_t * comm)
{
	//we use the same implementation than ext 5
	lbm_comm_release_ex5(comm);
}

static int get_neighbor_rank(lbm_comm_t* comm, int x, int y) {
	int current = comm->rank_x * comm->nb_y + comm->rank_y;
	return current + x * comm->nb_y + y;
}

/****************************************************/
void lbm_comm_ghost_exchange_ex6(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	int tag = 0;

	// LEFT <-> RIGHT

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	int communicate_right = comm->rank_x != comm->nb_x - 1;
	int communicate_left = comm->rank_x != 0;

	if (communicate_right) {
		MPI_Isend(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 0), tag, comm->communicator, comm->requests);
		MPI_Irecv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 0), tag, comm->communicator, comm->requests);
	}
	if (communicate_left) {
		MPI_Isend(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 0), tag, comm->communicator, comm->requests + 1);
		MPI_Irecv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 0), tag, comm->communicator, comm->requests + 1);
	}

	// TOP <-> BOTTOM

	double* send_up = lbm_mesh_get_cell(mesh, 0, 1);
	double* recv_up = lbm_mesh_get_cell(mesh, 0, comm->height - 1);
	double* send_down = lbm_mesh_get_cell(mesh, 0, comm->height - 2);
	double* recv_down = lbm_mesh_get_cell(mesh, 0, 0);

	int communicate_up = comm->rank_y != comm->nb_y - 1;
	int communicate_down = comm->rank_y != 0;

	if (communicate_up) {
		MPI_Isend(send_down, 1, comm->type, get_neighbor_rank(comm, 0, 1), tag, comm->communicator, comm->requests + 2);
		MPI_Irecv(recv_up, 1, comm->type, get_neighbor_rank(comm, 0, 1), tag, comm->communicator, comm->requests + 2);
	}
	if (communicate_down) {
		MPI_Isend(send_up, 1, comm->type, get_neighbor_rank(comm, 0, -1), tag, comm->communicator, comm->requests + 3);
		MPI_Irecv(recv_down, 1, comm->type, get_neighbor_rank(comm, 0, -1), tag, comm->communicator, comm->requests + 3);
	}

	// DIAGONALS (TOP LEFT) <-> DIAGONALS (BOTTOM RIGHT)

	int communicate_left_up = comm->rank_x != comm->nb_x - 1 && comm->rank_y != comm->nb_y - 1;
	int communicate_right_down = comm->rank_x != 0 && comm->rank_y != 0;

	if (communicate_left_up) {
		MPI_Isend(lbm_mesh_get_cell(mesh, comm->width - 2, comm->height - 2), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 1), tag, comm->communicator, comm->requests + 4);
		MPI_Irecv(lbm_mesh_get_cell(mesh, comm->width - 1, comm->height - 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 1), tag, comm->communicator, comm->requests + 4);
	}
	if (communicate_right_down) {
		MPI_Isend(lbm_mesh_get_cell(mesh, 1, 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, -1), tag, comm->communicator, comm->requests + 5);
		MPI_Irecv(lbm_mesh_get_cell(mesh, 0, 0), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, -1), tag, comm->communicator, comm->requests + 5);
	}

	// DIAGONALS (BOTTOM LEFT) <-> DIAGONALS (TOP RIGHT)

	int communicate_left_down = comm->rank_x != comm->nb_x - 1 && comm->rank_y != 0;
	int communicate_right_up = comm->rank_x != 0 && comm->rank_y != comm->nb_y - 1;

	if (communicate_left_down) {
		MPI_Isend(lbm_mesh_get_cell(mesh, comm->width - 2, 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, -1), tag, comm->communicator, comm->requests + 6);
		MPI_Irecv(lbm_mesh_get_cell(mesh, comm->width - 1, 0), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, -1), tag, comm->communicator, comm->requests + 6);
	}
	if (communicate_right_up) {
		MPI_Isend(lbm_mesh_get_cell(mesh, 1, comm->height - 2), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 1), tag, comm->communicator, comm->requests + 7);
		MPI_Irecv(lbm_mesh_get_cell(mesh, 0, comm->height - 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 1), tag, comm->communicator, comm->requests + 7);
	}

	// WAIT ALL

	if (communicate_right && !communicate_left) MPI_Wait(comm->requests, MPI_STATUS_IGNORE);
	else if (communicate_left && !communicate_right) MPI_Wait(comm->requests + 1, MPI_STATUS_IGNORE);
	else if (communicate_right && communicate_left) MPI_Waitall(2, comm->requests, MPI_STATUSES_IGNORE);

	if (communicate_up && !communicate_down) MPI_Wait(comm->requests + 2, MPI_STATUS_IGNORE);
	else if (communicate_down && !communicate_up) MPI_Wait(comm->requests + 3, MPI_STATUS_IGNORE);
	else if (communicate_up && communicate_down) MPI_Waitall(2, comm->requests + 2, MPI_STATUSES_IGNORE);

	if (communicate_left_up && !communicate_right_down) MPI_Wait(comm->requests + 4, MPI_STATUS_IGNORE);
	else if (communicate_right_down && !communicate_left_up) MPI_Wait(comm->requests + 5, MPI_STATUS_IGNORE);
	else if (communicate_left_up && communicate_right_down) MPI_Waitall(2, comm->requests + 4, MPI_STATUSES_IGNORE);

	if (communicate_left_down && !communicate_right_up) MPI_Wait(comm->requests + 6, MPI_STATUS_IGNORE);
	else if (communicate_right_up && !communicate_left_down) MPI_Wait(comm->requests + 7, MPI_STATUS_IGNORE);
	else if (communicate_left_down && communicate_right_up) MPI_Waitall(2, comm->requests + 6, MPI_STATUSES_IGNORE);
}
