/** \file   Weapon.h
 *  \brief  Définit la classe Weapon
 */
#ifndef WEAPON_H
#define WEAPON_H

#include <boost/thread/mutex.hpp>

using namespace std;

/** \class  Weapon
 *  \brief  Gére toutes les informations relatives aux armes.
 *
 * Contient les noms ainsi que les caractéristiques des armes
 * un ensemble de méthodes permettant d'implément différents types d'armes,...
 *
 */

 class Weapon
 {
     public:
        Weapon();
        virtual ~Weapon();

        // Accesseurs
        float getBaseDamage();
        int getType();

        //Mutateurs
        void setWeapon(int);
     protected:
     private:
        boost::mutex mutexPlayer;
        float baseDamage;
        int typeWeapon;
        int weapon;
 };

#endif // WEAPON_H
