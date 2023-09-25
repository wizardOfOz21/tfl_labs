#ifndef TFL_LABS_REGEXTOGLUSHKOVCONVERTER_H
#define TFL_LABS_REGEXTOGLUSHKOVCONVERTER_H

#include <iostream>
#include <unordered_map>
#include "StateMachine.h"

class RegexToGlushkovConverter{
private:
    StateMachine automata;
    std::string inputRegex;
    int stateCount;
    struct FFL {
        std::vector<std::pair<char, int>> First;
        std::unordered_map<std::string,std::vector<std::pair<char, int>>> Follow;
        std::vector<std::pair<char, int>>Last;
        FFL()=default;
        ~FFL()=default;
    };
public:
    explicit RegexToGlushkovConverter(std::string  regex);
    StateMachine buildGlushkovStateMachine();

    // TODO: сделать приватными
    std::vector<std::pair<char, int>> computeFirst();
    std::vector<std::pair<char, int>> computeLast();
    std::unordered_map<std::string,std::vector<std::pair<char, int>>> computeFollow();
};

#endif //TFL_LABS_REGEXTOGLUSHKOVCONVERTER_H
