/** \file   FuncButton.h
 *  \brief  Définit la classe FuncButton
 */
#ifndef FUNCBUTTON_H
#define FUNCBUTTON_H

#include "Entity.h"



/** \class  FuncButton
 *  \brief  Permet de stocker une entité de type func_button.
 *
 * Cette entité correspond a un boutton
 */
class FuncButton : public Entity
{
    public:
        FuncButton(vector<Entity*>* l_entity,
                map<irr::core::stringc, irr::core::stringc> properties);
        virtual ~FuncButton();

        // Callback
        virtual void onActivated(EnumEntityActivation activationType);
        bool onCollision (
                const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
    protected:
    private:
};

#endif // FUNCBUTTON_H
