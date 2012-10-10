/** \file   FuncDoor.cpp
 *  \brief  Implémente la classe FuncDoor
 */
#include "FuncDoor.h"


/**
 * Constructeur de la classe FuncDoor
 */
FuncDoor::FuncDoor(vector<Entity*>* l_entity,
        map<irr::core::stringc, irr::core::stringc> properties) :
    Entity(l_entity, properties)
{
    setName("func_door");
    setIsBlocEntity(true);
}


/**
 * Destructeur de la classe FuncDoor
 */
FuncDoor::~FuncDoor()
{
    //dtor
}


// Callback
/**
 * Appellée lorsque l'entité est activée
 */
void FuncDoor::onActivated(EnumEntityActivation activationType)
{
    // Si targetname est spécifié, seul un trigger peut ouvrir la porte
    if(getProperty("targetname") != "" && activationType != ENTITY_ACTIVATION_TRIGGERED)
        return;

    // Vérifie que la porte est stable
    if(!node->getAnimators().empty()) {
        irr::core::list<irr::scene::ISceneNodeAnimator*>::ConstIterator tempAnim = node->getAnimators().begin();
        irr::scene::ISceneNodeAnimator* anim = (irr::scene::ISceneNodeAnimator*)*tempAnim;

        if(!anim->hasFinished())
            return;

        // Detruit les anciennes animations
        node->removeAnimators();
    }

    irr::core::vector3df start = node->getPosition();
    irr::core::vector3df end = node->getPosition();
    end.Y += 100;

    irr::scene::ISceneNodeAnimator* anim;
    anim = node->getSceneManager()->createFlyStraightAnimator(
            start, end, 1000, false, true
    );

    node->addAnimator(anim);
    anim->drop();
}


/**
 * Appelé en cas de colision
 *
 * @param animator      Infos sur la colision
 */
bool FuncDoor::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    onActivated(ENTITY_ACTIVATION_COLLIDE);

    // Si on peut passer au travers
    if(getProperty("solid") == "false")
        return true;

    return false;
}
