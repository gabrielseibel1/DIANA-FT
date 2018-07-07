//
// Created by gabriel on 6/12/18.
//

#ifndef DIANA_DENDROGRAM_HARDENED_H
#define DIANA_DENDROGRAM_HARDENED_H


#include <map>
#include <vector>

typedef struct cluster {
    int size;
    int *points;
    struct cluster *father_cluster;
    struct cluster *next_cluster;
    struct cluster *left_child;
    struct cluster *right_child;

    //mocks for file io
    int id;
    int father_id;
    int brother_id;
    int left_child_id;
    int right_child_id;
} cluster_t;

class DendrogramHardened {
private:
    std::map<cluster_t*, int> ids;

    void printCluster(cluster_t *cluster);

    int getClusterId(cluster_t* cluster);
    void splitPoints(int **points_1, int **points_2, int *points_1_size, int *points_2_size, int *points_membership,
                      int n_points, int *list_of_points);
    void printClusterSavedIds(cluster_t *cluster);

    void catalogIds();
public:
    virtual ~DendrogramHardened();

    explicit DendrogramHardened(char *binary_filename);

    explicit DendrogramHardened(int father_cluster_size);

    void print();
    void printWithSavedIds();

    int toBinaryFile(char *filename);
    int toTextFile(char *filename);
    int fromBinaryFile(char *filename);
    int areAllClustersInLevelUnitary(int level);
    int getLevels();
    /**
     * Splits cluster in two smaller ones (if needed).
     * Returns whether there was a split (1) or not (0)
     */
    int splitCluster(int level, int *points_membership, cluster_t *original_cluster);
    cluster_t* getCluster(int level, int cluster_index);
    float **getPointsInCluster(cluster_t* cluster, float **points, int n_features);
    void updateIds();

    std::map<int, cluster_t*> clusters;

    void printIdsMap();

    int countClustersInLevel(int level);

};


#endif //DIANA_DENDROGRAM_HARDENED_H
