/** \file   module_message.h
 *  \brief  Définit la classe module_message
 */
#ifndef MODULE_MESSAGE_H
#define MODULE_MESSAGE_H

#include <iostream>
#include <map>

#include "common.h"

using namespace std;


/** \class  module_message
 *  \brief  Contient toutes les informations concernant un message.
 *
 * Les messages sont envoyés par un Module ou le Core vers un autre Module ou
 * le Core. Ils permettent aux modules de communiquer entre-eux.
 */
class module_message
{
    public:
        module_message(
                EnumModuleId idExpediteur, EnumModuleId idDestination,
                EnumCodesAction codeAction=ACTION_AUCUNE);
        ~module_message();

        EnumModuleId idExpediteur;
        EnumModuleId idDestination;

        EnumCodesAction codeAction;     // Action que l'on souhaite effectuer

        map<string, int> intData;
        map<string, string> strData;
};

#endif // MODULE_MESSAGE_H
