/** \file   common.cpp
 *  \brief  Ne fait rien
 */
#include "common.h"

#include <string>
#include <vector>
#include <time.h>

using namespace std;


/**
 *  Fonction convertissant un wchar_t* en string
 *
 *  @param wcharstr    Chaine à convertir
 *
 *  @return     string contenant la chaine convertie
 */
string wchar_to_string(const wchar_t * wcharstr){
    wstring basicstring(wcharstr);
    string test(basicstring.begin(), basicstring.end());

    return test;
}


/**
 * Donne le temps en millisecondes
 *
 * @return      Temps en millisecondes
 */
irr::u32 getTime()
{
    float time = (float)clock();

    return (irr::u32)((time / (float)CLOCKS_PER_SEC) * 1000.f);
}
