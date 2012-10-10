/** \file   Level.h
 *  \brief  Définit la classe Level
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <boost/thread/mutex.hpp>
#include <irrlicht.h>
#include <map>
#include <vector>

#include "Entity/FuncButton.h"
#include "Entity/FuncDoor.h"
#include "Entity/TargetKill.h"
#include "Entity/TriggerMultiple.h"

using namespace std;


/** \class  Level
 *  \brief  Permet de stocker et de charger un niveau.
 *
 * Fournit différents accesseurs et mutateurs permettant de manipuler
 * le niveau.
 */
class Level
{
    public:
        Level();
        virtual ~Level();

        void loadEntityList(irr::scene::IQ3LevelMesh* meshMap);
        void initializeCollisionsEntities(irr::scene::ISceneManager* mSmgr,
                irr::scene::IQ3LevelMesh* meshMap,
                irr::scene::IAnimatedMeshSceneNode* nodePlayer);

        void attachEntitiesToCore(Core* core);
        void triggerEntityBySceneNode(irr::scene::ISceneNode* node,
                EnumEntityActivation activationType);

        // Mutateurs
        void setEntityList(const irr::scene::quake3::tQ3EntityList& l_entity);

        // Accesseurs
        map<irr::core::stringc, irr::core::stringc>
                getEntity(irr::core::stringc name);
    protected:
    private:
        boost::mutex mutexLevel;

        const irr::scene::quake3::tQ3EntityList* l_irrEntity;
        vector<Entity*> l_entity;

        void clearEntity();
};

#endif // LEVEL_H
