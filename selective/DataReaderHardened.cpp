//
// Created by gabriel on 6/28/18.
//

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "DataReaderHardened.h"

void DataReaderHardened::read(char *filename) {
    char line[1024];
    int i, j;

    FILE *infile;
    if ((infile = fopen(filename, "r")) == nullptr) {
        fprintf(stderr, "Error: no such file (%s)\n", filename);
        exit(1);
    }
    while (fgets(line, 1024, infile) != nullptr)
        if (strtok(line, " \t\n") != 0)
            data->numObjects++;
    rewind(infile);
    while (fgets(line, 1024, infile) != nullptr) {
        if (strtok(line, " \t\n") != 0) {
            /* ignore the id (first attribute): numAttributes = 1; */
            while (strtok(nullptr, " ,\t\n") != nullptr)
                data->numAttributes++;
            break;
        }
    }

    /* allocate space for attributes[] and read attributes of all objects */
    data->attributes = (float **) malloc(data->numObjects * sizeof(float *));
    data->attributes[0] = (float *) malloc(data->numObjects * data->numAttributes * sizeof(float));
    for (i = 1; i < data->numObjects; i++)
        data->attributes[i] = data->attributes[i - 1] + data->numAttributes;
    rewind(infile);
    i = 0;
    while (fgets(line, 1024, infile) != nullptr) {
        if (strtok(line, " \t\n") == nullptr) continue;
        for (j = 0; j < data->numAttributes; j++) {
            /*buf[i]*/data->attributes[0][i] = static_cast<float>(atof(strtok(nullptr, " ,\t\n")));
            i++;
        }
    }
    fclose(infile);
}

DataReaderHardened::DataReaderHardened() {
    data = new InputData();
}

DataReaderHardened::~DataReaderHardened() {
    delete data;
}

InputData::InputData() {
    attributes = nullptr;
    numAttributes = 0;
    numObjects = 0;
}

InputData::~InputData() {
    if (attributes) {
        free(attributes[0]);
        free(attributes);
    }
}
