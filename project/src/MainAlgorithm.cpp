#include <fstream>
#include "MainAlgorithm.h"
#include "Constant.h"
#include <algorithm>
#include <unordered_set>

MainAlgorithm::MainAlgorithm(std::string& alphabet,int admissionToRegularity,
              int maxLenOfWord,int maxNumOfItersForSymmetricPump)
              :alphabet(alphabet),admissionToRegularity(admissionToRegularity),
              maxLenOfWord(maxLenOfWord),maxNumOfItersForSymmetricPump(maxNumOfItersForSymmetricPump){}

void MainAlgorithm::fillLanguage(std::shared_ptr<IMAT> MAT, const std::string& mode){
    int N=alphabet.size();
    std::vector<int> maxStateCount(alphabet.size());
    std::unordered_set<std::string> badWords;
    maxStateCount[0]=0;
    for (int i=1;i<=alphabet.size();i++){
        int K=i;
        std::string bitmask(K, 1);
        bitmask.resize(N, 0);
        do {
            std::string curStr;
            std::unordered_set<char> curLetter;
            for (int j = 0; j < N; ++j){
                if (bitmask[j]){
                    curStr+=alphabet[j];
                    curLetter.insert(alphabet[j]);
                }
            }
            bool isBad = false;
            for (const auto& badPref: badWords){
                bool needToSkip= false;
                for (auto letter : badPref){
                    if (curLetter.find(letter)==curLetter.end()){
                        needToSkip= true;
                        break;
                    }
                }
                if (!needToSkip){
                    isBad= true;
                    break;
                }
            }
            if (isBad){
                continue;
            }
            std::unique_ptr<StateMachine> automata = EquivalenceClassesTable::LStar
                    (curStr,maxStateCount[i-1]+admissionToRegularity,
                     maxLenOfWord,MAT,mode);
            if (automata!= nullptr){
                automata->FixStates();
                if (automata->IsAnyCycle()){
                    if (automata->GetStateNum()>maxStateCount[i]){
                        maxStateCount[i]=automata->GetStateNum();
                    }
                    std::ofstream out(mode+"out_"+curStr);
                    StateMachine::To_Graph(*automata, out);
                    out.close();
                    if (mode==PREFIX_MODE){
                        prefixLanguage.emplace_back(std::move(automata));
                        prefixLanguageStr.emplace_back(curStr);
                    } else {
                        suffixLanguage.emplace_back(std::move(automata));
                        suffixLanguageStr.emplace_back(curStr);
                    }
                }
            } else {
                badWords.insert(curStr);
            }
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }
    return;
}


void MainAlgorithm::Run(const std::shared_ptr<IMAT>& MAT){
    fillLanguage(MAT,PREFIX_MODE);
    fillLanguage(MAT, SUFFIX_MODE);
    if (prefixLanguage.empty() || suffixLanguage.empty()){
        std::cout<<"unreal";
    }
    for (auto pref:prefixLanguage){
        std::vector<std::unordered_set<std::string>> prefixCycles(pref->GetStateNum()+1);
        pref->FindCycles(prefixCycles);
        int a=0;
        for (auto suff:suffixLanguage){
            std::vector<std::unordered_set<std::string>> suffixCycles(suff->GetStateNum()+1);
            suff->FindCycles(suffixCycles);
        }
    }
}
