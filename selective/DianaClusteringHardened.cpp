#include "DianaClusteringHardened.h"
extern "C" {
    #include "../kmeans/kmeans_clustering.h"
}
#include <cmath>
#include <cstdlib>

DianaClusteringHardened::DianaClusteringHardened(int nPoints){
    srand(7);
    dendrogram = new DendrogramHardened(nPoints);
}

int *DianaClusteringHardened::membershipFromKmeans(float **points, int n_features, int n_points, int k, float threshold) {
    int *membership;

    membership = (int *) malloc(n_points * sizeof(int));

    srand(7);
    /* perform regular Kmeans */
    kmeans_clustering(points, n_features, n_points, k, threshold, membership);

    return membership;
}

/*----< diana_clustering() >---------------------------------------------*/
void DianaClusteringHardened::cluster(float **all_points,    /* in: [n_points][n_features] */
                              int n_features, float threshold) {

    //iterate over the levels of the dendrogram while not all clusters are unitary
    bool there_was_a_cluster_split; //condition to stop algorithm
    int level = 1; //1 (not 0) because father cluster has already been inserted
    do {
        there_was_a_cluster_split = false;
        int n_clusters_in_anterior_level = dendrogram->countClustersInLevel(level-1);

        //for each big cluster (of the anterior level) build two smaller clusters
        for (int cluster_to_divide_index = 0; cluster_to_divide_index < n_clusters_in_anterior_level; ++cluster_to_divide_index) {

            cluster_t* cluster_to_divide = dendrogram->getCluster(level - 1, cluster_to_divide_index);

            if (cluster_to_divide && cluster_to_divide->size > 1) { //no need to split a cluster that has only one element

                //retrieve attributes from each point of the cluster to be split
                float** points_with_attributes_from_cluster_to_divide = dendrogram->getPointsInCluster(cluster_to_divide, all_points, n_features);
                //get list of points that belong to new clusters
                int *points_membership = membershipFromKmeans(points_with_attributes_from_cluster_to_divide,
                                                                n_features,
                                                                cluster_to_divide->size,
                                                                2, /*split in two new clusters*/
                                                                threshold);

                there_was_a_cluster_split = dendrogram->splitCluster(level, points_membership, cluster_to_divide) || there_was_a_cluster_split;

                free(points_membership);
                free(points_with_attributes_from_cluster_to_divide[0]);
                free(points_with_attributes_from_cluster_to_divide);
            }
        }

        level = dendrogram->getLevels();

        //print_dendrogram();
    } while (/*ORDER OF OPERANDS HERE IS IMPORTANT!*/there_was_a_cluster_split/* && !are_all_clusters_in_level_unitary(level)*/);

    dendrogram->updateIds();
}
