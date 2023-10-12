#include "../include/RegexGenerator.h"
#include <ctime>

RegexGenerator::RegexGenerator() : RegexGenerator::RegexGenerator(13, 2, 2, 2) {}

RegexGenerator::RegexGenerator(int lettersNum, int starNesting,int lookaheadNum,int alphabetSize)
        :lettersNum(lettersNum),starNesting(starNesting),lookaheadNum(lookaheadNum) {

    if (lettersNum < 0) return;
    if (starNesting < 0) starNesting = 0;
    if (lookaheadNum < 0) lookaheadNum = 0;
    if (alphabetSize< 1) alphabetSize= 1;

    for (char i = 'a'; i < 'a' + alphabetSize && i <= 'z'; i++) {
        alphabet.push_back(i);
    }
}

void RegexGenerator::changeSeed() {
    seed++;
    srand((size_t)time(nullptr) + seed + rand());
}

char RegexGenerator::randSymb() {
    return alphabet[rand()%alphabet.size()];
}

std::string RegexGenerator::GenerateRegex() {
    changeSeed();
    curLettersNum=0;
    curNesting=0;
    curLookaheadNum=0;
    res="^";
    while(curLettersNum!=lettersNum) generateRegex();
    res+="$";
    return res;
}


//<regex> ::= <conc-regex> <alt> <regex> | <conc-regex>
void RegexGenerator::generateRegex(){
    int state;
    if (lettersNum==curLettersNum) return;
    if (lettersNum-curLettersNum<2) state=0;
    else state=rand()%2;

    switch (state) {
        case 0:
            generateConcRegex();
            if (lettersNum==curLettersNum) return;
            res+="|";
            generateRegex();
            break;
        case 1:
            generateConcRegex();
            break;
    }
}

// <conc-regex> ::= <simple-regex> | <simple-regex><conc-regex>
void RegexGenerator::generateConcRegex() {
    int state = rand()%2;
    switch (state) {
        case 0:
            generateSimpleRegex();
            break;
        case 1:
            generateSimpleRegex();
            if (lettersNum==curLettersNum) return;
            generateConcRegex();
            break;
    }
}

// <simple-regex> ::= <lbr><regex><rbr><unary>? | буква <unary>? | (?=regex$)
void RegexGenerator::generateSimpleRegex() {
    int state = rand()%3;
    if (curLettersNum==0){
        state = rand() % 7;
        if (state != 0 && state!=2) state = 1;
    }
    if (lookaheadNum==curLookaheadNum || fromLookahead) {
        if (fromLookahead) {
            state = rand() % 4;
            if (state != 0) state = 1;
        } else {
            state=rand()%2;
        }
    }
    bool needStar= false;
    switch (state) {
        case 0:
            curOpenBracketsNum++;
            res+="(";
            if (curNesting!=starNesting){
                int v=rand()%2;
                switch (v) {
                    case 0:
                        needStar=true;
                        curNesting++;
                        break;
                    case 1:
                        if (curNesting!=0) curNesting--;
                }
            }
            generateRegex();
            res+=")";
            if (!wasLookaheadInBrackets && needStar) res+="*";
            curOpenBracketsNum--;
            if (curOpenBracketsNum==0) wasLookaheadInBrackets= false;
            break;
        case 1:
            res+=randSymb();
            curLettersNum++;
            if (curNesting!=starNesting){
                int v=rand()%2;
                switch (v) {
                    case 0:
                        res+="*";
                        curNesting++;
                        break;
                    case 1:
                        if (curNesting!=0) curNesting--;
                }
            }
            break;
        case 2:
            curLookaheadNum++;
            res+="(?=";
            fromLookahead= true;
            generateRegex();
            fromLookahead= false;
            wasLookaheadInBrackets= true;
            res+="$)";
    }
}

/*
GRAMMAR:
<regex> ::= <conc-regex> <alt> <regex> | <conc-regex>
<conc-regex> ::= <simple-regex> | <simple-regex><conc-regex>
<simple-regex> ::= <lbr><regex><rbr><unary>? | буква <unary>? | (?=<regex>$)
<alt> ::= '|'
<lbr> ::= '('
<rbr> ::= ')'
<unary> ::= '*'
*/
