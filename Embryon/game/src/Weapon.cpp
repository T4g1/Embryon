/** \file   Weapon.cpp
 *  \brief  Implémente la classe Weapon
 */
 #include "Weapon.h"


 /**
 * Constructeur de l'objet Weapon
 */
 Weapon::Weapon()
 {
    //ctor
 }

 /**
 * Destructeur de l'objet Weapon
 */
Weapon::~Weapon()
{
    //dtor
}



//Accesseurs

/**
 * Donne les dégats de base de l'arme
 *
 * @return      Dégats de base
 */
float Weapon::getBaseDamage()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    //DEBUG
    baseDamage = 10;
    return baseDamage;
}

/**
 * Donne le type d'arme comme spécifié dans l'enum correspondant
 *
 * @return      Dégats de base
 */
int Weapon::getType()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return typeWeapon;
}

//Mutateurs
void Weapon::setWeapon(int idWeapon)
{
    this->weapon = idWeapon;
}

