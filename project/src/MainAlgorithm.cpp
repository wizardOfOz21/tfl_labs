#include "MainAlgorithm.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <unordered_set>

#include "Constant.h"

MainAlgorithm::MainAlgorithm(std::string& alphabet, int admissionToRegularity,
                             int maxLenOfWord,
                             int maxNumOfItersForSymmetricPump)
    : alphabet(alphabet),
      admissionToRegularity(admissionToRegularity),
      maxLenOfWord(maxLenOfWord),
      maxNumOfItersForSymmetricPump(maxNumOfItersForSymmetricPump) {}

void MainAlgorithm::fillLanguage(std::shared_ptr<IMAT> MAT,
                                 const std::string& mode) {
    int N = alphabet.size();
    std::vector<int> maxStateCount(alphabet.size());
    std::unordered_set<std::string> badWords;
    maxStateCount[0] = 0;
    for (int i = 1; i <= alphabet.size(); i++) {
        int K = i;
        std::string bitmask(K, 1);
        bitmask.resize(N, 0);
        do {
            std::string curStr;
            std::unordered_set<char> curLetter;
            for (int j = 0; j < N; ++j) {
                if (bitmask[j]) {
                    curStr += alphabet[j];
                    curLetter.insert(alphabet[j]);
                }
            }
            bool isBad = false;
            for (const auto& badPref : badWords) {
                bool needToSkip = false;
                for (auto letter : badPref) {
                    if (curLetter.find(letter) == curLetter.end()) {
                        needToSkip = true;
                        break;
                    }
                }
                if (!needToSkip) {
                    isBad = true;
                    break;
                }
            }
            if (isBad) {
                continue;
            }
            std::unique_ptr<StateMachine> automata =
                EquivalenceClassesTable::LStar(
                    curStr, maxStateCount[i - 1] + admissionToRegularity,
                    maxLenOfWord, MAT, mode);
            if (automata != nullptr) {
                automata->FixStates();
                if (automata->IsAnyCycle()) {
                    if (automata->GetStateNum() > maxStateCount[i]) {
                        maxStateCount[i] = automata->GetStateNum();
                    }
                    std::ofstream out(mode + "out_" + curStr);
                    StateMachine::To_Graph(*automata, out);
                    out.close();
                    if (mode == PREFIX_MODE) {
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

struct Fragment {
    std::string w0;
    std::string u1;
    std::string w2;
};

struct Pump {
    Fragment prefix;
    Fragment suffix;
};

using path = std::string;
using path_word = std::string;
using state_cycles = std::unordered_set<path_word>;
using automata_states_cycles = std::vector<state_cycles>;

struct FragmentHash {
    std::hash<std::string> hash_;
    size_t operator()(const Fragment& fragment) const {
        return hash_(fragment.w0 + fragment.u1 + fragment.w2);
    }
};

void MainAlgorithm::Run(const std::shared_ptr<IMAT>& MAT) {
    fillLanguage(MAT, PREFIX_MODE);
    fillLanguage(MAT, SUFFIX_MODE);
    if (prefixLanguage.empty() || suffixLanguage.empty()) {
        std::cout << "unreal" << std::endl;
    }

    int prefix_count = prefixLanguage.size();
    int suffix_count = suffixLanguage.size();

    std::vector<std::unordered_set<Fragment, FragmentHash>> prefixes_fragments(prefix_count);
    std::vector<std::unordered_set<Fragment, FragmentHash>> suffixes_fragments(suffix_count);

    for (auto pref: prefixLanguage) {
        int pref_size = pref->GetStateNum()+1;
        const std::unordered_set<int>& final = pref->GetFinalStates();
        automata_states_cycles prefixCycles(pref_size);
        pref->FindCycles(prefixCycles);
        for (int i = 0; i < pref_size; ++i) {
            std::unordered_set<path_word> paths_to_cycle = pref->FindPaths(0, i);
            std::unordered_set<path_word> paths_on_cycle;
            for (path p : prefixCycles[i]) {
                paths_on_cycle.insert(pref->GetPathWord(p));
            }
            std::unordered_set<path_word> paths_from_cycle = pref->FindPaths(i, final);
        }
    }
    // for (auto suff:suffixLanguage){
    //     std::vector<std::vector<std::string>>
    //     suffixCycles(suff->GetStateNum()+1); suff->FindCycles(suffixCycles);
    // }
}
