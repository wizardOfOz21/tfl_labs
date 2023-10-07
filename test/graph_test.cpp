// #include <fstream>

// #include "parser/tset.hpp"
// #include "StateMachine.h"

#include "gtest/gtest.h"

// TEST(StateMachine_Test, To_Graph_Test) {
//     StateMachine M(3);
//     M.SetFinalStates({0,1});
//     M.AddTransition(0,'a',1);
//     M.AddTransition(0,'a',2);
//     M.AddTransition(1,'b',2);
//     std::ofstream f("graph");
//     StateMachine::To_Graph(M, f);
//     f.close();
// }

// TEST(TSet_Test, To_Graph_Test) {
//     cchar a1{'a', 1};
//     cchar a2{'a', 2};
//     cchar b3{'b', 3};
//     cchar b4{'b', 4};
//     cchar a5{'a', 5};
//     TSet a = a1;
//     TSet ab = a2;
//     ab.concat(b3);
//     ab.iter();
//     a.concat(ab);
//     a.iter();

//     TSet ba = b4;
//     ba.concat(a5);
//     ba.iter();
//     a.plus(ba);

//     a.num = 5;
//     StateMachine M = a.to_machine();
//     std::ofstream f("graph");
//     StateMachine::To_Graph(M, f);
//     f.close();
//     system("dot -Tjpg graph -O");
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
