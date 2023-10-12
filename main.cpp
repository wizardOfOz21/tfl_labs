#include <getopt.h>
#include <iostream>
#include "project/include/StateMachine.h"
#include "project/include/RegexGenerator.h"
#include "project/include/StringGenerator.h"

struct InputData{
    std::string filePath;
    std::string regex;
};

static void showUsage(std::string name) {
    std::cout << "\nUsage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-h, --help\t\tShow this help message\n"
              << "\t-f, --file\t\tSpecify the test file path with regex string\n"
              << "\t-r, --regex\t\tSpecify the test regex string\n"
              << std::endl;
}

int parseFlags(int argc, char *argv[], InputData& data) {
  for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg=="--help" || arg=="-h") {
            showUsage(argv[0]);
        } else if (arg=="--file" || arg=="-f"){
            i++;
            if (i>=argc) return -1;
            data.filePath=argv[i];
        } else if (arg=="--regex" || arg=="-r") {
            i++;
            if (i>=argc) return -1;
            data.regex=argv[i];            
        } else {
            return -1;
        }
  }
  return 0;
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        showUsage(argv[0]);
        return 1;
    }
    InputData data;
    if (parseFlags(argc,argv,data)==-1){
        showUsage(argv[0]);
        return 1;
    }
    std::cout<<"Parse ok"<<std::endl;
}
