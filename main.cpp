#include <iostream>
#include "project/include/RegexToGlushkovConverter.h"

int main(){
//    std::cout<<"Hello world"<<std::endl;
    RegexToGlushkovConverter a("(ba|b)aa(a|ab)*");
    auto follow = a.computeFollow();
}
