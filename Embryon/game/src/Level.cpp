/** \file   Level.cpp
 *  \brief  Implémente la classe Level
 */
#include "Level.h"


/**
 * Constructeur de l'objet Level
 */
Level::Level()
{
}

/**
 * Destructeur de l'objet Level
 */
Level::~Level()
{
    clearEntity();
}


/**
 * Crée un objet adapté a chaque entité.
 *
 * @param meshMap      Informations concernant le niveau chargé
 */
void Level::loadEntityList(irr::scene::IQ3LevelMesh* meshMap)
{
    clearEntity();

    boost::mutex::scoped_lock l(mutexLevel);

    irr::scene::quake3::tQ3EntityList l_irrEntity = meshMap->getEntityList();

    for(unsigned int i=0; i<l_irrEntity.size(); i++) {
        const irr::scene::quake3::SVarGroup *group = l_irrEntity[i].getGroup(1);

        // Charge toutes les proprietés de l'entité
        map<irr::core::stringc, irr::core::stringc> properties;
        for(unsigned int j=0; j<group->Variable.size(); j++) {
            irr::scene::quake3::SVariable var = group->Variable[j];
            properties[var.name] = var.content;
        }

        // Crée l'entité correspondante
        if(properties["classname"] == "trigger_multiple")
            l_entity.push_back(new TriggerMultiple(&l_entity, properties));
        else if(properties["classname"] == "target_kill")
            l_entity.push_back(new TargetKill(&l_entity, properties));
        else if(properties["classname"] == "func_door")
            l_entity.push_back(new FuncDoor(&l_entity, properties));
        else if(properties["classname"] == "func_button")
            l_entity.push_back(new FuncButton(&l_entity, properties));
    }
}


/**
 * Initialise les collisions des entités.
 *
 * @param mSmgr         Scene manager
 * @param meshmap       Niveau chargé
 * @param nodePlayer    Node du joueur (pour les collisions)
 */
void Level::initializeCollisionsEntities(irr::scene::ISceneManager* mSmgr,
        irr::scene::IQ3LevelMesh* meshMap,
        irr::scene::IAnimatedMeshSceneNode* nodePlayer)
{
    for(unsigned int i=0; i<l_entity.size(); i++) {
        Entity* entity = l_entity[i];

        // Si ce n'est pas une entitée bloc
        if(!entity->getIsBlocEntity())
            continue;

        irr::core::stringc model = entity->getProperty("model");

        // Entité bloc, récupére son model
        irr::scene::IMesh *mesh = 0;
        irr::s32 modelId = atoi(model.subString(1, model.size()).c_str());

        mesh = meshMap->getBrushEntityMesh(modelId);

        if (!mesh)
            cout << "Entite bloc erronee: Aucun model associe !" << endl;
        else {
            // Ajout du bloc
            irr::scene::ISceneNode* node =
                    mSmgr->addMeshSceneNode(mesh, mSmgr->getRootSceneNode(), SCENE_NODE_ENTITY);
            //node->setDebugDataVisible(irr::scene::EDS_BBOX);
            node->setMaterialFlag(irr::video::EMF_LIGHTING, true);

            entity->attachToNode(node);

            // Selecteur
            irr::scene::ITriangleSelector* selector;
            selector = mSmgr->createTriangleSelectorFromBoundingBox(node);
            node->setTriangleSelector(selector);

            // Collision reponse
            irr::scene::ISceneNodeAnimatorCollisionResponse* anim;
            anim = mSmgr->createCollisionResponseAnimator(
                    selector, nodePlayer,
                    irr::core::vector3df(30,50,30),
                    irr::core::vector3df(0,0,0),
                    irr::core::vector3df(0,0,0)
            );
            anim->setCollisionCallback(entity);

            selector->drop();
            nodePlayer->addAnimator(anim);
            anim->drop();
        }
    }
}


/**
 * Permet de lier le core aux entités afin qu'elle puisse agir sur le
 * jeu.
 *
 * @param core          Modéle contenant toutes les données du jeu
 */
void Level::attachEntitiesToCore(Core* core)
{
    for(unsigned int i=0; i<l_entity.size(); i++)
        l_entity[i]->attachToCore(core);
}


/**
 * Déclenche l'entité correspondant au scene node donné
 *
 * @param node                  Scene node de l'entité
 * @param activationType        Type d'activation souhaitée
 */
void Level::triggerEntityBySceneNode(irr::scene::ISceneNode* node,
        EnumEntityActivation activationType)
{
    for(unsigned int i=0; i<l_entity.size(); i++) {
        Entity* entity = l_entity[i];

        if(entity->isThisNode(node))
            entity->onActivated(activationType);
    }
}


// Mutateurs
/**
 * Donne la liste d'entité au niveau
 *
 * @param l_irrEntity      Liste d'entitées chargée par irr::quake3
 */
void Level::setEntityList(const irr::scene::quake3::tQ3EntityList& l_irrEntity)
{
    boost::mutex::scoped_lock l(mutexLevel);
    this->l_irrEntity = &l_irrEntity;

    for(unsigned int i=0; i<l_irrEntity.size(); i++)
        cout << "loaded: entity \"" << l_irrEntity[i].name.c_str() << "\"" << endl;
}


// Accesseurs
/**
 * Donne l'entitée demandée
 *
 * @param name          Nom de l'entité
 *
 * @return              Container associant le nom des paramétres de l'entitée
 *                      a leurs valeurs
 */
map<irr::core::stringc, irr::core::stringc>
        Level::getEntity(irr::core::stringc name)
{
    boost::mutex::scoped_lock l(mutexLevel);
    map<irr::core::stringc, irr::core::stringc> entity;

    irr::scene::quake3::IEntity search;
    search.name = name;

    cout << "Load entity: " << name.c_str() << endl;    // DEBUG

    irr::s32 index = l_irrEntity->binary_search(search);
    if(index >= 0) {
        for(unsigned int i=0; i<(*l_irrEntity)[index].getGroupSize(); i++) {
            const irr::scene::quake3::SVarGroup *group =
                    (*l_irrEntity)[index].getGroup(i);

            cout << "Groupe " << i << endl;

            for(unsigned int j=0; j<group->Variable.size(); j++) {
                cout    << group->Variable[j].name.c_str() << " = "     // DEBUG
                        << group->Variable[j].content.c_str() << endl;  // DEBUG
                entity[group->Variable[j].name] = group->Variable[j].content;
            }
        }
    } else
        cout << "Load entity: not found." << endl;    // DEBUG

    return entity;
}


/**
 * Supprimme les entités
 */
void Level::clearEntity()
{
    // Detruit les entités
    for(unsigned int i=0; i<l_entity.size(); i++) {
        if(l_entity[i])
            delete l_entity[i];
    }

    l_entity.clear();
}
