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
//       8 neighbors using manual copy for non
//       contiguous side and blocking communications
//
// SUMMARY:
//     - 2D splitting along X and Y
//     - 8 neighbors communications
//     - Blocking communications
//     - Manual copy for non continguous cells
//
//////////////////////////////////////////////////////

/****************************************************/
#include "src/lbm_struct.h"
#include "src/exercises.h"

/****************************************************/
void lbm_comm_init_ex4(lbm_comm_t * comm, int total_width, int total_height)
{
	int comm_size;
	MPI_Comm_size( MPI_COMM_WORLD, &comm_size );

	int flat = 2;
	int dimensions[] = {0, 0};
	int periods[] = {0, 0};
	int reorder = 0;

	MPI_Dims_create(comm_size, flat, dimensions);
    MPI_Cart_create(MPI_COMM_WORLD, flat, dimensions, periods, reorder, &comm->communicator);

	int rank;
	int coords[] = {-1, -1};

	MPI_Comm_rank(comm->communicator, &rank);
	MPI_Cart_coords(comm->communicator, rank, flat, coords);

	// calculate the number of tasks along X axis and Y axis.
	comm->nb_x = dimensions[0];
	comm->nb_y = dimensions[1];

	// calculate the current task position in the splitting
	comm->rank_x = coords[0];
	comm->rank_y = coords[1];

	// calculate the local sub-domain size (do not forget the 
	// ghost cells). Use total_width & total_height as starting 
	// point.
	comm->width = total_width / dimensions[0] + 2;
	comm->height = total_height / dimensions[1] + 2;

	// calculate the absolute position  (in cell number) in the global mesh.
	// without accounting the ghost cells
	// (used to setup the obstable & initial conditions).
	comm->x = total_width / dimensions[0] * coords[0];
	comm->y = total_height / dimensions[1] * coords[1];

	// if you want to avoid allocating temporary copy buffer
	// for every step :
	comm->buffer_recv_down = malloc(comm->width * DIRECTIONS * sizeof(double));
	comm->buffer_recv_up = malloc(comm->width * DIRECTIONS * sizeof(double));
	comm->buffer_send_down = malloc(comm->width * DIRECTIONS * sizeof(double));
	comm->buffer_send_up = malloc(comm->width * DIRECTIONS * sizeof(double));

	//if debug print comm
	#ifndef NDEBUG
	lbm_comm_print(comm);
	#endif
}

/****************************************************/
void lbm_comm_release_ex4(lbm_comm_t * comm)
{
	free(comm->buffer_recv_down);
	free(comm->buffer_recv_up);
	free(comm->buffer_send_down);
	free(comm->buffer_send_up);
}

static int get_neighbor_rank(lbm_comm_t* comm, int x, int y) {
	int current = comm->rank_x * comm->nb_y + comm->rank_y;
	return current + x * comm->nb_y + y;
}

void load_buffer(double* buffer, lbm_comm_t* comm, lbm_mesh_t *mesh, int y) {
	for (int i = 0; i < comm->width; i++) 
		for (int j = 0; j < DIRECTIONS; j++)
			buffer[i * DIRECTIONS + j] = *(lbm_mesh_get_cell(mesh, i, y) + j);
}

void unload_buffer(double* buffer, lbm_comm_t* comm, lbm_mesh_t *mesh, int y) {
	for (int i = 0; i < comm->width; i++)
		for (int j = 0; j < DIRECTIONS; j++)
			*(lbm_mesh_get_cell(mesh, i, y) + j) = buffer[i * DIRECTIONS + j];
}

/****************************************************/
void lbm_comm_ghost_exchange_ex4(lbm_comm_t * comm, lbm_mesh_t * mesh)
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

	if (comm->rank_y != 0) {
		MPI_Recv(comm->buffer_recv_down, comm->width * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 0, -1), tag, comm->communicator, MPI_STATUS_IGNORE);
		unload_buffer(comm->buffer_recv_down, comm, mesh, 0);
	}
	if (comm->rank_y != comm->nb_y - 1) {
		load_buffer(comm->buffer_send_down, comm, mesh, comm->height - 2);
		MPI_Send(comm->buffer_send_down, comm->width * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 0, 1), tag, comm->communicator);
		MPI_Recv(comm->buffer_recv_up, comm->width * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 0, 1), tag, comm->communicator, MPI_STATUS_IGNORE);
		unload_buffer(comm->buffer_recv_up, comm, mesh, comm->height - 1);
	}
	if (comm->rank_y != 0) {
		load_buffer(comm->buffer_send_up, comm, mesh, 1);
		MPI_Send(comm->buffer_send_up, comm->width * DIRECTIONS, MPI_DOUBLE, get_neighbor_rank(comm, 0, -1), tag, comm->communicator);
	}

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
