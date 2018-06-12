//
// Created by gabriel on 6/12/18.
//

#ifndef DIANA_DIANACLUSTERING_H
#define DIANA_DIANACLUSTERING_H


#include "Dendrogram.h"

class DianaClustering {
public:
    Dendrogram *dendrogram;

    explicit DianaClustering(int nPoints);

    void doDianaClustering(float **all_points, int n_features, float threshold);

    int *membershipFromKmeans(float **points, int n_features, int n_points, int k, float threshold);
};


#endif //DIANA_DIANACLUSTERING_H
