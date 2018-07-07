//
// Created by gabriel on 7/7/18.
//

#include <fstream>
#include "HardeningUtils.h"

void onSDCDetected() {
    int previousSDCs;

    //open file as `in` to read it
    std::ifstream fileIn(sdcDetectionsFilename);

    //get SDCs reported until now
    if (!fileIn.is_open())
        previousSDCs = 0;
    else {
        fileIn >> previousSDCs;
        fileIn.close();
    }

    //open file as `out` to write to it
    std::ofstream fileOut(sdcDetectionsFilename, std::ofstream::trunc);

    if (!fileOut.is_open()) {
        fprintf(stderr, "Can't open %s as ofstream to report SDC\n", sdcDetectionsFilename);
        exit(EXIT_FAILURE);
    }

    fileOut << previousSDCs + 1;
    fileOut.close();

    //infinite loop and wait to be killed
    for (int j = 0; j < 42; j++) {
        printf(".");
        j--;
    }
}

bool assertEqual(int a, int b) {
    if (a == b) return true;
    onSDCDetected();
}
