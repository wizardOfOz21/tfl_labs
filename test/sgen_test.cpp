#include <fstream>
#include <regex>

#include "RegexGenerator.h"
#include "StringGenerator.h"
#include "StateMachine.h"
#include "gtest/gtest.h"
#include "parser/parser2.hpp"


TEST(StringGenerator_Test, Generate_From_Regex_Test) {
    RegexGenerator generator(13, 2, 0, 2);
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
         std::cout << regex << std::endl;
        Parser r(regex.data(), regex.length());
        node_ptr R = r.Parse();
        EXPECT_EQ(R->type, NodeType::REGEX);
        StateMachine M = R->to_machine_dfs();;
        StringGenerator sg;
        for (int i = 0; i < 1000; i++) {
            std::string curStr = sg.GenerateString(M);
        }
    }
}

TEST(StringGenerator_Test, Generate_From_Extended_Regex_Test) {
    RegexGenerator generator(13, 2, 5, 2);
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
        // std::cout << regex << std::endl;
        Parser r(regex.data(), regex.length());
        node_ptr R = r.Parse();
        StateMachine M = R->to_machine_dfs();;
        StringGenerator sg;
        for (int i = 0; i < 1000; i++) {
            std::string curStr = sg.GenerateString(M);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
