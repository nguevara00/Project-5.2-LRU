//
// Created by Ali A. Kooshesh on 5/9/21.
//

#include "InvertedListDictionary.hpp"

InvertedListDictionary::InvertedListDictionary(int rangeOfvalues) {
    repository.resize(rangeOfvalues);
}

void InvertedListDictionary::insert(int v) {
    repository.at(v) = verifier.size();
    verifier.push_back(v);
}

int InvertedListDictionary::numElements() {
    return verifier.size();
}

bool InvertedListDictionary::empty() {
    return numElements() == 0;
}

bool InvertedListDictionary::member(int v) {
    return repository.at(v) >= 0 && repository.at(v) < verifier.size() &&
           verifier.at(repository.at(v) ) == v;
}

void InvertedListDictionary::remove(int v) {
    verifier.at( repository.at(v) ) = verifier.at( verifier.size() - 1);
    repository.at( verifier.at( verifier.size() - 1) ) = repository.at(v);
    verifier.pop_back();
}
