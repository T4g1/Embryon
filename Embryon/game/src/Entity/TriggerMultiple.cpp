/** \file   TriggerMultiple.cpp
 *  \brief  Implémente la classe TriggerMultiple
 */
#include "TriggerMultiple.h"


/**
 * Constructeur de la classe TriggerMultiple
 */
TriggerMultiple::TriggerMultiple(vector<Entity*>* l_entity,
        map<irr::core::stringc, irr::core::stringc> properties) :
    Entity(l_entity, properties)
{
    setName("trigger_multiple");
    setIsBlocEntity(true);
}


/**
 * Destructeur de la classe TriggerMultiple
 */
TriggerMultiple::~TriggerMultiple()
{
    //dtor
}


/**
 * lie l'netité a la node qui la représente
 *
 * @param node      Node de l'entité
 */
void TriggerMultiple::attachToNode(irr::scene::ISceneNode* node)
{
    this->node = node;
    this->node->setVisible(false);
}


// Callback
/**
 * Appellée lorsque l'entité est activée
 */
void TriggerMultiple::onActivated(EnumEntityActivation activationType)
{
    // Activé par collision
    if(activationType != ENTITY_ACTIVATION_COLLIDE)
        return;

    if(!targetCache)
        searchTargetEntities();

    // Active les entités ciblées
    for(unsigned int i=0; i<l_targetEntities.size(); i++)
        (*l_entity)[l_targetEntities[i]]->onActivated(ENTITY_ACTIVATION_TRIGGERED);
}
