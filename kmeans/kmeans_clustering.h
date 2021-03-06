#ifndef C_CLUSTERING_KMEANS_CLUSTERING_H
#define C_CLUSTERING_KMEANS_CLUSTERING_H

/* kmeans_clustering.c */
void kmeans_clustering(float **, int, int, int, float, int *);

float euclid_dist_2(float *, float *, int);

int find_nearest_point(float *, int, float **, int);

#endif //C_CLUSTERING_KMEANS_CLUSTERING_H
