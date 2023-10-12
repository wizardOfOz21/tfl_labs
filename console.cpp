#include <filesystem>
#include <fstream>
#include <iostream>

#include "parser/parser2.hpp"
#include "project/include/RegexGenerator.h"
#include "project/include/StateMachine.h"
#include "project/include/StringGenerator.h"

int main() {
    std::cout << "Enter regex, 'stop' or 'save_stop' command" << std::endl;
    string cmd;
    system("mkdir tmp");
    while (true) {
        std::cin >> cmd;
        if (cmd == "stop") break;
        if (cmd == "save_stop") return 0;
        cmd = "^" + cmd + "$";
        Parser r(cmd.data(), cmd.length(), {'a', 'b', 'c'});
        node_ptr R = r.Parse();
        if (!R) {
            std::cout << "parse error" << std::endl;
            continue;
        }
        StateMachine M = R->to_machine_dfs();
        std::cout << "regex: " << std::endl;
        std::cout << M.ConvertToRegularExpr() << std::endl;
        std::ofstream machine("tmp/machine");
        StateMachine::To_Graph(M, machine);
        machine.close();
        system("dot -Tpng tmp/machine -o tmp/machine.png");
        std::ofstream tree("tmp/tree");
        R->to_graph(tree);
        tree.close();
        system("dot -Tpng tmp/tree -o tmp/tree.png");
    }
    std::filesystem::remove_all("tmp");
}
