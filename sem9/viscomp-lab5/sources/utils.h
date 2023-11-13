#ifndef VISCOMP_LAB_5_UTILS_H
#define VISCOMP_LAB_5_UTILS_H

/* Code for M1 Mosig Introduction to Visual Computing, TP 5 */
/* This header contains:
 1. Naive off reader for point cloud from .off file: readOff() 
 2. An auxiliary function that translate the mess center of the point cloud to the origin centerThePCL() 
 3. A function that compute rotation matrix out of rotation angle (yaw, pitch and roll) and translation
 */

/* If you have questions regarding the functions please ask the TA */
/* Detailed information is shown below */

/* This is a customized C structure for storing 3D point information */
struct point3d{
   float  x; // If you want to access x, you need to do p.x, suppose you decleared p as "struct point3d p;"
   float  y; // Similar to above
   float  z;
   int r;
   int g;
   int b;
};

/* The following function reads a point cloud from an off file and returns the point cloud in an array of structre of point3d */
/* The number of points is also return in N */
/* An example to use this function:

#include "imageFormationUtils.h"
int main(int argc, char* argv[])
{
  struct point3d *points;
  int N_v = 0;
  points = readOff(argv[1], &N_v);
  //centerThePCL(points, N_v);
} 
 */
struct point3d *readOff(char *fileName, int *N);


/* The following function translates the mess center of the point cloud to the origin. 
The translated point cloud is returned in points.
In oder to use this function you need also to tell it how many points are there in the cloud using N.
An example of using it can be seen in previous example (the commented line).*/
void centerThePCL(struct point3d *points, int N);

/* The following function provides you a transformation matrix given three rotation angle and translation value */
/* It is using yaw, pitch and roll convention. In case you don't know much about 3d rotation, you can go to rotation matrix under Wikipedia.*/
/* Gama, beta alpha are roll, pitch and yaw angles in arc respectively. */
/* T_x, T_y and T_z are translation in those three directions */
/* The tranformation matrix is stored and returned in result, which is a 1D array containing 16 float numbers */
/* Row major is used in result, i.e.:
			/ result[0]  result[1]  result[2]  result[3]  \
	TM=	| result[4]  result[5]  result[6]  result[7]  |
  		| result[8]  result[9]  result[10] result[11] |
			\ result[12] result[13] result[14] result[15] /
 */
/* Example of using it:
#include "imageFormationUtils.h"
int main(int argc, char* argv[])
{
  float TransMatrix[16];
  computeTrans(0.1, 1.74, 0.2, 0, 0.52, 0.15, TransMatrix);
}
*/
void computeTrans(float gama, float beta, float alpha, float T_x, float T_y, float T_z, float *result);

#endif // VISCOMP_LAB_5_UTILS_H
