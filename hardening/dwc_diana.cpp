//
// Created by gabriel on 6/28/18.
//

#include <fstream>
#include <getopt.h>
#include "../diana/Dendrogram.h"
#include "DianaDuplicator.h"
#include "DendrogramComparator.h"

int num_omp_threads = 1;

void usage(char *argv0) {
    const char *help =
            "Usage: %s [switches]\n"
            "       -i  input-filename      : file containing data to be clustered\n"
            "       -a  output-filename0    : file containing the output result\n"
            "       -b  output-filename1    : file containing the output result\n"
            "       -d  detected-filename   : file to store detected sdcs\n"
            "       -g  golden-filename     : file containing the golden result\n";
    fprintf(stderr, help, argv0);
    exit(-1);
}

int main(int argc, char *argv[]) {
    int opt;
    char *inputFilename;
    char *outputFilenames[2];
    char *sdcDetectionsFilename;
    char *goldenFilename;

    while ((opt = getopt(argc, argv, "i:a:b:d:g:?")) != EOF) {
        switch (opt) {
            case 'i':
                inputFilename = optarg;
                break;
            case 'a':
                outputFilenames[0] = optarg;
                break;
            case 'b':
                outputFilenames[1] = optarg;
                break;
            case 'd':
                sdcDetectionsFilename = optarg;
                break;
            case 'g':
                goldenFilename = optarg;
                break;
            case '?':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    for (int i = 0; i < 1; ++i) {

        //spacial duplication (2 threads)
        DianaDuplicator dwc;
        dwc.spacialDuplication(inputFilename);
        dwc.saveDendrogramBin(0, outputFilenames[0]);
        //dwc.saveDendrogramsText(outputFilenames); //DEBUG

        //if there was an SDC, update SDCs count in file
        if (dwc.sdcDetected()) {
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
        }
    }
}