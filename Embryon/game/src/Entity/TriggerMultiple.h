/** \file   TriggerMultiple.h
 *  \brief  Définit la classe TriggerMultiple
 */
#ifndef TRIGGERMULTIPLE
#define TRIGGERMULTIPLE

#include "Entity.h"


/** \class  TriggerMultiple
 *  \brief  Permet de stocker une entité de type trigger.
 *
 * Un trigger permet de déclencher une autre entité dans certaines conditions.
 */
class TriggerMultiple : public Entity
{
    public:
        TriggerMultiple(vector<Entity*>* l_entity,
                map<irr::core::stringc, irr::core::stringc> properties);
        virtual ~TriggerMultiple();

        void attachToNode(irr::scene::ISceneNode* node);

        // Callback
        virtual void onActivated(EnumEntityActivation activationType);
    protected:
    private:
};

#endif // TRIGGERMULTIPLE
