#pragma once
#include "string"
#include "Grammar.h"
#include "unordered_map"

class SLRTable{
private:
    struct ExtendedRule {
        std::string LHS;
        std::vector<std::string> RHS;

        bool operator==(const ExtendedRule& rule) const{
            if (rule.LHS==LHS) {
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


    Grammar inputGrammar;
    std::vector<ExtendedRule> extendedGrammarRules;
    std::unordered_map<int,std::vector<ExtendedRule>> stateDict;
    std::unordered_map<std::pair<int,std::string>,int, hash_pair> stateDict2;
    std::string newStartToken;
    int stateCount=0;

    void processGrammar();
    std::vector<ExtendedRule> findClosure(std::vector<ExtendedRule>& inpState, std::string& tokenAfterDOT);
    void computeGOTO(int state);
    void GOTO(int state,const std::string& token);
    void generateStates();

    bool isBelong(ExtendedRule rule,std::vector<ExtendedRule> arr);
    std::vector<int> getKeys(std::unordered_map<int,std::vector<ExtendedRule>>);
public:
    explicit SLRTable(Grammar grammar);
    SLRTable()=default;


    ~SLRTable()=default;
};
