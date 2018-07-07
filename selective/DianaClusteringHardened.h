//
// Created by gabriel on 6/12/18.
//

#ifndef DIANA_DIANA_CLUSTERING_HARDENED_H
#define DIANA_DIANA_CLUSTERING_HARDENED_H

#include "DendrogramHardened.h"

class DianaClusteringHardened {
public:
    DendrogramHardened *dendrogram;

    explicit DianaClusteringHardened(int nPoints);

    void cluster(float **all_points, int n_features, float threshold);

    int *membershipFromKmeans(float **points, int n_features, int n_points, int k, float threshold);
};


#endif //DIANA_DIANA_CLUSTERING_HARDENED_H
