#include <fstream>
#include <regex>

#include "RegexGenerator.h"
#include "StringGenerator.h"
#include "StateMachine.h"
#include "gtest/gtest.h"
#include "parser/parser2.hpp"
#include "parser/tset.hpp"
// #include "parser/parser.hpp"

TEST(Basic_Test, Parse_Test) {
    RegexGenerator generator;
    for (int i = 0; i < 1000; ++i) {
        string regex = generator.GenerateRegex();
        // std::cout << regex << std::endl;
        Parser r(regex.data(), regex.length(), {'a', 'b', 'c'});
        node_ptr R = r.Parse();
        EXPECT_TRUE(R);
    }
}

TEST(Basic_Test, Convert_Test) {
    RegexGenerator generator;
    for (int i = 0; i < 100; ++i) {
        string regex = generator.GenerateRegex();
        // std::cout << regex << std::endl;
        Parser r(regex.data(), regex.length(), {'a', 'b', 'c'});
        node_ptr R = r.Parse();
        StateMachine M = R->to_machine_dfs();
    }
}

TEST(Basic_Test, Match_Regular_Test) {
    RegexGenerator generator(6, 2, 0, 2);
    for (int i = 0; i < 10; ++i) {
        string regex = generator.GenerateRegex();
        // std::cout << regex << std::endl;
        Parser r(regex.data(), regex.length(), {'a', 'b', 'c'});
        node_ptr R = r.Parse();
        EXPECT_EQ(R->type, NodeType::REGEX);
        StateMachine M = R->to_machine_dfs();
        std::regex r1(regex);
        std::regex r2(M.ConvertToRegularExpr());
        StringGenerator sg;
        for (int i = 0; i < 100; i++) {
            std::string curStr = sg.GenerateString(M);
            bool r1_match = regex_match(curStr, r1);
            std::cout << r1_match << std::endl; 
            EXPECT_EQ(r1_match, regex_match(curStr, r2));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
