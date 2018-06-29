/*****************************************************************************/
/*IMPORTANT:  READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.         */
/*By downloading, copying, installing or using the software you agree        */
/*to this license.  If you do not agree to this license, do not download,    */
/*install, copy or use the software.                                         */
/*                                                                           */
/*                                                                           */
/*Copyright (c) 2005 Northwestern University                                 */
/*All rights reserved.                                                       */

/*Redistribution of the software in source and binary forms,                 */
/*with or without modification, is permitted provided that the               */
/*following conditions are met:                                              */
/*                                                                           */
/*1       Redistributions of source code must retain the above copyright     */
/*        notice, this list of conditions and the following disclaimer.      */
/*                                                                           */
/*2       Redistributions in binary form must reproduce the above copyright   */
/*        notice, this list of conditions and the following disclaimer in the */
/*        documentation and/or other materials provided with the distribution.*/
/*                                                                            */
/*3       Neither the name of Northwestern University nor the names of its    */
/*        contributors may be used to endorse or promote products derived     */
/*        from this software without specific prior written permission.       */
/*                                                                            */
/*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS    */
/*IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED      */
/*TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT AND         */
/*FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL          */
/*NORTHWESTERN UNIVERSITY OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,       */
/*INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES          */
/*(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR          */
/*SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)          */
/*HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,         */
/*STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    */
/*ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             */
/*POSSIBILITY OF SUCH DAMAGE.                                                 */
/******************************************************************************/

/*************************************************************************/
/**   File:         example.c                                           **/
/**   Description:  Takes as input a file:                              **/
/**                 ascii  file: containing 1 data point per line       **/
/**                 binary file: first int is the number of objects     **/
/**                              2nd int is the no. of features of each **/
/**                              object                                 **/
/**                 This example performs a fuzzy c-means clustering    **/
/**                 on the data. Fuzzy clustering is performed using    **/
/**                 min to max clusters and the clustering that gets    **/
/**                 the best score according to a compactness and       **/
/**                 separation criterion are returned.                  **/
/**   Author:  Wei-keng Liao                                            **/
/**            ECE Department Northwestern University                   **/
/**            email: wkliao@ece.northwestern.edu                       **/
/**                                                                     **/
/**   Edited by: Jay Pisharath                                          **/
/**              Northwestern University.                               **/
/**                                                                     **/
/**   ================================================================  **/
/**																		**/
/**   Edited by: Sang-Ha  Lee											**/
/**				 University of Virginia									**/
/**																		**/
/**   Description:	No longer supports fuzzy c-means clustering;	 	**/
/**					only regular k-means clustering.					**/
/**					Simplified for main functionality: regular k-means	**/
/**					clustering.											**/
/**                                                                     **/
/*************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <omp.h>

#include "Dendrogram.h"
#include "DianaClustering.h"

extern double wtime(void);

int num_omp_threads = 1;

/*---< usage() >------------------------------------------------------------*/
void usage(char *argv0) {
    char *help = (char *)
            "Usage: %s [switches] -i filename\n"
                    "       -i filename     		: file containing data to be clustered\n"
                    "       -o output-filename     		: file containing the output result\n"
                    "       -b                 	: input file is in binary format\n"
                    "       -k                 	: number of clusters (default is 5) \n"
                    "       -t threshold		: threshold value\n"
                    "       -n no. of threads	: number of threads\n";
    fprintf(stderr, help, argv0);
    exit(-1);
}

/*---< main() >-------------------------------------------------------------*/
int main(int argc, char **argv) {
    printf("\n\n^^^^^^^^^^^^^^^^^^^^^^^^^ START OF DIANA ^^^^^^^^^^^^^^^^^^^^^^^^^\n");

    int opt;
    extern char *optarg;
    extern int optind;
    char *filename = 0;
    char *out_filename = 0;
    float *buf;
    float **attributes;
    int i, j;

    int numAttributes;
    int numObjects;
    char line[1024];
    float threshold = 0.001;
    double computation_timing, total_timing, input_read_timing, output_write_timing, compare_timing;

    total_timing = omp_get_wtime();
    input_read_timing = omp_get_wtime();
    while ((opt = getopt(argc, argv, "i:o:t:n:?")) != EOF) {
        switch (opt) {
            case 'i':
                filename = optarg;
                break;
            case 'o':
                out_filename = optarg;
                break;
            case 't':
                threshold = atof(optarg);
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


    if (filename == 0 || out_filename == 0) usage(argv[0]);

    numAttributes = numObjects = 0;

    /* from the input file, get the numAttributes and numObjects ------------*/

    FILE *infile;
    if ((infile = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        exit(1);
    }
    while (fgets(line, 1024, infile) != NULL)
        if (strtok(line, " \t\n") != 0)
            numObjects++;
    rewind(infile);
    while (fgets(line, 1024, infile) != NULL) {
        if (strtok(line, " \t\n") != 0) {
            /* ignore the id (first attribute): numAttributes = 1; */
            while (strtok(NULL, " ,\t\n") != NULL) numAttributes++;
            break;
        }
    }

    /* allocate space for attributes[] and read attributes of all objects */
    buf = (float *) malloc(numObjects * numAttributes * sizeof(float));
    attributes = (float **) malloc(numObjects * sizeof(float *));
    attributes[0] = (float *) malloc(numObjects * numAttributes * sizeof(float));
    for (i = 1; i < numObjects; i++)
        attributes[i] = attributes[i - 1] + numAttributes;
    rewind(infile);
    i = 0;
    while (fgets(line, 1024, infile) != NULL) {
        if (strtok(line, " \t\n") == NULL) continue;
        for (j = 0; j < numAttributes; j++) {
            buf[i] = atof(strtok(NULL, " ,\t\n"));
            i++;
        }
    }
    fclose(infile);
    memcpy(attributes[0], buf, numObjects * numAttributes * sizeof(float));
    //printf("I/O completed\n");
    input_read_timing = omp_get_wtime() - input_read_timing;

    //run diana
    computation_timing = omp_get_wtime();
    /* perform DIANA and saves result in dendrogram */
    DianaClustering dianaClustering(numObjects);
    dianaClustering.cluster(attributes, numAttributes, threshold);
    computation_timing = omp_get_wtime() - computation_timing;

    //show results
    printf("\nDivisive Analysis completed for %d data objects with %d features each\n", numObjects, numAttributes);

    output_write_timing = omp_get_wtime();
    int output_success = dianaClustering.dendrogram->toBinaryFile(out_filename) >= 0;
    //int output_success2 = dianaClustering.dendrogram->toTextFile((char*)"diana_gold.txt") >= 0;
    output_write_timing = omp_get_wtime() - output_write_timing;

    if (output_success) {
        printf("√√√√ SUCCESS: Output of execution written to %s √√√√\n", out_filename);
    } else {
        printf("XXXX FAILED to write output of execution to %s XXXX\n", out_filename);
    }

    total_timing = omp_get_wtime() - total_timing;
    printf("\nTotal elapsed time: %f\n", total_timing);
    printf("Time for IO: %f\n", input_read_timing + output_write_timing);
    printf("Time for processing: %f\n", computation_timing);

    free(attributes);
    free(buf);

    printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^ END OF DIANA ^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    return (0);
}
