//
// Created by gabriel on 6/12/18.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "Dendrogram.h"

Dendrogram::Dendrogram(int father_cluster_size) {
    cluster_t *father_cluster = (cluster_t *) malloc(sizeof(cluster_t));
    father_cluster->size = father_cluster_size;
    father_cluster->points = (int *) malloc(sizeof(int) * father_cluster_size);
    for (int point = 0; point < father_cluster_size; ++point) {
        father_cluster->points[point] = point;
    }
    father_cluster->father_cluster = nullptr; //there is none
    father_cluster->next_cluster = nullptr; //there is none
    father_cluster->left_child = nullptr; //will be allocated later
    father_cluster->right_child = nullptr; //will be allocated later
    father_cluster->father_id = 0; //there is none
    father_cluster->brother_id = 0; //there is none

    clusters.insert(std::make_pair(0, father_cluster));
    ids[nullptr] = 0;
}

int Dendrogram::getClusterId(cluster_t *cluster_ptr) {
    auto it = ids.find(cluster_ptr);

    if (it == ids.end()) {

        ids[cluster_ptr] = static_cast<int>(ids.size());
        return static_cast<int>(ids.size());

    } else {
        return it->second;
    }
}

void
Dendrogram::splitPoints(int **points_1, int **points_2, int *points_1_size, int *points_2_size, int *points_membership,
                        int n_points, int *list_of_points) {
    //count how many points for each cluster
    (*points_1_size) = (*points_2_size) = 0;
    for (int i = 0; i < n_points; ++i) {
        switch (points_membership[i]) {
            default:
            case 0:
                (*points_1_size) = (*points_1_size) + 1;
                break;
            case 1:
                (*points_2_size) = (*points_2_size) + 1;
                break;
        }
    }

    //allocate two lists of points
    *points_1 = (int *) malloc(sizeof(int) * (*points_1_size));
    *points_2 = (int *) malloc(sizeof(int) * (*points_2_size));

    int points_1_index = 0;
    for (int point = 0; point < n_points; ++point) {
        if (points_membership[point] == 0) {
            (*points_1)[points_1_index] = list_of_points[point];
            ++points_1_index;
        }
    }

    int points_2_index = 0;
    for (int point = 0; point < n_points; ++point) {
        if (points_membership[point] == 1) {
            (*points_2)[points_2_index] = list_of_points[point];
            ++points_2_index;
        }
    }
}

int Dendrogram::areAllClustersInLevelUnitary(int level) {
    std::map<int, cluster_t *>::iterator it = clusters.find(level);

    if (it == clusters.end()) { //could not find level
        std::cout << "Could not find level " << level << "\n";
        return false;
    } else { //found level
        cluster_t *cluster = it->second;
        bool all_are_unitary = true;
        while (all_are_unitary && cluster) {
            all_are_unitary = all_are_unitary && (cluster->size == 1);
            cluster = cluster->next_cluster;
        }
        return all_are_unitary;
    }
}

int Dendrogram::getLevels() {
    clusters.size();
}

int Dendrogram::splitCluster(int level, int *points_membership, cluster_t *original_cluster) {
    int *points_1 = nullptr, *points_2 = nullptr;
    int points_1_size, points_2_size;
    splitPoints(&points_1, &points_2, &points_1_size, &points_2_size, points_membership, original_cluster->size,
                original_cluster->points);

    //if kmeans said all points are classified as same cluster, there is no need to split the original cluster in 2!!!
    if (points_1_size <= 0 || points_2_size <= 0) return false;

    auto *cluster_1 = (cluster_t *) malloc(sizeof(cluster_t));
    cluster_1->points = points_1;
    cluster_1->size = points_1_size;
    cluster_1->father_cluster = original_cluster;
    cluster_1->left_child = nullptr;
    cluster_1->right_child = nullptr;

    auto *cluster_2 = (cluster_t *) malloc(sizeof(cluster_t));
    cluster_2->points = points_2;
    cluster_2->size = points_2_size;
    cluster_2->father_cluster = original_cluster;
    cluster_2->left_child = nullptr;
    cluster_2->right_child = nullptr;

    //chain "brothers"
    cluster_1->next_cluster = cluster_2;
    cluster_2->next_cluster = nullptr;

    //attach "brothers" to father
    original_cluster->left_child = cluster_1;
    original_cluster->right_child = cluster_2;

    auto it = clusters.find(level);

    if (it == clusters.end()) { //could not find level - create new level and associate cluster to it
        clusters.insert(std::make_pair(level, cluster_1));
    } else { //found level - get last cluster and append new clusters to it
        cluster_t *preceding_cluster = it->second;
        while (preceding_cluster->next_cluster)
            preceding_cluster = preceding_cluster->next_cluster;
        preceding_cluster->next_cluster = cluster_1;
    }
    return true;
}

int Dendrogram::countClustersInLevel(int level) {
    cluster_t *cluster = clusters.find(level)->second;
    int count = 0;
    while (cluster) {
        ++count;
        cluster = cluster->next_cluster;
    }

    return count;
}


cluster_t *Dendrogram::getCluster(int level, int cluster_index) {
    cluster_t *cluster = clusters.find(level)->second;
    for (int i = 0; i < cluster_index; ++i) {
        if (!cluster) {
            if (i != cluster_index - 1) {
              fprintf(stderr, "Unreachable cluster_index %d in level %d! Stopped at index %d\n",
                      cluster_index, level, i);
              exit(EXIT_FAILURE);
            } else {
                break;
            }
        }
        cluster = cluster->next_cluster;
    }
    return cluster;
}

float **Dendrogram::getPointsInCluster(cluster_t *cluster, float **points, int n_features) {
    //allocate space for returned matrix
    float **points_in_cluster = (float **) malloc(cluster->size * sizeof(float *));
    points_in_cluster[0] = (float *) calloc(static_cast<size_t>(cluster->size), n_features * sizeof(float));
    for (int i = 1; i < cluster->size; i++)
        points_in_cluster[i] = points_in_cluster[i - 1] + n_features;

    for (int point_index = 0; point_index < cluster->size; ++point_index) {
        for (int feature_index = 0; feature_index < n_features; ++feature_index) {
            points_in_cluster[point_index][feature_index] = points[cluster->points[point_index]][feature_index];
        }
    }
    return points_in_cluster;
}

void Dendrogram::printCluster(cluster_t *cluster) {
    std::cout << "CLUSTER #" << getClusterId(cluster) << " {\n";
    std::cout << "\t\tpoints -> { ";
    for (int i = 0; i < cluster->size; ++i) {
        std::cout << cluster->points[i];
        if (i + 1 < cluster->size) std::cout << ", ";
    }
    std::cout << " }\n";
    std::cout << "\t\tfather -> #" << getClusterId(cluster->father_cluster) << "\n";
    std::cout << "\t\tbrother (next) -> #" << getClusterId(cluster->next_cluster) << "\n";
    std::cout << "\t\tleft child -> #" << getClusterId(cluster->left_child) << "\n";
    std::cout << "\t\tright child -> #" << getClusterId(cluster->right_child) << "\n";
    std::cout << "\t}\n";
}

void Dendrogram::print() {
    std::cout << "\n\nDENDROGRAM:\n";

    auto it = clusters.begin();
    while (it != clusters.end()) {
        std::cout << "LEVEL " << it->first << " {\n";
        cluster_t *cluster = it->second;
        do {
            std::cout << "\t";
            printCluster(cluster);
        } while ((cluster = cluster->next_cluster) != nullptr);
        std::cout << "}\n";
        ++it;
    }
}

int Dendrogram::toTextFile(char *filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "DENDROGRAM:\n";
        file << "LEVELS -> " << clusters.size() << "\n";
        auto it = clusters.begin();
        while (it != clusters.end()) {
            file << "LEVEL " << it->first << " {\n";
            cluster_t *cluster = it->second;
            do {
                file << "\t";

                { //cluster to file
                    file << "CLUSTER #" << cluster->id << " {\n";
                    file << "\t\tfather -> #" << cluster->father_id << "\n";
                    file << "\t\tbrother (next) -> #" << cluster->brother_id << "\n";
                    file << "\t\tleft child -> #" << cluster->left_child_id << "\n";
                    file << "\t\tright child -> #" << cluster->right_child_id << "\n";
                    file << "\t\tsize -> " << cluster->size << "\n";
                    file << "\t\tpoints -> { ";
                    for (int i = 0; i < cluster->size; ++i) {
                        file << cluster->points[i];
                        if (i + 1 < cluster->size) file << ", ";
                    }
                    file << " }\n";
                    file << "\t}\n";
                }

            } while ((cluster = cluster->next_cluster) != nullptr);
            file << "}\n";
            ++it;
        }

        file.close();
        return 0;

    } else {
        std::cout << "FAILED TO OPEN " << filename << "\n";
        return -1;
    }
}

int Dendrogram::toBinaryFile(char *filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        auto dendroIt = clusters.begin();
        int levels = (int) clusters.size();
        file.write((char *) &levels, sizeof(int)); //levels
        while (dendroIt != clusters.end()) {
            int level = dendroIt->first;
            cluster_t *cluster = dendroIt->second;

            file.write((char *) &level, sizeof(int)); //level
            do {
                //cluster to file
                /*int cluster_id = getClusterId(cluster);
                    int father_id = getClusterId(cluster->father_cluster);
                    int brother_id = getClusterId(cluster->next_cluster);
                    int left_child_id = getClusterId(cluster->left_child);
                    int right_child_id = getClusterId(cluster->right_child);*/

                file.write((char *) &cluster->size, sizeof(int)); //cluster size
                file.write((char *) &cluster->id, sizeof(int)); //cluster id
                file.write((char *) &cluster->father_id, sizeof(int)); //father's id
                file.write((char *) &cluster->brother_id, sizeof(int)); //brother's id
                file.write((char *) &cluster->left_child_id, sizeof(int)); //left child's id
                file.write((char *) &cluster->right_child_id, sizeof(int)); //right child's id
                file.write((char *) cluster->points, cluster->size * sizeof(int)); //points

            } while ((cluster = cluster->next_cluster) != nullptr);
            ++dendroIt;
        }

        file.close();
        return 0;

    } else {
        std::cout << "FAILED TO OPEN " << filename << "IN BINARY MODE\n";
        return -1;
    }
}

int Dendrogram::fromBinaryFile(char *filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        int levels;
        file.read((char *) &levels, sizeof(int)); //read levels
        for (int l = 0; l < levels; ++l) {
            int level;
            file.read((char *) &level, sizeof(int)); //level

            cluster_t *first_cluster = nullptr;
            cluster_t *cluster = nullptr;
            cluster_t *prev_cluster = nullptr;
            do {
                cluster = (cluster_t *) malloc(sizeof(cluster_t));
                cluster->next_cluster = nullptr;
                if (first_cluster == nullptr) first_cluster = cluster;

                file.read((char *) &cluster->size, sizeof(int)); //cluster size
                file.read((char *) &cluster->id, sizeof(int)); //cluster id
                file.read((char *) &cluster->father_id, sizeof(int)); //father's id
                file.read((char *) &cluster->brother_id, sizeof(int)); //brother's id
                file.read((char *) &cluster->left_child_id, sizeof(int)); //left child's id
                file.read((char *) &cluster->right_child_id, sizeof(int)); //right child's id

                cluster->points = (int *) malloc(cluster->size * sizeof(int));
                file.read((char *) cluster->points, cluster->size * sizeof(int)); //points

                if (prev_cluster != nullptr) { //requirement for reconstructing the dendrogram
                    prev_cluster->next_cluster = cluster;
                }
                prev_cluster = cluster;
            } while (cluster->brother_id != 0);

            clusters.insert(std::make_pair(level, first_cluster));
        }

        file.close();
        return 0;

    } else {
        std::cout << "FAILED TO OPEN " << filename << " IN BINARY MODE [" << strerror(errno) << "]\n";
        return -1;
    }
}

void Dendrogram::catalogIds() {
    auto dendroIt = clusters.begin();
    while (dendroIt != clusters.end()) {
        cluster_t *cluster = dendroIt->second;
        do {
            getClusterId(cluster);
            getClusterId(cluster->father_cluster);
            getClusterId(cluster->next_cluster);
            getClusterId(cluster->left_child);
            getClusterId(cluster->right_child);

        } while ((cluster = cluster->next_cluster) != nullptr);
        ++dendroIt;
    }

    int lastId = -1;
    for (auto idsIt = ids.begin(); idsIt != ids.end(); ++idsIt) {
        int id = idsIt->second;
        if (id != lastId + 1) {
            idsIt->second = lastId + 1;
        }
        lastId ++;
    }
}

void Dendrogram::updateIds() {
    //catalogIds();

    auto dendroIt = clusters.begin();
    while (dendroIt != clusters.end()) {
        cluster_t *cluster = dendroIt->second;
        do {
            cluster->id = getClusterId(cluster);
            cluster->father_id = getClusterId(cluster->father_cluster);
            cluster->brother_id = getClusterId(cluster->next_cluster);
            cluster->left_child_id = getClusterId(cluster->left_child);
            cluster->right_child_id = getClusterId(cluster->right_child);

        } while ((cluster = cluster->next_cluster) != nullptr);
        ++dendroIt;
    }
}

void Dendrogram::printClusterSavedIds(cluster_t *cluster) {
    std::cout << "CLUSTER #" << getClusterId(cluster) << " {\n";
    std::cout << "\t\tpoints -> { ";
    for (int i = 0; i < cluster->size; ++i) {
        std::cout << cluster->points[i];
        if (i + 1 < cluster->size) std::cout << ", ";
    }
    std::cout << " }\n";
    std::cout << "\t\tfather -> #" << cluster->father_id << "\n";
    std::cout << "\t\tbrother (next) -> #" << cluster->brother_id << "\n";
    std::cout << "\t\tleft child -> #" << cluster->left_child_id << "\n";
    std::cout << "\t\tright child -> #" << cluster->right_child_id << "\n";
    std::cout << "\t}\n";
}

void Dendrogram::printWithSavedIds() {
    std::cout << "\n\nDENDROGRAM:\n";

    auto it = clusters.begin();
    while (it != clusters.end()) {
        std::cout << "LEVEL " << it->first << " {\n";
        cluster_t *cluster = it->second;
        do {
            std::cout << "\t";
            printClusterSavedIds(cluster);
        } while ((cluster = cluster->next_cluster) != nullptr);
        std::cout << "}\n";
        ++it;
    }
}

void Dendrogram::printIdsMap() {
    std::cout << "\n\nIDS MAP: ";

    auto it = ids.begin();
    while (it != ids.end()) {
        std::cout << "{" << it->first << "," << it->second << "} , ";
        ++it;
    }

    std::cout << "\n\n";
}

Dendrogram::Dendrogram(char* binary_filename) {
    fromBinaryFile(binary_filename);
}

Dendrogram::~Dendrogram() {
    auto dendroIt = clusters.begin();
    while (dendroIt != clusters.end()) {
        cluster_t *cluster = dendroIt->second;
        do {

            free(cluster->points);
            cluster_t* toFree = cluster;
            cluster = cluster->next_cluster;
            free(toFree);

        } while (cluster != nullptr);
        ++dendroIt;
    }
}
