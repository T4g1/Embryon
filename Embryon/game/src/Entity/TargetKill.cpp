/** \file   TargetKill.cpp
 *  \brief  Implémente la classe TargetKill
 */
#include "TargetKill.h"


/**
 * Constructeur de la classe TargetKill
 */
TargetKill::TargetKill(vector<Entity*>* l_entity,
        map<irr::core::stringc, irr::core::stringc> properties) :
    Entity(l_entity, properties)
{
    setName("target_kill");
}


/**
 * Destructeur de la classe TargetKill
 */
TargetKill::~TargetKill()
{
    //dtor
}


// Callback
/**
 * Appellée lorsque l'entité est activée
 */
void TargetKill::onActivated(EnumEntityActivation activationType)
{
    // Activé par une autre entité
    if(activationType != ENTITY_ACTIVATION_TRIGGERED)
        return;

    cout << getName() << ": Die bitch !" << endl;
}
