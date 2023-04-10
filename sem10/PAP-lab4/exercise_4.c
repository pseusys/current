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

int flat = 2;
int dimensions[] = {0, 0};
int periods[] = {0, 0};
int reorder = 0;
int row_split[] = {1, 0};
int col_split[] = {0, 1};

MPI_Comm row_communicator, col_communicator;

/****************************************************/
void lbm_comm_init_ex4(lbm_comm_t * comm, int total_width, int total_height)
{
	int rank;
	int comm_size;
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &comm_size );

	MPI_Dims_create(comm_size, flat, dimensions);
    MPI_Cart_create(MPI_COMM_WORLD, flat, dimensions, periods, reorder, &comm->communicator);

	if (comm->communicator == MPI_COMM_NULL) {
		printf("Process number %d has nothing to do!\n", rank);
		return;
	}

	int row_rank, col_rank;
	int row_size, col_size;

    MPI_Cart_sub(comm->communicator, row_split, &row_communicator);
    MPI_Comm_rank(row_communicator, &row_rank);
	MPI_Comm_size(row_communicator, &row_size);

    MPI_Cart_sub(comm->communicator, col_split, &col_communicator);
    MPI_Comm_rank(col_communicator, &col_rank);
	MPI_Comm_size(col_communicator, &col_size);

	// calculate the number of tasks along X axis and Y axis.
	comm->nb_x = row_size;
	comm->nb_y = col_size;

	// calculate the current task position in the splitting
	comm->rank_x = row_rank;
	comm->rank_y = col_rank;

	// calculate the local sub-domain size (do not forget the 
	// ghost cells). Use total_width & total_height as starting 
	// point.
	comm->width = total_width / row_size + 2;
	comm->height = total_height / col_size + 2;

	// calculate the absolute position  (in cell number) in the global mesh.
	// without accounting the ghost cells
	// (used to setup the obstable & initial conditions).
	comm->x = total_width / row_size * row_rank;
	comm->y = total_height / col_size * col_rank;

	// if you want to avoid allocating temporary copy buffer
	// for every step :
	comm->buffer_recv_down = malloc(comm->width * sizeof(float));
	comm->buffer_recv_up = malloc(comm->width * sizeof(float));
	comm->buffer_send_down = malloc(comm->width * sizeof(float));
	comm->buffer_send_up = malloc(comm->width * sizeof(float));

	//if debug print comm
	#ifndef NDEBUG
	lbm_comm_print(comm);
	#endif
}

/****************************************************/
void lbm_comm_release_ex4(lbm_comm_t * comm)
{
	if (comm->communicator == MPI_COMM_NULL) return;
	free(comm->buffer_recv_down);
	free(comm->buffer_recv_up);
	free(comm->buffer_send_down);
	free(comm->buffer_send_up);
}

void fill_buffer(double* buffer, lbm_mesh_t *mesh, int y) {
	
}

/****************************************************/
void lbm_comm_ghost_exchange_ex4(lbm_comm_t * comm, lbm_mesh_t * mesh)
{
	//
	// TODO: Implement the 2D communication with :
	//         - blocking MPI functions
	//         - manual copy in temp buffer for non contiguous side 
	//
	// To be used:
	//    - DIRECTIONS: the number of doubles composing a cell
	//    - double[9] lbm_mesh_get_cell(mesh, x, y): function to get the address of a particular cell.
	//    - comm->width : The with of the local sub-domain (containing the ghost cells)
	//    - comm->height : The height of the local sub-domain (containing the ghost cells)
	//
	// TIP: create a function to get the target rank from x,y task coordinate. 
	// TIP: You can use MPI_PROC_NULL on borders.
	// TIP: send the corner values 2 times, with the up/down/left/write communication
	//      and with the diagonal communication in a second time, this avoid
	//      special cases for border tasks.

	//example to access cell
	//double * cell = lbm_mesh_get_cell(mesh, local_x, local_y);
	//double * cell = lbm_mesh_get_cell(mesh, comm->width - 1, 0);

	//TODO:
	//   - implement left/write communications
	//   - implement top/bottom communication (non contiguous)
	//   - implement diagonal communications

	if (comm->communicator == MPI_COMM_NULL) return;

	int tag = 0;

	// LEFT-> RIGHT

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, row_communicator, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1) {
		MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, row_communicator);
		MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, row_communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, row_communicator);

	// TOP-> BOTTOM

	double* send_left = lbm_mesh_get_cell(mesh, 1, 0);
	double* recv_left = lbm_mesh_get_cell(mesh, comm->width - 1, 0);
	double* send_right = lbm_mesh_get_cell(mesh, comm->width - 2, 0);
	double* recv_right = lbm_mesh_get_cell(mesh, 0, 0);

	if (comm->rank_x != 0) MPI_Recv(recv_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, row_communicator, MPI_STATUS_IGNORE);
	if (comm->rank_x != comm->nb_x - 1) {
		MPI_Send(send_right, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, row_communicator);
		MPI_Recv(recv_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x + 1, tag, row_communicator, MPI_STATUS_IGNORE);
	}
	if (comm->rank_x != 0) MPI_Send(send_left, comm->height * DIRECTIONS, MPI_DOUBLE, comm->rank_x - 1, tag, row_communicator);
}
