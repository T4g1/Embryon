/** \file   common.h
 *  \brief  Définit plusieurs fonctons et énumérations
 *          utile dans tout le programme.
 */
#ifndef COMMON_H
#define COMMON_H

#define FPS             50

#define GRAVITY         -2

#include <boost/multi_array.hpp>
#include <string>
#include <irrlicht.h>

using namespace std;


/** \enum   EnumModuleId
 *  \brief  Contient les identifiants des différents modules.
 */
enum EnumModuleId {
    CORE, RENDERING, EVENTS, GAME, GUI
};

/** \enum   EnumDirection
 *  \brief  Définit les direction possible pour se déplacer
 */
enum EnumDirection {
    AVANCER,
    RECULER,
    GAUCHE,
    DROITE
};

/** \enum   EnumEntityActivation
 *  \brief  Définit tous les types d'activation d'entité possible
 */
enum EnumEntityActivation {
    ENTITY_ACTIVATION_COLLIDE,          // Déclenché par collision
    ENTITY_ACTIVATION_ACTIONNED,        // Déclenché par action du joueur
    ENTITY_ACTIVATION_TRIGGERED         // Déclenché par une autre entité
};



string wchar_to_string(const wchar_t*);
string string_to_wchar(const string*);

irr::u32 getTime();

#endif // COMMON_H
