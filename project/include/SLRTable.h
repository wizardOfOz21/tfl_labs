#pragma once
#include "string"
#include "Grammar.h"
#include "unordered_map"
#include "map"
#include <iostream>

struct ExtendedRule {
    std::string LHS;
    std::vector<std::string> RHS;

    bool operator==(const ExtendedRule& rule) const{
        if (rule.LHS==LHS) {
            if (rule.RHS.size()!=RHS.size()){
                return false;
            }
            for (int i=0;i<rule.RHS.size();i++){
                if (rule.RHS[i]!=RHS[i]){
                    return false;
                }
            }
            return true;
        }
        return false;
    }
};

struct Actions {
    std::vector<int> shiftActions;
    std::vector<ExtendedRule> reduceActions;
};

class SLRTable{
private:
    struct hash_pair {
        template <class T1, class T2>
        size_t operator()(const std::pair<T1, T2>& p) const
        {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);

            if (hash1 != hash2) {
                return hash1 ^ hash2;
            }

            return hash1;
        }
    };

    struct Comp{
        bool operator()(const std::string& lhs, const std::string& rhs) const{
//            std::cout<<lhs<<" "<<lhs[lhs.size()-1]<<" "<<rhs<<std::endl;
            if (std::string(1,lhs[lhs.size()-1])=="'"){
                return true;
            }
            if (std::string(1,rhs[rhs.size()-1])=="'"){
                return false;
            }
            if (lhs<rhs){
                return true;
            }
            return false;
        }
    };


    Grammar inputGrammar;
    std::vector<ExtendedRule> extendedGrammarRules;
    std::map<int,std::vector<ExtendedRule>> stateDict;
    std::unordered_map<std::pair<int,std::string>,int, hash_pair> GOTOStateDict;
    std::vector<std::vector<std::string>> table;
    std::vector<std::string> cols;

    std::map<std::string,std::vector<std::string>,Comp> dict;

    std::string newStartToken;
    int stateCount=0;

    void processGrammar();
    std::vector<ExtendedRule> findClosure(std::vector<ExtendedRule>& inpState, std::string& tokenAfterDOT);
    void computeGOTO(int state);
    void GOTO(int state,const std::string& token);
    void generateStates();
    void createParseTable();
    std::vector<std::string> follow(const std::string& nonTerm, std::unordered_set<std::string>& used);
    std::vector<std::string> first(std::vector<std::string>& rule);
    void printTable();

    bool isBelong(ExtendedRule rule,std::vector<ExtendedRule> arr);
    std::vector<int> getKeys(std::map<int,std::vector<ExtendedRule>>);
    std::vector<std::string> findCols();
public:
    explicit SLRTable(Grammar grammar);
    SLRTable()=delete;

    Actions GetActions(int state, std::string token);


    ~SLRTable()=default;
};
