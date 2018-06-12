//
// Created by gabriel on 6/12/18.
//

//
// Created by gabriel on 6/10/18.
//

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include "DianaClustering.h"

#define MAX_ITER 500
#define TIMING

#ifdef LOGS
#include "../log_helper.h"
#endif

#ifdef TIMING
long long timing_get_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000000) + tv.tv_usec;
    }

    long long setup_start, setup_end;
    long long loop_start, loop_end;
    long long kernel_start, kernel_end;
    long long check_start, check_end;
#endif

typedef struct input_data {
    float **attributes;
    int num_objects;
    int num_attributes;
} input_data_t;

void usage(char *argv0) {
    const char *help =
            "Usage: %s [switches]\n"
                    "       -i input-filename   : file containing data to be clustered\n"
                    "       -o output-filename  : file containing the output result\n"
                    "       -g golden-filename  : file containing the golden result"
                    "       -t threshold		: kmeans threshold value\n"
                    "       -n no. of threads	: number of threads\n";
    fprintf(stderr, help, argv0);
    exit(-1);
}

input_data_t *getInputData(char *filename) {
    auto *inputData = (input_data_t *) (malloc(sizeof(input_data_t)));
    inputData->attributes = nullptr;
    inputData->num_attributes = 0;
    inputData->num_objects = 0;

    float *buf;
    char line[1024];
    int i, j;

    FILE *infile;
    if ((infile = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        exit(1);
    }
    while (fgets(line, 1024, infile) != NULL)
        if (strtok(line, " \t\n") != 0)
            inputData->num_objects++;
    rewind(infile);
    while (fgets(line, 1024, infile) != NULL) {
        if (strtok(line, " \t\n") != 0) {
            /* ignore the id (first attribute): numAttributes = 1; */
            while (strtok(NULL, " ,\t\n") != NULL)
                inputData->num_attributes++;
            break;
        }
    }

    /* allocate space for attributes[] and read attributes of all objects */
    buf = (float *) malloc(inputData->num_objects * inputData->num_attributes * sizeof(float));
    inputData->attributes = (float **) malloc(inputData->num_objects * sizeof(float *));
    inputData->attributes[0] = (float *) malloc(inputData->num_objects * inputData->num_attributes * sizeof(float));
    for (i = 1; i < inputData->num_objects; i++)
        inputData->attributes[i] = inputData->attributes[i - 1] + inputData->num_attributes;
    rewind(infile);
    i = 0;
    while (fgets(line, 1024, infile) != NULL) {
        if (strtok(line, " \t\n") == NULL) continue;
        for (j = 0; j < inputData->num_attributes; j++) {
            buf[i] = atof(strtok(NULL, " ,\t\n"));
            i++;
        }
    }
    fclose(infile);

    memcpy(inputData->attributes[0], buf, inputData->num_objects * inputData->num_attributes * sizeof(float));

    return inputData;
}

void flagIfDifferent(int a, int b, int level, int cluster_count, int *errors, char *description) {
    if (a != b) {
        printf("SDC: [L:%d, N:%d] %s -> OUT: %d , GOLD: %d\n",
               level, cluster_count, description, a, b);
        ++(*errors);
    }
}

int compare_out_and_gold(char *out_filename, char *gold_filename) {
    int errors = 0;

    auto *dendrogramOut = new Dendrogram(out_filename);
    auto *dendrogramGold = new Dendrogram(gold_filename);

    //retrieve level count
    int levels_out = dendrogramOut->getLevels();
    int levels_gold = dendrogramGold->getLevels();

    //check equal level count
    if (levels_out != levels_gold) {
        printf("SDC: Levels -> OUT: %d , GOLD: %d\n", levels_out, levels_gold);
        ++errors;
    }

    auto iter_out = dendrogramOut->clusters.begin();
    auto iter_gold = dendrogramGold->clusters.begin();

    //here we loop for "the smallest level" times, so all dendrogram operations are safe (all levels exist).
    //if the sizes are the same, compares all levels
    int smallest_level = (levels_gold <= levels_out) ? levels_gold : levels_out;
    for (int level = 0; level < smallest_level; ++level) {
        cluster_t *cluster_out = iter_out->second;
        cluster_t *cluster_gold = iter_gold->second;

        int cluster_count = 0;
        bool clusters_are_not_nullptr = (cluster_out != nullptr) && (cluster_gold != nullptr);
        while (clusters_are_not_nullptr) {

            //check equal cluster size
            flagIfDifferent(cluster_out->size, cluster_gold->size,
                            level, cluster_count, &errors, (char *)("Cluster size"));
            //check equal id
            flagIfDifferent(cluster_out->id, cluster_gold->id,
                            level, cluster_count, &errors, (char *) "Cluster id");
            //check equal father id
            flagIfDifferent(cluster_out->father_id, cluster_gold->father_id,
                            level, cluster_count, &errors, (char *) "Father id");
            //check equal brother id
            flagIfDifferent(cluster_out->brother_id, cluster_gold->brother_id,
                            level, cluster_count, &errors, (char *) "Brother id");
            //check equal left child id
            flagIfDifferent(cluster_out->left_child_id, cluster_gold->left_child_id,
                            level, cluster_count, &errors, (char *) "Left Child id");
            //check equal right child id
            flagIfDifferent(cluster_out->right_child_id, cluster_gold->right_child_id,
                            level, cluster_count, &errors, (char *) "Right Child id");

            //check equal points. Here we loop for "the smallest cluster size" times,
            // so all points[i] operations are safe. If the sizes are the same, compares all points
            int smallest_size = (cluster_gold->size <= cluster_out->size) ? cluster_gold->size : cluster_out->size;
            for (int p = 0; p < smallest_size; ++p) {
                if (cluster_out->points[p] != cluster_gold->points[p]) {
                    printf("SDC: [L:%d, N:%d, P:%d] Point -> OUT: %d , GOLD: %d\n",
                           level, cluster_count, p, cluster_out->points[p], cluster_gold->points[p]);
                    ++errors;
                }
            }

            //proceed to next clusters
            cluster_out = cluster_out->next_cluster;
            cluster_gold = cluster_gold->next_cluster;
            ++cluster_count;

            //detect different number of clusters in the level
            clusters_are_not_nullptr = (cluster_out != nullptr) && (cluster_gold != nullptr);
            if (!clusters_are_not_nullptr) {
                if ((cluster_out != nullptr) && (cluster_gold == nullptr)){
                    printf("SDC: [L:%d] Cluster count -> OUT: >=%d , GOLD: %d\n",
                           level, levels_out, levels_gold);
                    ++errors;
                }
                if ((cluster_out == nullptr) && (cluster_gold != nullptr)) {
                    printf("SDC: [L:%d] Cluster count -> OUT: %d , GOLD: >=%d\n",
                           level, levels_out, levels_gold);
                    ++errors;
                }
            }
        }
        ++iter_out;
        ++iter_gold;
        ++level;
    }

    return errors;
}

int num_omp_threads = 1;

/**
 * High level wrapper that loops diana kernel, comparison and logging
 */
int main(int argc, char **argv) {

    int opt;
    char *input_filename = nullptr;
    char *output_filename = nullptr;
    char *golden_filename = nullptr;
    float kmeans_threshold = 0.001;

    while ((opt = getopt(argc, argv, "i:o:g:t:n:?")) != EOF) {
        switch (opt) {
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'g':
                golden_filename = optarg;
                break;
            case 't':
                kmeans_threshold = atof(optarg);
                break;
            case 'n':
                num_omp_threads = atoi(optarg);
                break;
            case '?':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    #ifdef TIMING
        setup_start = timing_get_time();
    #endif
    #ifdef LOGS
        set_iter_interval_print(10);
        char test_info[200];
        snprintf(test_info, 200, "size:%d omp_num_threads:%d", size, omp_num_threads);
        start_log_file("openmpQuicksort", test_info);
    #endif

    //read data to input data struct
    input_data_t *input_data = getInputData(input_filename);

    #ifdef TIMING
        setup_end = timing_get_time();
    #endif

    //loop indefinitely
    for (int i = 0; i < MAX_ITER; ++i) {
        #ifdef TIMING
            loop_start = timing_get_time();
        #endif


        #ifdef TIMING
            kernel_start = timing_get_time();
        #endif
        #ifdef LOGS
            start_iteration();
        #endif

        //call diana kernel and write output to file
        DianaClustering dianaClustering(input_data->num_objects);
        dianaClustering.doDianaClustering(input_data->attributes, input_data->num_attributes, kmeans_threshold);
        int output_success = dianaClustering.dendrogram->toBinaryFile(output_filename) >= 0;
        int output_success2 = dianaClustering.dendrogram->toTextFile((char*)"diana_out.txt") >= 0;

        #ifdef LOGS
            end_iteration();
        #endif
        #ifdef TIMING
            kernel_end = timing_get_time();
        #endif


        #ifdef TIMING
            check_start = timing_get_time();
        #endif

        //compare out with gold (output_dendrogram, golden_dendrogram) --> TODO adapt to use log_error_detail(error_detail)
        int detected_sdcs = compare_out_and_gold(output_filename, golden_filename);

        #ifdef LOGS
            log_error_count(detected_sdcs);
        #endif
        #ifdef TIMING
            check_end = timing_get_time();
        #endif

        if (detected_sdcs > 0) {
            std::cout << "Detected " << detected_sdcs << " SDCS\n";
        } else {
            std::cout << ".\n";
        }

        #ifdef TIMING
            loop_end = timing_get_time();
            double setup_timing = (double) (setup_end - setup_start) / 1000000;
            double loop_timing = (double) (loop_end - loop_start) / 1000000;
            double kernel_timing = (double) (kernel_end - kernel_start) / 1000000;
            double check_timing = (double) (check_end - check_start) / 1000000;
            printf("\n\tTIMING:\n");
            printf("setup: %f\n",setup_timing);
            printf("loop: %f\n",loop_timing);
            printf("kernel: %f\n",kernel_timing);
            printf("check: %f\n",check_timing);
        #endif
    }

    #ifdef LOGS
        end_log_file();
    #endif
}