#include <iostream>
#include <fstream>
#include "MainAlgorithm.h"
#include "parser/parser2.hpp"

struct InputData{
    std::string alhabet;
    int admissionToRegularity;
    int maxLenOfWord;
    int maxNumOfItersForPump;
};

static void showUsage(const std::string& name) {
    std::cout << "\nUsage: " << name << " <option(s)>\n"
              << "You must spec options in any order:\n"
              << "\t-a,  --alphabet\t\tSpecify the alphabet (string like \"abc\", without repeating letters)\n"
              << "\t-ra, --regadm\t\tSpecify admission to regularity\n"
              << "\t-ml, --maxlen\t\tSpecify max len of word for equality query to MAT\n"
              << "\t-pi, --pumpit\t\tSpecify max num of iters for symmetric pump \n"
              << std::endl;
}

int parseFlags(int argc, char *argv[], InputData& data) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg=="--alphabet" || arg=="-a"){
            i++;
            if (i>=argc) return -1;
            data.alhabet=argv[i];
        } else if (arg=="--regadm" || arg=="-ra") {
            i++;
            if (i>=argc) return -1;
            data.admissionToRegularity=std::stoi(argv[i]);
        } else if (arg=="--maxlen" || arg=="-ml") {
            i++;
            if (i>=argc) return -1;
            data.maxLenOfWord=std::stoi(argv[i]);
        } else if (arg=="--pumpit" || arg=="-pi") {
            i++;
            if (i>=argc) return -1;
            data.maxNumOfItersForPump=std::stoi(argv[i]);
        } else {
            return -1;
        }
    }
    return 0;
}

StateMachine convertToStateMachine(const std::string& inputRegex) {
    Parser r(inputRegex.data(), inputRegex.length());
    node_ptr R = r.Parse();
    if (!R) {
        throw std::string{"Parse error"};
    }
    StateMachine M = R->to_machine_dfs();
    M.FixStates();
    return M;
}

StateMachine makePrefixLang(StateMachine& m){
    std::unordered_set<int> finalStatesPrefixes;
    for (int i=0;i<=m.GetStateNum();i++){
        finalStatesPrefixes.insert(i);
    }
    auto trans = m.GetTransitions();
    return {trans,finalStatesPrefixes,m.GetStateNum()};
}

StateMachine makeSuffixLang(StateMachine& lang,std::string &suffixLangRegex){
    std::unordered_set<int> finalStatesSuffixes;
    for (int i=0;i<=lang.GetStateNum();i++){
        finalStatesSuffixes.insert(i);
    }
    auto trans = convertToStateMachine(suffixLangRegex).GetTransitions();;
    return {trans,finalStatesSuffixes, lang.GetStateNum()};
}


int main(int argc, char *argv[]){
    // if (argc !=9) {
    //     showUsage(argv[0]);
    //     return 1;
    // }
    // InputData data;
    // parseFlags(argc,argv,data);
    InputData data{ "abc", 10, 5, 10 };

    std::string mainLangRegex="^(a|b)*a(c|a)$";
    std::string suffixLangRegex="^(c|a)a(a|b)*$";

    StateMachine lang = convertToStateMachine(mainLangRegex);
    StateMachine automataPrefixes = makePrefixLang(lang);
    StateMachine automataSuffixes = makeSuffixLang(lang,suffixLangRegex);

    MATMock m(lang, automataPrefixes, automataSuffixes);

    MainAlgorithm main(data.alhabet,data.admissionToRegularity,
                       data.maxLenOfWord,data.maxNumOfItersForPump);
    main.Run(std::make_unique<MATMock>(m));
    return 0;
}
