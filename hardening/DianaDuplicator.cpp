//
// Created by gabriel on 6/28/18.
//

#include <thread>
#include "DianaDuplicator.h"
#include "DendrogramComparator.h"
#include "DataReader.h"
#include "../diana/DianaClustering.h"

void DianaDuplicator::runKernel(InputData *inputData, int number, Dendrogram **ptrToResult) {
    printf("Thread %d running ...\n", number);

    //run DIANA
    DianaClustering diana = DianaClustering(inputData->numObjects);
    diana.cluster(inputData->attributes, inputData->numAttributes, STD_KMEANS_THRESHOLD);

    //save result to provided location
    *ptrToResult = diana.dendrogram;

    printf("Thread %d finished\n", number);
}

void DianaDuplicator::spacialDuplication(char *inputFilename) {
    //read input data
    auto *dataReader = new DataReader();
    dataReader->read(inputFilename);

    //spin two threads
    std::thread thread1 (runKernel, dataReader->data, 0, &dendrograms[0]);
    std::thread thread2 (runKernel, dataReader->data, 1, &dendrograms[1]);

    //wait for them to finish
    thread1.join();
    thread2.join();

    delete dataReader;
}

bool DianaDuplicator::sdcDetected() {
    //check if there are results to compare
    if (!dendrograms[0] && !dendrograms[1]) {
        fprintf(stderr, "Attempted SDC detection before any execution\n");
        exit(EXIT_FAILURE);
    }

    //compare the dendrograms
    DendrogramComparator comparator;
    return !comparator.areEqual(dendrograms);
}

void DianaDuplicator::saveDendrogramBin(int index, char *filename) {
    dendrograms[index]->toBinaryFile(filename);
}

void DianaDuplicator::saveDendrogramsText( char *filenames[2]) {
    dendrograms[0]->toTextFile(filenames[0]);
    dendrograms[1]->toTextFile(filenames[1]);
}

DianaDuplicator::DianaDuplicator() {
    dendrograms[0] = nullptr;
    dendrograms[1] = nullptr;
}


