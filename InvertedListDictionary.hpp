//
// Created by Ali A. Kooshesh on 5/9/21.
//

#ifndef BINOMIALQUEUES_INVERTEDLISTDICTIONARY_HPP
#define BINOMIALQUEUES_INVERTEDLISTDICTIONARY_HPP

#include <vector>

class InvertedListDictionary {
public:
    InvertedListDictionary(int rangeOfvalues);

    void insert(int v);
    int numElements();
    bool empty();
    bool member(int v);
    void remove(int v);

protected:
    std::vector<int> repository, verifier;
};


#endif //BINOMIALQUEUES_INVERTEDLISTDICTIONARY_HPP
