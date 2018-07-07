//
// Created by gabriel on 6/28/18.
//

#ifndef DIANA_FT_DATAREADER_HARDENED_H
#define DIANA_FT_DATAREADER_HARDENED_H

class InputData {
public:
    InputData();

    virtual ~InputData();

    float **attributes;
    int numObjects;
    int numAttributes;
};

class DataReaderHardened {

public:
    InputData *data;

    DataReaderHardened();

    virtual ~DataReaderHardened();

    void read(char *filename);
};


#endif //DIANA_FT_DATAREADER_HARDENED_H
