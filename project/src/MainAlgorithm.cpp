#include <fstream>
#include "MainAlgorithm.h"

MainAlgorithm::MainAlgorithm(std::string& alphabet,int admissionToRegularity,
              int maxLenOfWord,int maxNumOfItersForSymmetricPump)
              :alphabet(alphabet),admissionToRegularity(admissionToRegularity),
              maxLenOfWord(maxLenOfWord),maxNumOfItersForSymmetricPump(maxNumOfItersForSymmetricPump){}

void MainAlgorithm::fillPrefixLanguage(std::shared_ptr<IMAT> MAT){
    int N=alphabet.size();
    std::vector<int> maxStateCount(alphabet.size());
    std::unordered_set<std::string> badPrefixes;
    maxStateCount[0]=0;
    for (int i=1;i<=alphabet.size();i++){
        int K=i;
        std::string bitmask(K, 1);
        bitmask.resize(N, 0);
        do {
            std::string curStr;
            for (int j = 0; j < N; ++j){
                if (bitmask[j]) curStr+=alphabet[j];
            }
            std::sort(curStr.begin(), curStr.end());
            for (const auto& badPref: badPrefixes){
                if (curStr.find(badPref) != std::string::npos) {
                    continue;
                }
            }
            std::unique_ptr<StateMachine> automata = EquivalenceClassesTable::LStar
                    (curStr,maxStateCount[i-1]+admissionToRegularity,
                     maxLenOfWord,MAT);
            if (automata!= nullptr){
                std::ofstream out("out_"+curStr);
                StateMachine::To_Graph(*automata, out);
                out.close();
                if (automata->IsAnyCycle()){
                    if (automata->GetStateNum()>maxStateCount[i]){
                        maxStateCount[i]=automata->GetStateNum();
                    }
                    prefixLanguage.emplace_back(std::move(automata));
                }
            } else {
                badPrefixes.insert(curStr);
            }
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }
}


void MainAlgorithm::Run(const std::shared_ptr<IMAT>& MAT){
    fillPrefixLanguage(MAT);
    int a=1;
}