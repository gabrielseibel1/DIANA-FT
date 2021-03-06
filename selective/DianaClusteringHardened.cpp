#include "DianaClusteringHardened.h"
extern "C" {
    #include "kmeans_clustering_hardened.h"
}
#include <cmath>
#include <cstdlib>
#include "HardeningUtils.h"

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

    int n_features_1 = n_features, n_features_2 = n_features;

    //iterate over the levels of the dendrogram while not all clusters are unitary
    bool there_was_a_cluster_split; //condition to stop algorithm
    int level = 1; //1 (not 0) because father cluster has already been inserted
    do {
        there_was_a_cluster_split = false;
        int n_clusters_in_anterior_level = dendrogram->countClustersInLevel(level-1);
        int n_clusters_in_anterior_level_1 = n_clusters_in_anterior_level;
        int n_clusters_in_anterior_level_2 = n_clusters_in_anterior_level;

        //for each big cluster (of the anterior level) build two smaller clusters
        int cluster_to_divide_index_1 = 0, cluster_to_divide_index_2 = 0;
        while (assertEqual(cluster_to_divide_index_1, cluster_to_divide_index_2) &&
               assertEqual(n_clusters_in_anterior_level_1, n_clusters_in_anterior_level_2) &&
               cluster_to_divide_index_1 < n_clusters_in_anterior_level_1)
        {
            assertEqual(cluster_to_divide_index_1, cluster_to_divide_index_2);
            cluster_t* cluster_to_divide = dendrogram->getCluster(level - 1, cluster_to_divide_index_1, cluster_to_divide_index_2);

            if (cluster_to_divide && cluster_to_divide->size > 1) { //no need to split a cluster that has only one element

                //retrieve attributes from each point of the cluster to be split
                assertEqual(n_features_1, n_features_2);
                float** points_with_attributes_from_cluster_to_divide = dendrogram->getPointsInCluster(cluster_to_divide, all_points, n_features_1);
                //get list of points that belong to new clusters
                assertEqual(n_features_1, n_features_2);
                int *points_membership = membershipFromKmeans(points_with_attributes_from_cluster_to_divide,
                                                              n_features_1,
                                                              cluster_to_divide->size,
                                                              2, /*split in two new clusters*/
                                                              threshold);

                there_was_a_cluster_split = dendrogram->splitCluster(level, points_membership, cluster_to_divide) || there_was_a_cluster_split;

                free(points_membership);
                free(points_with_attributes_from_cluster_to_divide[0]);
                free(points_with_attributes_from_cluster_to_divide);
            }
            cluster_to_divide_index_1++; cluster_to_divide_index_2++;
        }
        level = dendrogram->getLevels();

        //print_dendrogram();
    } while (/*ORDER OF OPERANDS HERE IS IMPORTANT!*/there_was_a_cluster_split/* && !are_all_clusters_in_level_unitary(level)*/);

    dendrogram->updateIds();
}
