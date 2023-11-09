#include "MATMock.h"
#include <sstream>
#include "Constant.h"

MATMock::MATMock(StateMachine& pref,StateMachine& suff): pref(pref),suff(suff){}

bool MATMock::IsMembership(const std::string& word,const std::string& mode){
    if (mode==PREFIX_MODE){
        return pref.IsWordBelong(word);
    }
   return suff.IsWordBelong(word);
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
                                              int len,int curIndex,const std::string& mode, std::string *res){
    if (*res != "equal"){
        return;
    }
    if(curIndex == len){
        auto perm=arrToStr(permutation);
        if (IsMembership(perm,mode) != M.IsWordBelong(perm)){
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
                            len,curIndex+1,mode,res);
        }
    }
}

std::string MATMock::IsEqual(StateMachine& M,std::string& alphabet,int maxLenOfWord,const std::string& mode) {
    for (int i=1;i<=maxLenOfWord;i++){
        std::vector<std::string> permutation(i);
        std::string res = "equal";
        checkPermutationsWithLen(M,alphabet,permutation,i,0,mode,&res);
        if (res!= "equal"){
            return res;
        }
    }
    return "equal";
}