#include "MATMock.h"
#include <sstream>

MATMock::MATMock(StateMachine& m): ideal(m){}

bool MATMock::IsMembership(const std::string& word){
   return ideal.IsWordBelong(word);
}

std::string arrToStr(std::vector<std::string>& permutation){
    std::string res;
    for (auto str: permutation){
        res+=str;
    }
    return res;
}

void MATMock::checkPermutationsWithLen(StateMachine& M,
                                              const std::string& alphabet,std::vector<std::string>& permutation,
                                              int len,int curIndex, std::string *res){
    if (*res != "equal"){
        return;
    }
    if(curIndex == len){
        auto perm=arrToStr(permutation);
        if (IsMembership(perm) != M.IsWordBelong(perm)){
            *res= perm;
        }
    }
    else{
        for(int i = 0; i < alphabet.size(); i++){
            if (*res != "equal"){
                return;
            }
            permutation[curIndex] = std::string(1,alphabet[i]);
            checkPermutationsWithLen(M,alphabet,permutation,
                            len,curIndex+1,res);
        }
    }
}

std::string MATMock::IsEqual(StateMachine& M,std::string& alphabet,int maxLenOfWord) {
    for (int i=1;i<=maxLenOfWord;i++){
        std::vector<std::string> permutation(i);
        std::string res = "equal";
        checkPermutationsWithLen(M,alphabet,permutation,i,0,&res);
        if (res!= "equal"){
            return res;
        }
    }
    return "equal";
}