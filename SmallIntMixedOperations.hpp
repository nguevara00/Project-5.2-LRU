//
// Created by Ali A. Kooshesh on 5/9/21.
//

#ifndef BINOMIALQUEUES_SMALLINTMIXEDOPERATIONS_HPP
#define BINOMIALQUEUES_SMALLINTMIXEDOPERATIONS_HPP
#include <vector>
#include "InvertedListDictionary.hpp"

class SmallIntMixedOperations: public InvertedListDictionary {
public:
    SmallIntMixedOperations(int rangeOfValues): InvertedListDictionary(rangeOfValues) {}
    int minValue();
    int aRandomValue();
    void print();

};


#endif //BINOMIALQUEUES_SMALLINTMIXEDOPERATIONS_HPP
