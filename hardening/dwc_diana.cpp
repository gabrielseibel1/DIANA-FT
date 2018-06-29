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
            "       -d  detected-filename   : file to store detected sdcs\n"
            "       -o  output-filename0    : file containing the output result (bin) \n"
            "[OPT]  -a  output-filename0    : file containing #1 output result (txt) \n"
            "[OPT]  -b  output-filename1    : file containing #2 output result (txt)\n";
    fprintf(stderr, help, argv0);
    exit(-1);
}

int main(int argc, char *argv[]) {
    int opt;
    char *inputFilename;
    char *outputFilename;
    char *textOutputsFilenames[2] = {nullptr, nullptr};
    char *sdcDetectionsFilename;

    while ((opt = getopt(argc, argv, "i:o:d:a:b:?")) != EOF) {
        switch (opt) {
            case 'i':
                inputFilename = optarg;
                break;
            case 'o':
                outputFilename = optarg;
                break;
            case 'a':
                textOutputsFilenames[0] = optarg;
                break;
            case 'b':
                textOutputsFilenames[1] = optarg;
                break;
            case 'd':
                sdcDetectionsFilename = optarg;
                break;
            case '?':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    //spacial duplication (2 threads)
    DianaDuplicator dwc;
    dwc.spacialDuplication(inputFilename);
    dwc.saveDendrogramBin(0, outputFilename);

    //if user specified .txt files to dump results, use them
    if (textOutputsFilenames[0]) dwc.saveDendrogramText(0, textOutputsFilenames[0]);
    if (textOutputsFilenames[1]) dwc.saveDendrogramText(1, textOutputsFilenames[1]);

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