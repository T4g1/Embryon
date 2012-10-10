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
}


// Mutateurs
/**
 * Donne la liste d'entité au niveau
 *
 * @param l_entity      Liste d'entitées chargée par irr::quake3
 */
void Level::setEntityList(const irr::scene::quake3::tQ3EntityList& l_entity)
{
    boost::mutex::scoped_lock l(mutexLevel);
    this->l_entity = &l_entity;

    for(int i=0; i<l_entity.size(); i++)
        cout << "loaded: entity \"" << l_entity[i].name.c_str() << "\"" << endl;
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

    irr::s32 index = l_entity->binary_search(search);
    if(index >= 0) {
        for(unsigned int i=0; i<(*l_entity)[index].getGroupSize(); i++) {
            const irr::scene::quake3::SVarGroup *group =
                    (*l_entity)[index].getGroup(i);

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
