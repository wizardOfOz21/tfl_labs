#pragma once
#include "string"
#include "Grammar.h"

class SLRTable{
private:

    const Grammar inputGrammar;
    std::vector<std::string> extendedGrammarRules;

    void processGrammar();
public:
    explicit SLRTable(Grammar  grammar);
    SLRTable()=default;


    ~SLRTable()=default;
};
