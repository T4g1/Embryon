/** \file   Entity.h
 *  \brief  Définit la classe Entity
 */
#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <irrlicht.h>

#include "../common.h"

using namespace std;

class Core;


/** \class  Entity
 *  \brief  Classe de base pour toutes les entitées utilisées dans la map.
 *
 * Jette les bases des interactions inter-entités (trigger, light, func_door,
 * ...).
 */
class Entity :
    public irr::scene::ICollisionCallback
{
    public:
        Entity(vector<Entity*>* l_entity,
                map<irr::core::stringc, irr::core::stringc> properties);
        virtual ~Entity();

        void attachToCore(Core* core);
        virtual void attachToNode(irr::scene::ISceneNode* node);
        bool isThisNode(irr::scene::ISceneNode* node);

        // Accesseurs
        string getName();
        irr::core::stringc getProperty(irr::core::stringc name);
        bool getIsBlocEntity();

        // Mutateurs
        void setName(string name);
        void setIsBlocEntity(bool isBlocEntity);

        // Callback
        virtual void onActivated(EnumEntityActivation activationType) =0;

        // Callback de collisions joueur
        bool onCollision (
                const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
    private:
    protected:
        Core* core;
        vector<Entity*>* l_entity;
        irr::scene::ISceneNode* node;

        string name;
        map<irr::core::stringc, irr::core::stringc> properties;
        bool isBlocEntity;

        bool targetCache;
        vector<int> l_targetEntities;

        void searchTargetEntities();
};

#endif // ENTITY_H
