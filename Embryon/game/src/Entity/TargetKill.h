#ifndef TARGETKILL_H
#define TARGETKILL_H

#include "Entity.h"


class TargetKill : public Entity
{
    public:
        TargetKill(vector<Entity*>* l_entity,
                map<irr::core::stringc, irr::core::stringc> properties);
        virtual ~TargetKill();

        // Callback
        virtual void onActivated(EnumEntityActivation activationType);
    protected:
    private:
};

#endif // TARGETKILL_H
