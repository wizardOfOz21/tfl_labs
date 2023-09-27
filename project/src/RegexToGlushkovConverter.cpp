#include "../include/RegexToGlushkovConverter.h"
#include <iostream>
#include <utility>
#include <stack>

RegexToGlushkovConverter::RegexToGlushkovConverter(std::string regex):inputRegex(std::move(regex)),stateCount(0){}

std::vector<std::pair<char, int>> RegexToGlushkovConverter::computeFirst() {
    int countReal = 0;
    int countFact = 0;
    int i = 0;
    std::stack<std::pair<char, int>> st;
    std::vector<std::pair<char, int>> first;
    while (i < inputRegex.length()) {
        switch (inputRegex[i]) {
            case '(':
                st.emplace('(', countFact);
                break;
            case ')':
                st.pop();
                break;
            case '*':
                if (!st.empty()){
                    countFact=st.top().second;
                } else if (countFact!=0) {
                    countFact--;
                }
                break;
            case '|':
                if (!st.empty()){
                    countFact=st.top().second;
                } else {
                    countFact=0;
                }
                break;
            case '.':
                break;
            default :
                if (countFact == 0) {
                    first.emplace_back(inputRegex[i], ++countReal);
                    countFact++;
                } else {
                    countReal++;
                }
        }
        i++;
    }
    stateCount=countReal;
    return first;
}

std::vector<std::pair<char, int>> RegexToGlushkovConverter::computeLast() {
    int countReal = 0;
    int countFact = 0;
    int i = inputRegex.length()-1;
    std::stack<int> st;
    std::vector<std::pair<char, int>> last;
    while (i >= 0) {
        switch (inputRegex[i]) {
            case '(':
                st.pop();
                if (!st.empty()){
                    countFact=st.top();
                } else if (countFact!=0) {
                    countFact--;
                }
                break;
            case ')':
                st.push(countFact);
                break;
            case '*':
                st.push(countFact);
                break;
            case '|':
                if (!st.empty()){
                    countFact=st.top();
                } else {
                    countFact=0;
                }
                break;
            case '.':
                break;
            default :
                if (countFact == 0) {
                    last.emplace_back(inputRegex[i], stateCount-countReal);
                    countReal++;
                    countFact++;
                } else {
                    countReal++;
                }
        }
        i--;
    }
    return last;
}

StateMachine RegexToGlushkovConverter::buildGlushkovStateMachine(){
    FFL ffl;
    ffl.First=this->computeFirst();
    ffl.Last= this->computeLast();
    ffl.Follow = this->computeFollow();
    StateMachine automata(stateCount,stateCount);
    int count=0;
    for (int i=0;i<inputRegex.length();i++){
        char letter=inputRegex[i];
        if (isalpha(letter)){
            count++;
        }
        if (letter=='.') {
            if (i+1<inputRegex.length() && inputRegex[i+1]=='*'){
                automata.AddNonAlphabetSymbol(count,-1);
                i++;
            } else {
                automata.AddNonAlphabetSymbol(count,1);
            }
        }
    }
    for (auto a : ffl.First){
        automata.AddTransition(0,a);
    }
    for(auto & iter : ffl.Follow){
        int curState = int(iter.first.c_str()[1]-'0');
        for (auto a : iter.second){
            automata.AddTransition(curState,a);
        }
    }
    automata.SetFinalStates(ffl.Last);
    return automata;
}

std::unordered_map<std::string,std::vector<std::pair<char, int>>> RegexToGlushkovConverter::computeFollow() {
    int count = 0;
    int i = 0;
    std::vector<char> order(1);
    std::stack<std::pair<char, int>> st;
    std::unordered_map<std::string,std::vector<std::pair<char, int>>> follow;
    while (i < inputRegex.length()) {
        switch (inputRegex[i]) {
            case '(':
                st.emplace('(', count);
                break;
            case ')':
                if (i+1<inputRegex.length() && inputRegex[i+1]=='*'){
                    break;
                }

                if (!st.empty() && st.top().first=='|' && isalpha(inputRegex[i+1])){
                    std::string key=std::string(1,order[st.top().second])+std::to_string(st.top().second);
                    follow[key].emplace_back(inputRegex[i+1],count+1);

                    key=std::string(1,order[count])+std::to_string(count);
                    follow[key].emplace_back(inputRegex[i+1],count+1);

                    count++;
                    order.push_back(inputRegex[i+1]);
                    i++;
                    st.pop();
                }
                st.pop();
                break;
            case '*':
                if (i-1>=0 && isalpha(inputRegex[i-1])) {
                    std::string key=std::string(1,order[count])+std::to_string(count);
                    follow[key].emplace_back(inputRegex[i-1],count);
                }

                if (i-1>=0 && inputRegex[i-1]==')') {
                    if (st.top().first=='|'){
                        auto curOper= st.top();
                        st.pop();

                        std::string key=std::string(1,order[count])+std::to_string(count);
                        follow[key].emplace_back(order[curOper.second+1],curOper.second+1);
                        follow[key].emplace_back(order[st.top().second+1],st.top().second+1);

                        key=std::string(1,order[curOper.second])+std::to_string(curOper.second);
                        follow[key].emplace_back(order[curOper.second+1],curOper.second+1);
                        follow[key].emplace_back(order[st.top().second+1],st.top().second+1);

                    } else {
                        std::string key=std::string(1,order[count])+std::to_string(count);
                        follow[key].emplace_back(order[st.top().second+1],st.top().second+1);
                    }
                }

                break;
            case '|':
                if (!st.empty() && st.top().second!=0){
                    std::string key=std::string(1,order[st.top().second])+std::to_string(st.top().second);
                    follow[key].emplace_back(inputRegex[i+1],count+1);
                }
                st.emplace('|', count);
                count++;
                order.push_back(inputRegex[i+1]);
                i++;
                break;
            case '.':
                break;
            default :
                if (count!=0){
                    std::string key=std::string(1,order[count])+std::to_string(count);
                    follow[key].emplace_back(inputRegex[i],count+1);
                }
                count++;
                order.push_back(inputRegex[i]);
        }
        i++;
    }
    return follow;
}
