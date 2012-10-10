/** \file   FuncButton.cpp
 *  \brief  Implémente la classe FuncButton
 */
#include "FuncButton.h"


/**
 * Constructeur de la classe FuncButton
 */
FuncButton::FuncButton(vector<Entity*>* l_entity,
        map<irr::core::stringc, irr::core::stringc> properties) :
    Entity(l_entity, properties)
{
    setName("func_button");
    setIsBlocEntity(true);
}


/**
 * Destructeur de la classe FuncButton
 */
FuncButton::~FuncButton()
{
    //dtor
}


// Callback
/**
 * Appellée lorsque l'entité est activée
 */
void FuncButton::onActivated(EnumEntityActivation activationType)
{
    // Si on fonce dedans, le bouton ne s'active pas
    if(activationType == ENTITY_ACTIVATION_COLLIDE)
        return;

    if(!targetCache)
        searchTargetEntities();

    // Active les entités ciblées
    for(unsigned int i=0; i<l_targetEntities.size(); i++)
        (*l_entity)[l_targetEntities[i]]->onActivated(ENTITY_ACTIVATION_TRIGGERED);
}


/**
 * Appelé en cas de colision
 *
 * @param animator      Infos sur la colision
 */
bool FuncButton::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    return false;
}
