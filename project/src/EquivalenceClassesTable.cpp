#include "EquivalenceClassesTable.h"
#include <fstream>

EquivalenceClassesTable::EquivalenceClassesTable(std::string& alphabet,std::unique_ptr<IMAT> MAT)
:alphabet(alphabet),MAT(std::move(MAT)){
    mainTable[""]="";
    suffixes.emplace_back("");
    fillRecognitionStringForMainTable("");
}

void EquivalenceClassesTable::updateClassesOfEquivalence(){
    classesOfEquivalence.clear();
    for (const auto& equivClass:mainTable){
        if (classesOfEquivalence.find(equivClass.second)==classesOfEquivalence.end()){
            classesOfEquivalence[equivClass.second]=equivClass.first;
        }
        if (classesOfEquivalence[equivClass.second]>equivClass.first){
            classesOfEquivalence[equivClass.second]=equivClass.first;
        }
    }
}

void EquivalenceClassesTable::fillRecognitionStringForAdditionalTable(const std::string& curPrefix){
    int alreadyFilled = additionalTable[curPrefix].length();
    for (int i=alreadyFilled;i<suffixes.size();i++){
        if (MAT->IsMembership(curPrefix+suffixes[i])){
            additionalTable[curPrefix]+="+";
        }else {
            additionalTable[curPrefix]+="-";
        }
    }
}

void EquivalenceClassesTable::fillRecognitionStringForMainTable(const std::string& curPrefix){
    int alreadyFilled = mainTable[curPrefix].length();
    for (int i=alreadyFilled;i<suffixes.size();i++){
        if (MAT->IsMembership(curPrefix+suffixes[i])){
            mainTable[curPrefix]+="+";
        }else {
            mainTable[curPrefix]+="-";
        }
    }
}

void EquivalenceClassesTable::AddNewKeysToAdditionalTable(){
    for (const auto& equivClass : mainTable){
        for (char symb : alphabet){
            std::string curPrefix=equivClass.first+std::string(1,symb);
            if (additionalTable.find(curPrefix)==additionalTable.end()){
                additionalTable[curPrefix]="";

            }
        }
    }
}

void EquivalenceClassesTable::FillMainTable(){
    for (const auto& equivClass : mainTable){
        fillRecognitionStringForMainTable(equivClass.first);
    }
}

void EquivalenceClassesTable::FillAdditionalTable(){
    for (const auto& equivClass : additionalTable){
        fillRecognitionStringForAdditionalTable(equivClass.first);
    }
}

void EquivalenceClassesTable::AddWordAndItsPrefixesToMainTable(std::string & word){
    std::string curPrefix;
    for (char symb : word){
        curPrefix+=std::string(1,symb);
        if (mainTable.find(curPrefix)==mainTable.end()){
            mainTable[curPrefix]="";
            fillRecognitionStringForMainTable(curPrefix);
        }
    }
}

void EquivalenceClassesTable::MakeComplete(){
    for (const auto& elem : additionalTable) {
        if (classesOfEquivalence.find(elem.second)==classesOfEquivalence.end()){
            mainTable[elem.first]=elem.second;
            classesOfEquivalence[elem.second]=elem.first;
            fillRecognitionStringForMainTable(elem.first);
            AddNewKeysToAdditionalTable();
            FillAdditionalTable();
        }
    }
}

bool EquivalenceClassesTable::MakeConsistent(){
    for (auto elem1 : additionalTable){
        for (auto elem2 : additionalTable){
            if (elem1==elem2){
                continue;
            }
            auto word1=elem1.first;
            auto word2=elem2.first;
            word1.pop_back();
            word2.pop_back();
            if (word1==word2){
                continue;
            }
            if ((elem1.second==elem2.second) != (mainTable[word1]==mainTable[word2])){
                if (mainTable[word1]==mainTable[word2]){
                    for (int i=0;i<mainTable[word1].size();i++){
                        if (mainTable[word1][i]!=elem2.second[i] || mainTable[word1][i]!=elem1.second[i]){
                            suffixes.push_back(alphabet[0]+suffixes[i]);
                            FillMainTable();
                            FillAdditionalTable();
                            updateClassesOfEquivalence();
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


StateMachine EquivalenceClassesTable::BuildDFA() {
    int countOfStates=-1;
    std::unordered_set<int> finalStates;
    std::unordered_map<std::string,int> prefixToStateNumDict;
    std::unordered_set<std::string> used;
    for (auto elem:mainTable){
        if (used.find(elem.second)==used.end()){
            countOfStates++;
            used.insert(elem.second);
            prefixToStateNumDict[elem.first]=countOfStates;
            if (elem.second[0]=='+'){
                finalStates.insert(countOfStates);
            }
        }
    }
    std::vector<std::string> v(countOfStates+1);
    std::vector<std::vector<std::string>> transitions(countOfStates+1, v);
    for (const auto& elem: prefixToStateNumDict){
        for (char letter: alphabet){
            auto nextState=elem.first+std::string(1,letter);
            auto from=elem.second;
            auto to = prefixToStateNumDict[classesOfEquivalence[additionalTable[nextState]]];
            if (transitions[from][to].empty()){
                transitions[from][to]=std::string(1,letter);
            } else {
                transitions[from][to]+=" "+std::string(1,letter);
            }
        }
    }
    return {transitions,finalStates,countOfStates};
}

std::unique_ptr<StateMachine> EquivalenceClassesTable::LStar
(std::string& alphabet,int maxNumOfEquivClasses, int maxLenOfWord, std::unique_ptr<IMAT> MAT){
    EquivalenceClassesTable equivTable(alphabet,std::move(MAT));
    StateMachine DFA;
    int i=0;
    while (true){
        if (i==2){
            return nullptr;
        }
        equivTable.updateClassesOfEquivalence();
        if (equivTable.classesOfEquivalence.size()>=maxNumOfEquivClasses){
            return nullptr;
        }
        equivTable.AddNewKeysToAdditionalTable();
        equivTable.FillAdditionalTable();

        bool f= true;
        while (f) {
            equivTable.MakeComplete();
            f = equivTable.MakeConsistent();
        }

        DFA = equivTable.BuildDFA();
        std::ofstream out("out"+std::to_string(i));
        StateMachine::To_Graph(DFA,out);
        out.close();

        auto verdict=equivTable.MAT->IsEqual(DFA);
        if (verdict=="equal"){
            break;
        }
        equivTable.AddWordAndItsPrefixesToMainTable(verdict);
        i++;
    }
    return std::make_unique<StateMachine>(DFA);
}