/*****************************************************
    AUTHOR  : Sébastien Valat
    MAIL    : sebastien.valat@univ-grenoble-alpes.fr
    LICENSE : BSD
    YEAR    : 2021
    COURSE  : Parallel Algorithms and Programming
*****************************************************/

//////////////////////////////////////////////////////
//
// Goal: Implement 2D grid communication scheme with
//      8 neighbors using MPI types for non contiguous
//      side.
//
// SUMMARY:
//     - 2D splitting along X and Y
//     - 8 neighbors communications
//     - Blocking communications
// NEW:
//     - >>> MPI type for non contiguous cells <<<
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex5(lbm_comm_t * comm, int total_width, int total_height)
{
	//we use the same implementation than ex5 execpt for type creation
	lbm_comm_init_ex4(comm, total_width, total_height);

	MPI_Type_vector(comm->width, DIRECTIONS, comm->height * DIRECTIONS, MPI_DOUBLE, &comm->type);
	MPI_Type_commit(&comm->type);
}

/****************************************************/
void lbm_comm_release_ex5(lbm_comm_t * comm)
{
	//we use the same implementation than ex5 except for type destroy
	lbm_comm_release_ex4(comm);

	MPI_Type_free(&comm->type);
}

static int get_neighbor_rank(lbm_comm_t* comm, int x, int y) {
	int current = comm->rank_x * comm->nb_y + comm->rank_y;
	return current + x * comm->nb_y + y;
}

/****************************************************/
void lbm_comm_ghost_exchange_ex5(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	int tag = 0;

	// LEFT <-> RIGHT

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 0), tag, comm->communicator, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1) {
		MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 0), tag, comm->communicator);
		MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 0), tag, comm->communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 0), tag, comm->communicator);

	// TOP <-> BOTTOM

	double* send_up = lbm_mesh_get_cell(mesh, 0, 1);
	double* recv_up = lbm_mesh_get_cell(mesh, 0, comm->height - 1);
	double* send_down = lbm_mesh_get_cell(mesh, 0, comm->height - 2);
	double* recv_down = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_y != 0) MPI_Recv(recv_down, 1, comm->type, get_neighbor_rank(comm, 0, -1), tag, comm->communicator, MPI_STATUS_IGNORE);
	if (comm->rank_y != comm->nb_y - 1) {
		MPI_Send(send_down, 1, comm->type, get_neighbor_rank(comm, 0, 1), tag, comm->communicator);
		MPI_Recv(recv_up, 1, comm->type, get_neighbor_rank(comm, 0, 1), tag, comm->communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_y != 0) MPI_Send(send_up, 1, comm->type, get_neighbor_rank(comm, 0, -1), tag, comm->communicator);

	// DIAGONALS (TOP LEFT) <-> DIAGONALS (BOTTOM RIGHT)

	if (comm->rank_x != 0 && comm->rank_y != 0) MPI_Recv(lbm_mesh_get_cell(mesh, 0, 0), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, -1), tag, comm->communicator, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1 && comm->rank_y != comm->nb_y - 1) {
		MPI_Send(lbm_mesh_get_cell(mesh, comm->width - 2, comm->height - 2), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 1), tag, comm->communicator);
		MPI_Recv(lbm_mesh_get_cell(mesh, comm->width - 1, comm->height - 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, 1), tag, comm->communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0 && comm->rank_y != 0) MPI_Send(lbm_mesh_get_cell(mesh, 1, 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, -1), tag, comm->communicator);

	// DIAGONALS (BOTTOM LEFT) <-> DIAGONALS (TOP RIGHT)

	if (comm->rank_x != 0 && comm->rank_y != comm->nb_y - 1) MPI_Recv(lbm_mesh_get_cell(mesh, 0, comm->height - 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 1), tag, comm->communicator, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1 && comm->rank_y != 0) {
		MPI_Send(lbm_mesh_get_cell(mesh, comm->width - 2, 1), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, -1), tag, comm->communicator);
		MPI_Recv(lbm_mesh_get_cell(mesh, comm->width - 1, 0), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 1, -1), tag, comm->communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0 && comm->rank_y != comm->nb_y - 1) MPI_Send(lbm_mesh_get_cell(mesh, 1, comm->height - 2), DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, -1, 1), tag, comm->communicator);
}
