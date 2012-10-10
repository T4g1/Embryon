/** \file   Player.h
 *  \brief  Définit la classe Player
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <boost/thread/mutex.hpp>
#include <list>
#include <irrlicht.h>

using namespace std;


/** \class  Player
 *  \brief  Gére toutes les informations relatives au joueur.
 *
 * Contient la position du joueur, sa vie, son armure, sa position. Fournit
 * un ensemble de méthodes permettant d'agir sur le joueur (dégats, mouvements,
 * ...).
 */
class Player : public irr::scene::ICollisionCallback
{
    public:
        Player();
        virtual ~Player();

        void takeDamage(int degats);
        void updatePosition();
        void updateViseurRay();
        void updateRotation();

        // Accesseurs
        irr::scene::ISceneNode* getNode();
        irr::core::vector3df getPosition();
        irr::core::line3df getViseurRay();
        int getVie();
        int getArmure();

        // Mutateurs
        void setNode(irr::scene::ISceneNode* node);
        void setCamera(irr::scene::ICameraSceneNode* camera);
        void setPosition(irr::core::vector3df position);
        void setRotation(irr::core::vector3df rotation);
        void setMouseRay(irr::core::line3d<irr::f32> mouseRay);
        void setViseurPosition(irr::core::vector3df viseurPosition);
        void setForwards(bool forwards);
        void setBackwards(bool backwards);
        void setStrafeLeft(bool strafeLeft);
        void setStrafeRight(bool strafeRight);

        // Callback de collisions joueur
        bool onCollision (
                const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
    protected:
    private:
        boost::mutex mutexPlayer;

        irr::core::vector3df position;
        irr::scene::ICameraSceneNode* camera;
        irr::scene::ISceneNode* node;

        irr::core::line3d<irr::f32> mouseRay;
        irr::core::line3d<irr::f32> viseurRay;

        int vie;
        int armure;

        bool forwards;
        bool backwards;
        bool strafeLeft;
        bool strafeRight;

        float speed;

        //list<weapon> l_weapon;
};

#endif // PLAYER_H
