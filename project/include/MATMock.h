#pragma once

#include <string>
#include "IMAT.h"

class MATMock : public IMAT {
private:
    StateMachine pref;
    StateMachine suff;
    void checkPermutationsWithLen(StateMachine& M,
                                           const std::string& alphabet,std::vector<std::string>& permutation,
                                           int len,int curIndex,const std::string& mode, std::string *res);
public:
    MATMock(StateMachine& pref,StateMachine& suff);
    bool IsMembership(const std::string& word,const std::string& mode) override;
    std::string IsEqual(StateMachine& M,std::string& alphabet,int maxLenOfWord,const std::string& mode) override;
};
