#include "SLRTable.h"

#include <utility>
#include "iostream"
#include <sstream>
#include <algorithm>

const std::string DOT = ".";


SLRTable::SLRTable(Grammar  grammar):inputGrammar(std::move(grammar)){
    processGrammar();

    std::vector<ExtendedRule> tmp;
    auto I0 = findClosure(tmp,newStartToken);
    stateDict[0] = I0;

    generateStates();

    int a;
    std::cout<<"here";
}

void SLRTable::processGrammar(){
    const std::string SUFFIX = "'";

    newStartToken = inputGrammar.StartToken() + SUFFIX;
    while (inputGrammar.NonTerms().find(newStartToken)!=inputGrammar.NonTerms().end()){
        newStartToken+=SUFFIX;
    }
    auto str = ExtendedRule{
            newStartToken,
            std::vector<std::string> {DOT,inputGrammar.StartToken()},
    };

    extendedGrammarRules.push_back(ExtendedRule {
        newStartToken,
        std::vector<std::string> {DOT,inputGrammar.StartToken()},
    });

    for (std::string rule : inputGrammar.Rules()) {
        std::stringstream ss(rule);
        std::string token;
        std::string lhs;
        std::vector<std::string> rhs {DOT};
        bool wasOr = false;

        while (ss >> token) {
            if (lhs.empty()){
                lhs=token;
            } else if (token=="->"){
                continue;
            } else if (token == "|"){
                wasOr = true;
                extendedGrammarRules.push_back(ExtendedRule{lhs,rhs});

                rhs.clear();
                rhs.push_back(DOT);
            } else {
                rhs.push_back(token);
            }
        }
        if (wasOr){
            extendedGrammarRules.push_back(ExtendedRule{lhs,rhs});
        }
    }
}

std::vector<SLRTable::ExtendedRule> SLRTable::findClosure(std::vector<ExtendedRule>& inpState, std::string& tokenAfterDOT) {
    std::vector<ExtendedRule> closure;

    if (tokenAfterDOT==newStartToken){
        for (const ExtendedRule& rule :extendedGrammarRules){
            if (rule.LHS==tokenAfterDOT){
                closure.push_back(rule);
            }
        }
    } else {
        closure=inpState;
    }

    int prevLen = -1;
    while(prevLen!=closure.size()) {
        prevLen = closure.size();

        std::vector<ExtendedRule> tmpClosure;

        for (ExtendedRule rule : closure) {
                if (rule.RHS[rule.RHS.size() - 1] != DOT) {
                    int dotInd = (std::find(rule.RHS.begin(),rule.RHS.end(),DOT) - rule.RHS.begin());
                    std::string dotPointsHere = rule.RHS[dotInd + 1];
                    for (const ExtendedRule &in_rule: extendedGrammarRules) {
                        if (dotPointsHere == in_rule.LHS && !isBelong(in_rule, tmpClosure)) {
                            tmpClosure.push_back(in_rule);
                        }
                    }

                }
        }
        for (const ExtendedRule& rule:tmpClosure){
            if (!isBelong(rule,closure)){
                closure.push_back(rule);
            }
        }

    }

    return closure;
}

void SLRTable::computeGOTO(int state){
    for (ExtendedRule rule : stateDict[state]) {
        std::unordered_set<std::string> generateStatesFor;

        if (rule.RHS[rule.RHS.size()-1]!=DOT){
            int dotInd = (std::find(rule.RHS.begin(),rule.RHS.end(),DOT) - rule.RHS.begin());
            std::string dotPointsHere = rule.RHS[dotInd + 1];
            if (generateStatesFor.find(dotPointsHere)==generateStatesFor.end()){
                generateStatesFor.insert(dotPointsHere);
            }
        }
        if (!generateStatesFor.empty()) {
            for (const std::string& token : generateStatesFor){
                GOTO(state,token);
            }
        }
    }
}

void SLRTable::GOTO(int state,const std::string& token) {
    std::vector<ExtendedRule> newState;

    for (ExtendedRule rule : stateDict[state]) {
        if (rule.RHS[rule.RHS.size()-1]!=DOT){
            int dotInd = (std::find(rule.RHS.begin(),rule.RHS.end(),DOT) - rule.RHS.begin());
            if ( rule.RHS[dotInd + 1] == token){
                auto shiftedRule = rule;
                shiftedRule.RHS[dotInd]=shiftedRule.RHS[dotInd + 1];
                shiftedRule.RHS[dotInd + 1] = '.';
                newState.push_back(shiftedRule);
            }
        }
    }

    std::vector<ExtendedRule> addClosureRules;
    for (ExtendedRule rule : newState){
        if (rule.RHS[rule.RHS.size()-1]!=DOT){
            int dotInd = (std::find(rule.RHS.begin(),rule.RHS.end(),DOT) - rule.RHS.begin());
            auto closureRes = findClosure(newState,rule.RHS[dotInd+1]);
            for (ExtendedRule inRule: closureRes){
                if (!isBelong(inRule,addClosureRules) && !isBelong(inRule,newState)){
                    addClosureRules.push_back(inRule);
                }
            }
        }
    }

    for (ExtendedRule rule : addClosureRules){
        newState.push_back(rule);
    }

    int stateExists = -1;
    for (std::pair<const int, std::vector<ExtendedRule>> _state : stateDict){
        if (stateDict[_state.first]==newState){
            stateExists=_state.first;
            break;
        }
    }

    if (stateExists == -1){
        stateCount++;
        stateDict[stateCount]=newState;
        stateDict2[std::make_pair(state,token)]=stateCount;
    } else {
        stateDict2[std::make_pair(state,token)]=stateExists;
    }
}

void SLRTable::generateStates(){
    int prevLen=-1;
    std::unordered_set<int> used;
    while(stateDict.size()!=prevLen){
        prevLen=stateDict.size();
        auto keys= getKeys(stateDict);
        for (auto key : keys){
            if (used.find(key)==used.end()){
                used.insert(key);
                computeGOTO(key);
            }
        }
    }
}

std::vector<int> SLRTable::getKeys(std::unordered_map<int,std::vector<ExtendedRule>> map) {
    std::vector<int> keys;
    for (auto record :map){
        keys.push_back(record.first);
    }
    return keys;
}

bool SLRTable::isBelong(ExtendedRule rule,std::vector<ExtendedRule> arr){
    for (const ExtendedRule& elem : arr){
        if (rule==elem){
            return true;
        }
    }
    return false;
}