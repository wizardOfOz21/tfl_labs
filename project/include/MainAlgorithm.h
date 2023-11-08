#pragma once
#include "EquivalenceClassesTable.h"
#include "MATMock.h"

class MainAlgorithm{
private:
    std::vector<std::unique_ptr<StateMachine>> prefixLanguage;
    std::string alphabet;
    int admissionToRegularity;
    int maxLenOfWord;
    int maxNumOfItersForSymmetricPump;

    void fillPrefixLanguage(std::shared_ptr<IMAT> MAT);
public:
    MainAlgorithm(std::string& alphabet,int admissionToRegularity,
                  int maxLenOfWord,int maxNumOfItersForSymmetricPump);
    void Run(const std::shared_ptr<IMAT>& MAT);
};
