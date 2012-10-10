/** \file   Level.h
 *  \brief  Définit la classe Level
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <boost/thread/mutex.hpp>
#include <irrlicht.h>

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

        // Mutateurs
        void setEntityList(const irr::scene::quake3::tQ3EntityList& l_entity);

        // Accesseurs
        map<irr::core::stringc, irr::core::stringc>
                getEntity(irr::core::stringc name);
    protected:
    private:
        boost::mutex mutexLevel;

        const irr::scene::quake3::tQ3EntityList* l_entity;
};

#endif // LEVEL_H
