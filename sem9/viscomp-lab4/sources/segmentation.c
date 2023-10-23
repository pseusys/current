#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "segmentation.h"
#include "imagine.h"


typedef struct {
    void *centroids;
    int* association;
} clusts;

void free_clusts(clusts* clts) {
    free(clts->centroids);
    free(clts->association);
    free(clts);
}


int KMEAN_MAX_ITERATIONS = 500;

/**
 * GENERATES: return value
*/
clusts* kmeans(const void* base, int items, int point_size, int clusters, const void* maxval, double (*distance)(const void*, const void*, const void*), void (*mean)(const void*, int, const int*, void*, int)) {
    void* centroids = malloc(clusters * point_size);
    void* prev_centroids = calloc(clusters, point_size);
    for (int i = 0; i < clusters; i++) {
        void* current = (void*) (((size_t) centroids) + i * point_size);
        void* random = (void*) (((size_t) base) + (rand() % items) * point_size);
        memcpy(current, random, point_size);
    }

    int iteration = 0;
    int* point_association = malloc(items * sizeof(int));
    while (iteration < KMEAN_MAX_ITERATIONS && memcmp(centroids, prev_centroids, clusters * point_size) != 0) {

        // Associate every point with the closest centroid.
        for (int i = 0; i < items; i++) {
            int closest_centroid = 0;
            void* current = (void*) ((size_t) base + i * point_size);
            double min_distance = distance(current, centroids, maxval);
            for (int j = 1; j < clusters; j++) {
                void* centroid = (void*) ((size_t) centroids + j * point_size);
                double centroid_distance = distance(current, centroid, maxval);
                if (centroid_distance < min_distance) {
                    min_distance = centroid_distance;
                    closest_centroid = j;
                }
            }
            point_association[i] = closest_centroid;
        }

        // Push current centroids to previous.
        memcpy(prev_centroids, centroids, clusters * point_size);

        // Recalculate centroid centers.
        mean(base, items, point_association, centroids, clusters);

        // Increase iteration number.
        iteration++;
    }

    free(prev_centroids);

    clusts* clts = (clusts*) malloc(sizeof(clusts));
    clts->centroids = centroids;
    clts->association = point_association;
    return clts;
}


// This segmentation uses (i) as image pixel

double intencity_distance(const void* a, const void* b, const void* maxval) {
    return abs(*((byte*) a) - *((byte*) b));
}

void intencity_mean(const void* base, int items, const int* association, void* centroids, int clusters) {
    int* intensity_weights = calloc(clusters, sizeof(int));
    long long int* intensity_sums = calloc(clusters, sizeof(long long int));

    for (int i = 0; i < items; i++) {
        intensity_weights[association[i]]++;
        intensity_sums[association[i]] += ((byte*) base)[i];
    }

    for (int i = 0; i < clusters; i++)
        if (intensity_weights[i] > 0)
            ((byte*) centroids)[i] = (byte) (intensity_sums[i] / intensity_weights[i]);

    free(intensity_weights);
    free(intensity_sums);
}

void intencity_segmentation(int clusters, int maxval, int length, byte* source) {
    clusts* clts = kmeans(source, length, sizeof(byte), clusters, &maxval, &intencity_distance, &intencity_mean);
    for (int i = 0; i < length; i++) source[i] = ((byte*) clts->centroids)[clts->association[i]];
    free_clusts(clts);
}


// This segmentation uses (i, x, y) as image pixel

typedef struct {
    byte payload;
    int x, y;
} byte_x_y;

double intencity_location_distance(const void* a, const void* b, const void* maxval) {
    double i_d_n = (double) abs(((byte_x_y*) a)->payload - ((byte_x_y*) b)->payload) / ((byte_x_y*) maxval)->payload;  // Intensity delta normalized
    double x_d_n = abs(((byte_x_y*) a)->x - ((byte_x_y*) b)->x) / ((byte_x_y*) maxval)->x;  // 'x' coordinate delta normalized
    double y_d_n = abs(((byte_x_y*) a)->y - ((byte_x_y*) b)->y) / ((byte_x_y*) maxval)->y;  // 'y' coordinate delta normalized
    return sqrt(i_d_n * i_d_n + x_d_n * x_d_n + y_d_n * y_d_n);
}

void intencity_location_mean(const void* base, int items, const int* association, void* centroids, int clusters) {    
    int* intensity_weights = calloc(clusters, sizeof(int));
    long long int* intensity_sums = calloc(clusters, sizeof(long long int));
    long long int* x_coord_sums = calloc(clusters, sizeof(long long int));
    long long int* y_coord_sums = calloc(clusters, sizeof(long long int));

    for (int i = 0; i < items; i++) {
        intensity_weights[association[i]]++;
        intensity_sums[association[i]] += ((byte_x_y*) base)[i].payload;
        x_coord_sums[association[i]] += ((byte_x_y*) base)[i].x;
        y_coord_sums[association[i]] += ((byte_x_y*) base)[i].y;
    }

    for (int i = 0; i < clusters; i++)
        if (intensity_weights[i] > 0) {
            ((byte_x_y*) centroids)[i].payload = (byte) (intensity_sums[i] / intensity_weights[i]);
            ((byte_x_y*) centroids)[i].x = (byte) (x_coord_sums[i] / intensity_weights[i]);
            ((byte_x_y*) centroids)[i].y = (byte) (y_coord_sums[i] / intensity_weights[i]);
        }

    free(intensity_weights);
    free(intensity_sums);
    free(x_coord_sums);
    free(y_coord_sums);
}

void intencity_location_segmentation(int clusters, int maxval, int width, int height, byte* source) {
    byte_x_y maximum = { .payload = maxval, .x = width, .y = height };

    int length = width * height;
    byte_x_y* coordinated = (byte_x_y*) malloc(length * sizeof(byte_x_y));
    for (int i = 0; i < length; i++) {
        coordinated[i].payload = source[i];
        coordinated[i].x = i % width;
        coordinated[i].y = i / width;
    }

    clusts* clts = kmeans(coordinated, length, sizeof(byte_x_y), clusters, &maximum, &intencity_location_distance, &intencity_location_mean);
    for (int i = 0; i < length; i++) source[i] = ((byte_x_y*) clts->centroids)[clts->association[i]].payload;
    free_clusts(clts);

    free(coordinated);
}


// This segmentation uses (r, g, b) as image pixel

double rgb_distance(const void* a, const void* b, const void* maxval) {
    double rgb_d[3];  // 'r', 'g' and 'b' value deltas
    for (int i = 0; i < RGB_TRIPLET; i++)
        rgb_d[i] = (byte) abs(((byte*) a)[i] - ((byte*) b)[i]);
    return sqrt(rgb_d[0] * rgb_d[0] + rgb_d[1] * rgb_d[1] + rgb_d[2] * rgb_d[2]);
}

void rgb_mean(const void* base, int items, const int* association, void* centroids, int clusters) {    
    int* intensity_weights = calloc(clusters, sizeof(int));
    long long int* color_sums = calloc(clusters * RGB_TRIPLET, sizeof(long long int));

    for (int i = 0; i < items; i++) {
        intensity_weights[association[i]]++;
        for (int j = 0; j < RGB_TRIPLET; j++)
            color_sums[association[i] * RGB_TRIPLET + j] += ((byte*) base)[i * RGB_TRIPLET + j];
    }

    for (int i = 0; i < clusters; i++)
        if (intensity_weights[i] > 0)
            for (int j = 0; j < RGB_TRIPLET; j++)
                ((byte*) centroids)[i * RGB_TRIPLET + j] = (byte) (color_sums[i * RGB_TRIPLET + j] / intensity_weights[i]);

    free(intensity_weights);
    free(color_sums);
}

void rgb_segmentation(int clusters, int maxval, int length, byte* source) {
    clusts* clts = kmeans(source, length, sizeof(byte) * RGB_TRIPLET, clusters, &maxval, &rgb_distance, &rgb_mean);
    for (int i = 0; i < length; i++)
        for (int j = 0; j < RGB_TRIPLET; j++) 
            source[i * RGB_TRIPLET + j] = ((byte*) clts->centroids)[clts->association[i] * RGB_TRIPLET + j];
    free_clusts(clts);
}


// This segmentation uses (r, g, b, x, y) as image pixel

typedef struct {
    byte rgb[3];
    int x, y;
} rgb_x_y;

double rgb_location_distance(const void* a, const void* b, const void* maxval) {
    double rgb_d_n[3];  // 'r', 'g' and 'b' value deltas normalized
    for (int i = 0; i < RGB_TRIPLET; i++)
        rgb_d_n[i] = (double) abs(((rgb_x_y*) a)->rgb[i] - ((rgb_x_y*) b)->rgb[i]) / ((byte_x_y*) maxval)->payload;
    double x_d_n = abs(((rgb_x_y*) a)->x - ((rgb_x_y*) b)->x) / ((byte_x_y*) maxval)->x;  // 'x' coordinate delta normalized
    double y_d_n = abs(((rgb_x_y*) a)->y - ((rgb_x_y*) b)->y) / ((byte_x_y*) maxval)->y;  // 'y' coordinate delta normalized
    return sqrt(rgb_d_n[0] * rgb_d_n[0] + rgb_d_n[1] * rgb_d_n[1] + rgb_d_n[2] * rgb_d_n[2] + x_d_n * x_d_n + y_d_n * y_d_n);
}

void rgb_location_mean(const void* base, int items, const int* association, void* centroids, int clusters) {    
    int* intensity_weights = calloc(clusters, sizeof(int));
    long long int* color_sums = calloc(clusters * RGB_TRIPLET, sizeof(long long int));
    long long int* x_coord_sums = calloc(clusters, sizeof(long long int));
    long long int* y_coord_sums = calloc(clusters, sizeof(long long int));

    for (int i = 0; i < items; i++) {
        intensity_weights[association[i]]++;
        for (int j = 0; j < RGB_TRIPLET; j++)
            color_sums[association[i] * RGB_TRIPLET + j] += ((rgb_x_y*) base)[i].rgb[j];
        x_coord_sums[association[i]] += ((rgb_x_y*) base)[i].x;
        y_coord_sums[association[i]] += ((rgb_x_y*) base)[i].y;
    }

    for (int i = 0; i < clusters; i++)
        if (intensity_weights[i] > 0) {
            for (int j = 0; j < RGB_TRIPLET; j++)
                ((rgb_x_y*) centroids)[i].rgb[j] = (byte) (color_sums[i * RGB_TRIPLET + j] / intensity_weights[i]);
            ((rgb_x_y*) centroids)[i].x = (byte) (x_coord_sums[i] / intensity_weights[i]);
            ((rgb_x_y*) centroids)[i].y = (byte) (y_coord_sums[i] / intensity_weights[i]);
        }

    free(intensity_weights);
    free(color_sums);
    free(x_coord_sums);
    free(y_coord_sums);
}

void rgb_location_segmentation(int clusters, int maxval, int width, int height, byte* source) {
    byte_x_y maximum = { .payload = maxval, .x = width, .y = height };

    int length = width * height;
    rgb_x_y* coordinated = (rgb_x_y*) malloc(length * sizeof(rgb_x_y));
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < RGB_TRIPLET; j++)
            coordinated[i].rgb[j] = source[i * RGB_TRIPLET + j];
        coordinated[i].x = i % width;
        coordinated[i].y = i / width;
    }

    clusts* clts = kmeans(coordinated, length, sizeof(rgb_x_y), clusters, &maximum, &rgb_location_distance, &rgb_location_mean);
    for (int i = 0; i < length; i++)
        for (int j = 0; j < RGB_TRIPLET; j++)
            source[i * RGB_TRIPLET + j] = ((rgb_x_y*) clts->centroids)[clts->association[i]].rgb[j];
    free_clusts(clts);

    free(coordinated);
}
