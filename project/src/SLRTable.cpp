#include "SLRTable.h"

#include <utility>
#include "iostream"
#include <sstream>
#include <algorithm>
#include <set>

const std::string DOT = ".";
const std::string EPSILON = "#";
const std::string NOTHING ="NOTHING";
const std::string SPEC_TOKEN="@";


SLRTable::SLRTable(Grammar  grammar):inputGrammar(std::move(grammar)){
    processGrammar();

    std::vector<ExtendedRule> tmp;
    auto I0 = findClosure(tmp,newStartToken);
    stateDict[0] = I0;

    generateStates();

    createParseTable();

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
        GOTOStateDict[std::make_pair(state,token)]=stateCount;
    } else {
        GOTOStateDict[std::make_pair(state,token)]=stateExists;
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

std::vector<std::string> SLRTable::findCols(){
    std::vector<std::string> rows;
    for (const auto& term : inputGrammar.Terms()){
        rows.push_back(term);
    }
    rows.push_back(SPEC_TOKEN);
    for (const auto& nonTerm : inputGrammar.NonTerms()){
        rows.push_back(nonTerm);
    }


    return rows;
}

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{

    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;

    std::vector<T> vec(first, last);
    return vec;
}

std::vector<std::string> strToVect(const std::string& str){
    std::stringstream ss(str);
    std::vector<std::string> ans;
    std::string token;

    while (ss >> token) {
        ans.push_back(token);
    }
    return ans;
}

std::vector<std::string> SLRTable::first(std::vector<std::string>& rule){
    if (rule[0]!=NOTHING && !rule.empty()) {
        if (rule[0]==EPSILON) {
            return {EPSILON};
        }
        if (inputGrammar.Terms().find(rule[0])!=inputGrammar.Terms().end()){
            return {rule[0]};
        }

        if (dict.find(rule[0])!=dict.end()){
            std::vector<std::string> res;
            auto rhs = dict[rule[0]];
            for (std::string token : rhs){
                std::vector<std::string> v{token};
                auto inRes= first(v);
                for (auto t : inRes){
                    res.push_back(t);
                }
            }
            if (std::find(res.begin(), res.end(),EPSILON)==res.end()){
                return res;
            }
            res.erase(std::remove(res.begin(), res.end(), EPSILON), res.end());
            if (res.size()>1){
                auto sliced =slice(rule,1,rule.size()-1);
                auto ansNew = first(sliced);
                if (ansNew[0]!=NOTHING){
                    res.insert( res.end(), ansNew.begin(), ansNew.end());
                }
                return res;
            }
            res.push_back(EPSILON);
            return res;
        }
    }
    return {NOTHING};
}

std::vector<std::string> setToVect(std::unordered_set<std::string> s ){
    std::vector<std::string> res;
    for (auto i : s){
        res.push_back(i);
    }
    return res;
}

std::vector<std::string> SLRTable::follow(const std::string& nonTerm) {
    std::unordered_set<std::string> solSet;
    if (nonTerm==newStartToken){
        solSet.insert(SPEC_TOKEN);
    }

    for (auto cur : dict){
        std::string curNonTerm = cur.first;
        auto rhs = cur.second;
        for (std::string subRule : rhs) {
            auto subRuleVect= strToVect(subRule);

            while (std::find(subRuleVect.begin(), subRuleVect.end(),nonTerm)!=subRuleVect.end()) {
                auto nonTermInd = std::find(subRuleVect.begin(), subRuleVect.end(),nonTerm);
                subRuleVect = slice(subRuleVect,nonTermInd-subRuleVect.begin()+1,subRuleVect.size()-1);
                std::vector<std::string> firstRes;
                if (!subRuleVect.empty()){
                    firstRes=first(subRuleVect);
                    if (std::find(firstRes.begin(), firstRes.end(),EPSILON)!=firstRes.end()){
                        firstRes.erase(std::remove(firstRes.begin(), firstRes.end(), EPSILON), firstRes.end());
                        auto ansNew = follow(curNonTerm);
                        firstRes.insert(firstRes.end(), ansNew.begin(), ansNew.end());
                    }
                } else {
                    if (nonTerm!=curNonTerm){
                        firstRes = follow(curNonTerm);
                    }
                }
                for (auto t : firstRes) {
                    solSet.insert(t);
                }
            }
        }
    }
    auto v = setToVect(solSet);
    return v;
}

void SLRTable::createParseTable() {
    std::vector<std::string> v(inputGrammar.NonTerms().size()+inputGrammar.Terms().size()+1);
    table = std::vector<std::vector<std::string>> (stateCount+1,v);

    auto nonTerms= inputGrammar.NonTerms();
    auto terms= inputGrammar.Terms();

    std::vector<std::string> cols = findCols();

    for (auto entry : GOTOStateDict){
        int state = entry.first.first;
        std::string token = entry.first.second;
        int col = std::find(cols.begin(), cols.end(),token) - cols.begin();
        if (nonTerms.find(token)!=nonTerms.end() || terms.find(token)!=terms.end()){
            table[state][col]+= std::to_string(GOTOStateDict[entry.first]);
        }
    }

    std::unordered_map<int,ExtendedRule> processed;
    int c=0;
    for (ExtendedRule rule : extendedGrammarRules){
        auto tmpRule=rule;
        tmpRule.RHS.erase(std::find(tmpRule.RHS.begin(), tmpRule.RHS.end(),DOT));
        processed[c]=tmpRule;
        c++;
    }

    std::string addedRule = extendedGrammarRules[0].LHS + " -> " + extendedGrammarRules[0].RHS[1];
    auto rules= inputGrammar.Rules();
    rules.emplace(rules.cbegin(),addedRule);
    for (auto rule : rules){
        std::stringstream ss(rule);
        std::string token;
        std::string lhs;
        std::string rhs;
        std::vector<std::string> multirhs;

        while (ss >> token) {
            if (lhs.empty()){
                lhs=token;
            } else if (token=="->"){
                continue;
            } else if (token == "|"){
                rhs.pop_back();
                multirhs.push_back(rhs);
                rhs.clear();
            } else {
                rhs+=token+" ";
            }
        }
        rhs.pop_back();
        multirhs.push_back(rhs);
        dict[lhs]=multirhs;
    }

    for (auto state : stateDict){
       int stateNum=state.first;
       auto rules = state.second;
       for (auto rule:rules){
           if (rule.RHS[rule.RHS.size()-1]==DOT){
               auto tmpRule = rule;
               tmpRule.RHS.erase(std::find(tmpRule.RHS.begin(), tmpRule.RHS.end(),DOT));
               for (auto proc : processed){
                   if (proc.second==tmpRule){
                       auto followRes= follow(rule.LHS);
                       for (auto col : followRes) {
                           int ind = std::find(cols.begin(), cols.end(),col)-cols.begin();
                           if (proc.first==0){
                               table[stateNum][ind]="acc";
                           } else {
                               table[stateNum][ind]+="R"+std::to_string(proc.first);
                           }
                       }
                   }
               }
           }
       }
    }
}

std::vector<int> SLRTable::getKeys(std::map<int,std::vector<ExtendedRule>> map) {
    std::vector<int> keys;
    for (const auto& record :map){
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