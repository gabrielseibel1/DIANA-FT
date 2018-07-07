//
// Created by gabriel on 6/28/18.
//

#include "DendrogramComparator.h"

bool DendrogramComparator::areEqual(Dendrogram *dendrograms[2]) {
    //retrieve level count
    int levelCount[2] = {
            dendrograms[0]->getLevels(),
            dendrograms[1]->getLevels()
    };

    //check different level count
    if (levelCount[0] != levelCount[1])
        return false;

    //get iterators for each dendrogram
    std::map<int, cluster *>::iterator iterators[2] = {
            dendrograms[0]->clusters.begin(),
            dendrograms[1]->clusters.begin(),
    };

    //here we loop for "the smallest level" times, so all dendrogram operations are safe (all levels exist).
    //if the sizes are the same, compares all levels
    int smallestLevel = (levelCount[0] <= levelCount[1]) ? levelCount[0] : levelCount[1];
    for (int level = 0; level < smallestLevel; ++level) {

        //clusters of each dendrogram
        cluster_t *clusters[2] = {
                iterators[0]->second,
                iterators[1]->second
        };

        int clusterCount = 0;
        bool clustersNotNullptr = (clusters[0] != nullptr) && (clusters[1] != nullptr);
        while (clustersNotNullptr) {

            //check different cluster size
            if (clusters[0]->size != clusters[1]->size)
                return false;

            //check different id
            if (clusters[0]->id != clusters[1]->id)
                return false;

            //check different father id
            if (clusters[0]->father_id != clusters[1]->father_id)
                return false;

            //check different brother id
            if (clusters[0]->brother_id != clusters[1]->brother_id)
                return false;

            //check different left child id
            if (clusters[0]->left_child_id != clusters[1]->left_child_id)
                return false;

            //check different right child id
            if (clusters[0]->right_child_id != clusters[1]->right_child_id)
                return false;

            //check difference in points. here we loop for "the smallest cluster size" times,
            //so all points[i] operations are safe (if the sizes are the same, compares all points)
            int smallestSize = (clusters[1]->size <= clusters[0]->size) ? clusters[1]->size : clusters[0]->size;
            for (int p = 0; p < smallestSize; ++p) {
                if (clusters[0]->points[p] != clusters[1]->points[p])
                    return false;
            }

            //proceed to next clusters
            clusters[0] = clusters[0]->next_cluster;
            clusters[1] = clusters[1]->next_cluster;
            ++clusterCount;

            //detect different number of clusters in the level
            clustersNotNullptr = (clusters[0] != nullptr) && (clusters[1] != nullptr);
            if (!clustersNotNullptr) {
                if ((clusters[0] != nullptr) && (clusters[1] == nullptr))
                    return false;
                if ((clusters[0] == nullptr) && (clusters[1] != nullptr))
                    return false;
            }
        }
        ++iterators[0];
        ++iterators[1];
        ++level;
    }

    //no differences found, must be equal!
    return true;
}


int DendrogramComparator::countDifferences(Dendrogram *dendrograms[2]) {
    int errors = 0;

    //retrieve level count
    int levelCount[2] = {
            dendrograms[0]->getLevels(),
            dendrograms[1]->getLevels()
    };

    //check different level count
    if (levelCount[0] != levelCount[1]) {
        ++errors;
    }

    //get iterators for each dendrogram
    std::map<int, cluster *>::iterator iterators[2] = {
            dendrograms[0]->clusters.begin(),
            dendrograms[1]->clusters.begin(),
    };

    //here we loop for "the smallest level" times, so all dendrogram operations are safe (all levels exist).
    //if the sizes are the same, compares all levels
    int smallestLevel = (levelCount[0] <= levelCount[1]) ? levelCount[0] : levelCount[1];
    for (int level = 0; level < smallestLevel; ++level) {

        //clusters of each dendrogram
        cluster_t *clusters[2] = {
                iterators[0]->second,
                iterators[1]->second
        };

        int clusterCount = 0;
        bool clustersNotNullptr = (clusters[0] != nullptr) && (clusters[1] != nullptr);
        while (clustersNotNullptr) {

            //check different cluster size
            if (clusters[0]->size != clusters[1]->size) ++errors;

            //check different id
            if (clusters[0]->id != clusters[1]->id) ++errors;

            //check different father id
            if (clusters[0]->father_id != clusters[1]->father_id) ++errors;

            //check different brother id
            if (clusters[0]->brother_id != clusters[1]->brother_id) ++errors;

            //check different left child id
            if (clusters[0]->left_child_id != clusters[1]->left_child_id) ++errors;

            //check different right child id
            if (clusters[0]->right_child_id != clusters[1]->right_child_id) ++errors;

            //check difference in points. here we loop for "the smallest cluster size" times,
            //so all points[i] operations are safe (if the sizes are the same, compares all points)
            int smallestSize = (clusters[1]->size <= clusters[0]->size) ? clusters[1]->size : clusters[0]->size;
            for (int p = 0; p < smallestSize; ++p) {
                if (clusters[0]->points[p] != clusters[1]->points[p]) ++errors;
            }

            //proceed to next clusters
            clusters[0] = clusters[0]->next_cluster;
            clusters[1] = clusters[1]->next_cluster;
            ++clusterCount;

            //detect different number of clusters in the level
            clustersNotNullptr = (clusters[0] != nullptr) && (clusters[1] != nullptr);
            if (!clustersNotNullptr) {
                if ((clusters[0] != nullptr) && (clusters[1] == nullptr)) {
                    ++errors;
                }
                if ((clusters[0] == nullptr) && (clusters[1] != nullptr)) {
                    ++errors;
                }
            }
        }
        ++iterators[0];
        ++iterators[1];
        ++level;
    }

    return errors;
}
