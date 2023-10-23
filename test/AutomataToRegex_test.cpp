#include <regex>
#include <vector>

#include "StateMachine.h"
#include "StringGenerator.h"
#include "gtest/gtest.h"

TEST(AutomataToRegex, Test1) {
  std::vector<std::vector<char>> vect1{{' ', 'b', ' ', 'b', 'a'},
                                       {' ', ' ', 'a', ' ', ' '},
                                       {' ', 'b', ' ', 'b', 'a'},
                                       {' ', 'b', ' ', 'b', 'a'},
                                       {' ', ' ', ' ', ' ', 'a'}};
  std::unordered_set<int> finalStates1{4, 3, 2, 0};
  StateMachine automata1(vect1, finalStates1, 4);
  auto converted = automata1.ConvertToRegularExpr();
  const std::regex r1("^" + converted + "$");
  const std::regex r2("^(ba|b)*a*$");
  StringGenerator sg;
  for (int i = 0; i < 1000; i++) {
    std::string curStr = sg.GenerateString(automata1);
    EXPECT_EQ(regex_match(curStr, r1), regex_match(curStr, r2));
  }
}

TEST(AutomataToRegex, Test2) {
  std::vector<std::vector<char>> vect1{
      {' ', 'b', ' ', 'b', 'a', 'c'}, {' ', ' ', 'a', ' ', ' ', ' '},
      {' ', 'b', ' ', 'b', 'a', ' '}, {' ', 'b', ' ', 'b', 'a', ' '},
      {' ', ' ', ' ', ' ', 'a', ' '}, {' ', ' ', ' ', ' ', ' ', ' '}};
  std::unordered_set<int> finalStates1{4, 3, 2, 0, 5};
  StateMachine automata1(vect1, finalStates1, 5);
  auto converted = automata1.ConvertToRegularExpr();
  const std::regex r1("^" + converted + "$");
  const std::regex r2("^(ba|b)*a*|c$");
  StringGenerator sg;
  for (int i = 0; i < 1000; i++) {
    std::string curStr = sg.GenerateString(automata1);
    EXPECT_EQ(regex_match(curStr, r1), regex_match(curStr, r2));
  }
}

TEST(AutomataToRegex, Test3) {  // 0   1   2   3   4   5
  std::vector<std::vector<char>> vect1{
      {' ', 'a', 'a', ' ', 'c', 'c'}, {' ', 'a', ' ', ' ', ' ', ' '},
      {' ', ' ', ' ', 'b', ' ', ' '}, {' ', ' ', 'a', ' ', 'c', ' '},
      {' ', ' ', 'a', ' ', 'c', ' '}, {' ', ' ', ' ', ' ', ' ', ' '}};
  std::unordered_set<int> finalStates1{1, 3, 4, 0, 5};
  StateMachine automata1(vect1, finalStates1, 5);
  auto converted = automata1.ConvertToRegularExpr();
  const std::regex r1("^" + converted + "$");
  const std::regex r2("^a*|(ab|c)*|c$");
  StringGenerator sg;
  for (int i = 0; i < 1000; i++) {
    std::string curStr = sg.GenerateString(automata1);
    EXPECT_EQ(regex_match(curStr, r1), regex_match(curStr, r2));
  }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
