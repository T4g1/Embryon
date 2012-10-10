/** \file   common.cpp
 *  \brief  Ne fait rien
 */
#include <string>
#include <vector>

using namespace std;

/**
 *  Fonction convertissant un wchar_t* en string
 *
 *  @param *wcharstr    Chaine à transformer
 *
 *  @return     string contenant la chaine convertie
 */

string wchar_to_string(const wchar_t * wcharstr){
    wstring basicstring(wcharstr);
    //basicstring += L" (basic_string)";
    string test(basicstring.begin(), basicstring.end());

    return test;
}




#include "common.h"
