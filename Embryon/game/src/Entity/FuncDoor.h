/** \file   FuncDoor.h
 *  \brief  Définit la classe FuncDoor
 */
#ifndef FUNCDOOR_H
#define FUNCDOOR_H

#include "Entity.h"



/** \class  FuncDoor
 *  \brief  Permet de stocker une entité de type func_door.
 *
 * Cette entité correspond a une porte qui s'ouvre par translation.
 * On peut créer de fausse porte en placant targetname avec une valeur
 * non vide et l'attribut "solid" a "false".
 *
 * Lorsque targetname est spécifié, la porte ne peut être activée via le
 * boutton d'action.
 */
class FuncDoor : public Entity
{
    public:
        FuncDoor(vector<Entity*>* l_entity,
                map<irr::core::stringc, irr::core::stringc> properties);
        virtual ~FuncDoor();

        // Callback
        virtual void onActivated(EnumEntityActivation activationType);
        bool onCollision (
                const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
    protected:
    private:
};

#endif // FUNCDOOR_H
