#include <iostream>
#include <fstream>
#include "MainAlgorithm.h"

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


int main(int argc, char *argv[]){
    if (argc !=9) {
        showUsage(argv[0]);
        return 1;
    }
    InputData data;
    parseFlags(argc,argv,data);

    std::vector<std::vector<std::string>> vect1{{"", "a", "b", "a", "",""},
                                                {"", "a", "b", "a", "",""},
                                                {"", "a", "b", "a", "",""},
                                                {"", "", "", "", "a","b"},
                                                {"", "", "", "", "",""},
                                                {"", "", "", "", "",""}};
    std::unordered_set<int> finalStates1{4, 5};
    StateMachine automata1(vect1, finalStates1, 5);
    std::ofstream out("res");
    StateMachine::To_Graph(automata1,out);
    MATMock m(automata1);

    MainAlgorithm main(data.alhabet,data.admissionToRegularity,
                       data.maxLenOfWord,data.maxNumOfItersForPump);
    main.Run(std::make_unique<MATMock>(m));
    return 0;
}
