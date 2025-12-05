//
// Created by Ali A. Kooshesh on 5/9/21.
//

#include "SmallIntMixedOperations.hpp"
#include<iostream>

int SmallIntMixedOperations::minValue() {
    if(empty())
        return INT32_MAX;

    int smallest = verifier.at(0);
    for( auto v: verifier )
        if( v < smallest )
            smallest = v;
    return smallest;
}

int SmallIntMixedOperations::aRandomValue() {
    // Pre-condition -- the inverted list is not empty.

    int randIdx = rand() % numElements();
    return verifier.at(randIdx);
}

void SmallIntMixedOperations::print() {
    for(auto v: verifier )
        std::cout << v << std::endl;
}

