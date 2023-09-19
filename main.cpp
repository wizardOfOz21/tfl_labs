#include "include.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "usage: gen {out_file_name} {rules_file_name}" << std::endl; 
        return 0;
    }
    std::ostream* out;
    std::ofstream smt_spec(argv[1]);
    out = &smt_spec;
    vector<rule> rules;

    std::ifstream Rules(argv[2]);
    get_rules(rules, Rules);
    Rules.close();

    std::ifstream head("head.smt2");
    *out << head.rdbuf();
    head.close();

    set alf;
    for (rule r : rules) {
        *out << make_condition(r.first, r.second, alf) << std::endl;
    }

    std::ifstream tail("tail.smt2");
    *out << tail.rdbuf();
    tail.close();
    smt_spec.close();
    return 0;
}
