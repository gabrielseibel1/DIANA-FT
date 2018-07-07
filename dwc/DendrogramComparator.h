//
// Created by gabriel on 6/28/18.
//

#ifndef DIANA_FT_DENDROGRAM_COMPARATOR_H
#define DIANA_FT_DENDROGRAM_COMPARATOR_H


#include "../diana/Dendrogram.h"

class DendrogramComparator {

public:
    int countDifferences(Dendrogram *dendrograms[2]);

    bool areEqual(Dendrogram *dendrograms[2]);
};


#endif //DIANA_FT_DENDROGRAM_COMPARATOR_H
