#pragma once
#include "EquivalenceClassesTable.h"
#include "MATMock.h"

class MainAlgorithm{
private:
    std::vector<std::unique_ptr<StateMachine>> prefixLanguage;
    std::vector<std::unique_ptr<StateMachine>> suffixLanguage;
    std::string alphabet;
    int admissionToRegularity;
    int maxLenOfWord;
    int maxNumOfItersForSymmetricPump;

    void fillLanguage(std::shared_ptr<IMAT> MAT, const std::string& mode);
public:
    MainAlgorithm(std::string& alphabet,int admissionToRegularity,
                  int maxLenOfWord,int maxNumOfItersForSymmetricPump);
    void Run(const std::shared_ptr<IMAT>& MAT);
};
