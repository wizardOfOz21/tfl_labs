#ifndef TFL_LABS_REGEXGENERATOR_H
#define TFL_LABS_REGEXGENERATOR_H
#include <string>
#include <vector>

class RegexGenerator {
    friend class StringGenerator;
private:
    std::vector<char> alphabet;
    size_t seed = 0;

    int lettersNum;
    int starNesting;
    int lookaheadNum;

    int curLettersNum=0;
    int curNesting=0;
    int curLookaheadNum=0;
    bool fromLookahead= false;
    bool wasLookaheadInBrackets= false;
    bool needToReturn = false;
    int curOpenBracketsNum=0;
    int needToClose=0;
    std::string res;

    void generateRegex();
    void generateConcRegex();
    void generateSimpleRegex();
    char randSymb();
    void changeSeed();

public:
    RegexGenerator();
    RegexGenerator(int lettersNum, int starNesting,int lookaheadNum,int alphabetSize);
    std::string GenerateRegex();
};

#endif //TFL_LABS_REGEXGENERATOR_H
