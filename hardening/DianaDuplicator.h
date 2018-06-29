//
// Created by gabriel on 6/28/18.
//

#ifndef DIANA_FT_DIANA_DUPLICATOR_H
#define DIANA_FT_DIANA_DUPLICATOR_H


#include "../diana/Dendrogram.h"
#include "DataReader.h"

#define STD_KMEANS_THRESHOLD 0.01

class DianaDuplicator {

public:

    DianaDuplicator();

    /**
     * Runs two threads of DIANA and saves the results in memory.
     * @param inputFilename the name of the input file
     */
    void spacialDuplication(char *inputFilename);

    /**
     * Compares saved dendrograms for detected SDCs
     * @return whether an SDC was detected
     */
    bool sdcDetected();

    /**
     * Save one of the dendrograms in a binary file
     */
    void saveDendrogramBin(int index, char *filename);

    /**
     * Save one of the dendrograms in a text file
     */
    void saveDendrogramText(int index, char *filename);

private:


    /**
     * Two dendrograms that are the results of the two kernel executions
     */
    Dendrogram *dendrograms[2];

    /**
     * Executes DIANA kernel. Should be used in a thread
     * @param inputFilename the name of the input file
     * @param number a number to be printed by the thread
     * @param ptrToResult points to where to store the result of the computation (which is a Dendrogram *)
     */
    static void runKernel(InputData *inputData, int number, Dendrogram **ptrToResult);
};


#endif //DIANA_FT_DIANA_DUPLICATOR_H
