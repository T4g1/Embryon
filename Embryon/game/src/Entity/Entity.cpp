/** \file   Entity.cpp
 *  \brief  Implémente la classe Entity
 */
#include "Entity.h"

#include "../Core/Core.h"


/**
 * Constructeur de Entity
 *
 * @param properties            Propiétés de l'entité
 */
Entity::Entity(vector<Entity*>* l_entity,
        map<irr::core::stringc, irr::core::stringc> properties)
{
    setName("Entity");
    setIsBlocEntity(false);

    this->l_entity = l_entity;
    this->properties = properties;

    targetCache = false;
}


/**
 * Destructeur de Entity
 */
Entity::~Entity()
{
    //dtor
}


/**
 * Lie l'entité au Core
 */
void Entity::attachToCore(Core* core)
{
    this->core = core;
}


/**
 * lie l'netité a la node qui la représente
 *
 * @param node      Node de l'entité
 */
void Entity::attachToNode(irr::scene::ISceneNode* node)
{
    this->node = node;
}


/**
 * Vérifie si le node est celui de l'entité
 *
 * @param node      Node a verifier
 *
 * @return          True si le node correspond
 */
bool Entity::isThisNode(irr::scene::ISceneNode* node)
{
    return (this->node == node);
}


// Accesseurs
/**
 * Donne le nom de l'entité.
 *
 * @return                  Nom de l'entité
 */
string Entity::getName()
{
    return name;
}


/**
 * Donne la propriété voulue
 *
 * @param name      Nom de la propriété
 *
 * @return          Chaine vide si la propriété n'existe pas
 */
irr::core::stringc Entity::getProperty(irr::core::stringc name)
{
    if(properties.find(name) == properties.end())
        return "";

    return properties[name];
}


/**
 * Indique s'il s'agit d'une entité bloc ou non
 */
bool Entity::getIsBlocEntity()
{
    return isBlocEntity;
}


// Mutateurs
/**
 * Modifie le nom de l'entité
 *
 * @param name              Nouveau nom
 */
void Entity::setName(string name)
{
    this->name = name;
}


/**
 * Modifie le type de bloc
 */
void Entity::setIsBlocEntity(bool isBlocEntity)
{
    this->isBlocEntity = isBlocEntity;
}


/**
 * Appelé en cas de colision
 *
 * @param animator      Infos sur la colision
 */
bool Entity::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    onActivated(ENTITY_ACTIVATION_COLLIDE);

    return true;
}


/**
 * Trouve les entité lié a celle-ci
 */
void Entity::searchTargetEntities()
{
    targetCache = true;

    irr::core::stringc target = getProperty("target");
    if(target == "")
        return;

    for(unsigned int i=0; i<l_entity->size(); i++) {
        Entity* entity = (*l_entity)[i];

        if(entity->getProperty("targetname") == target)
            l_targetEntities.push_back(i);
    }
}

