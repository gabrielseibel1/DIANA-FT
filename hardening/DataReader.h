//
// Created by gabriel on 6/28/18.
//

#ifndef DIANA_FT_DATAREADER_H
#define DIANA_FT_DATAREADER_H

class InputData {
public:
    InputData();

    virtual ~InputData();

    float **attributes;
    int numObjects;
    int numAttributes;
};

class DataReader {

public:
    InputData *data;

    DataReader();

    virtual ~DataReader();

    void read(char *filename);
};


#endif //DIANA_FT_DATAREADER_H
