/** \file   module_message.cpp
 *  \brief  Implémente la classe module_message
 */
#include "module_message.h"


/**
 * Constructeur de module_message.
 *
 * @param idExpediteur      Identifiant du module expediteur
 * @param idDestination     Identifiant du module de destination
 * @param codeAction        Optionnel, designe l'action a réaliser
 */
module_message::module_message(
        EnumModuleId idExpediteur, EnumModuleId idDestination,
        EnumCodesAction codeAction)
{
    intData = map<string, int>();

    this->idExpediteur = idExpediteur;
    this->idDestination = idDestination;
    this->codeAction = codeAction;
}

/**
 * Destructeur du module_message
 */
module_message::~module_message()
{
    //dtor
}
