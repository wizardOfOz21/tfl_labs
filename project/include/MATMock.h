#pragma once

#include <string>
#include "IMAT.h"

class MATMock : public IMAT {
private:
    StateMachine ideal;
    void checkPermutationsWithLen(StateMachine& M,
                                           const std::string& alphabet,std::vector<std::string>& permutation,
                                           int len,int curIndex, std::string *res);
public:
    MATMock(StateMachine& m);
    bool IsMembership(const std::string& word) override;
    std::string IsEqual(StateMachine& M,std::string& alphabet,int maxLenOfWord) override;
};
