#pragma once

#include <string>
#include "IMAT.h"

class MATMock : public IMAT {
private:
    StateMachine ideal;
    int maxLenOfWord;
    std::string alphabet;
    void checkPermutationsWithLen(StateMachine& M,
                                           const std::string& alphabet,std::vector<std::string>& permutation,
                                           int len,int curIndex, std::string *res);
public:
    MATMock(StateMachine& m,std::string& alphabet,int maxLenOfWord);
    bool IsMembership(const std::string& word) override;
    std::string IsEqual(StateMachine& M) override;
};
