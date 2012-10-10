/** \file   Player.cpp
 *  \brief  Implémente la classe Player
 */
#include "Player.h"

#include "common.h"


/**
 * Constructeur de l'objet Player
 */
Player::Player()
{
    setForwards(false);
    setBackwards(false);
    setStrafeLeft(false);
    setStrafeRight(false);

    lastPositionUpdate = -1;
    setSpeed(0.2f);
}

/**
 * Destructeur de l'objet Player
 */
Player::~Player()
{
    //dtor
}


/**
 * Permet de blesser le joueur.
 * Appellé lorsqu'il recoit des dégats
 *
 * @param degats        Degats recu
 */
void Player::takeDamage(int degats)
{
    /// \todo cacluls complexe pour gerer la vie en fonction de l'armure
}


/**
 * Met la position donnée a jour en fonction du deplacement du joueur
 */
void Player::updatePosition()
{
    int x = 0, z = 0;

    irr::u32 currentTime = getTime();
    irr::f32 elapsedTime = (irr::f32)(currentTime - lastPositionUpdate);

    if(forwards)    x += 1;
    if(backwards)   x -= 1;
    if(strafeLeft)  z += 1;
    if(strafeRight) z -= 1;

    // NB: En cas de ralentissement, le joueur peut traverser les murs
    // Cet effet est particuliérement vicieux sur la gravité, d'ou la gravité fixe

    position.X += getSpeed() * x * elapsedTime;
    position.Y -= getSpeed() * elapsedTime;
    position.Z += getSpeed() * z * elapsedTime;

    setPosition(position);
}


/**
 * Met a jour la ligne de visée du joueur
 */
void Player::updateViseurRay()
{
    irr::core::vector3df viseurPosition;
    float y = getPosition().Y;

    // Crée le plan d'intersection
    irr::core::plane3df mousePlane(
            irr::core::vector3df(1.0f, y, 1.0f),
            irr::core::vector3df(1.0f, y, 5.0f),
            irr::core::vector3df(5.0f, y, 1.0f)
    );

    // Intersection du plan et de la souris
    mousePlane.getIntersectionWithLine(
            mouseRay.start,
            mouseRay.getVector(),
            viseurPosition
    );

    // Vecteur de visée en 2D
    irr::core::line2df viseur2D;
    viseur2D.start.X = getViseurRay().start.X;
    viseur2D.start.Y = getViseurRay().start.Z;
    viseur2D.end.X = viseurPosition.X;
    viseur2D.end.Y = viseurPosition.Z;

    // Normalisé et multiplié par la portée puis additionné a la position
    // pour avoir un vecteur de visée de taille fixe
    /// \todo Portée du joueur
    viseurPosition.X = viseur2D.start.X + viseur2D.getUnitVector().X * 50; //portee joueur
    viseurPosition.Z = viseur2D.start.Y + viseur2D.getUnitVector().Y * 50; //portee joueur

    // Met le viseur du joueur a jour
    setViseurPosition(viseurPosition);
}


/**
 * Met a jour la rotation en fonction de la visée
 */
void Player::updateRotation()
{
    irr::core::vector3df rotation;
    rotation = viseurRay.getVector().getHorizontalAngle();
    setRotation(rotation);
}


// Accesseurs
/**
 * Donne la position de la camera
 *
 * @return      Position de la camera
 */
irr::core::vector3df Player::getCameraPosition()
{
    irr::core::vector3df position = getPosition();

    position.X -= 50;
    position.Y += 150;

    return position;
}


/**
 * Donne la position du joueur
 *
 * @return      Position du joueur
 */
irr::core::vector3df Player::getPosition()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return position;
}


/**
 * Donne la rotation du joueur
 *
 * @return      Rotation du joueur
 */
irr::core::vector3df Player::getRotation()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return rotation;
}


/**
 * Donne le rayon de visée du joueur
 *
 * @return          Rayon de visée
 */
irr::core::line3df Player::getViseurRay()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return viseurRay;
}


/**
 * Donne la vie du joueur
 *
 * @return      Vie du joueur
 */
int Player::getVie()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return vie;
}


/**
 * Donne l'armure du joueur
 *
 * @return      Armure du joueur
 */
int Player::getArmure()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return armure;
}


/**
 * Donne la vitesse du joueur
 *
 * @return      Vitesse du joueur
 */
float Player::getSpeed()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return speed;
}


// Mutateurs
/**
 * modifie la position du joueur et celle de la caméra
 *
 * @param position          Nouvelle position du joueur
 */
void Player::setPosition(irr::core::vector3df position)
{
    boost::mutex::scoped_lock l(mutexPlayer);

    // Modifie la position du joueur
    this->position = position;
    lastPositionUpdate = getTime();
    viseurRay.start = position;         // Viseur

    // Modifie le mouseRay (on a déplacé la caméra)
    l.unlock();
    position = getCameraPosition();
    l.lock();
    mouseRay.start = position;

    // Met la rotation du joueur a jour
    l.unlock();
    updateViseurRay();
}


/**
 * Modifie l'orientation du joueur
 *
 * @param rotation      Rotation a appliquer
 */
void Player::setRotation(irr::core::vector3df rotation)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->rotation = rotation;
}


/**
 * Met le mouseRay a jour
 *
 * @param mouseRay          Nouveau mouseRay (vecteur partant de la camera
 *                          Vers la souris)
 */
void Player::setMouseRay(irr::core::line3d<irr::f32> mouseRay)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->mouseRay = mouseRay;

    // Recalcule la position du viseur
    l.unlock();
    updateViseurRay();
}


/**
 * Met le viseur du joueur a jour
 *
 * @param viseurPosition        Nouvelle position du viseur
 */
void Player::setViseurPosition(irr::core::vector3df viseurPosition)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    viseurRay.end = viseurPosition;

    // On a modifié la position du viseur, il faut ré-orienter le joueur
    l.unlock();
    updateRotation();
}


/**
 * Definit si on avance
 *
 * @param forwards      Definit si le joueur avance
 */
void Player::setForwards(bool forwards)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->forwards = forwards;
}


/**
 * Definit si on recule
 *
 * @param backwards      Definit si le joueur recule
 */
void Player::setBackwards(bool backwards)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->backwards = backwards;
}


/**
 * Definit si on va a gauche
 *
 * @param strafeLeft      Definit si le joueur va a gauche
 */
void Player::setStrafeLeft(bool strafeLeft)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->strafeLeft = strafeLeft;
}


/**
 * Definit si on va a droite
 *
 * @param strafeRight      Definit si le joueur va a droite
 */
void Player::setStrafeRight(bool strafeRight)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->strafeRight = strafeRight;
}


/**
 * Definit la vitesse du joueur
 *
 * @param speed      Vitesse du joueur
 */
void Player::setSpeed(float speed)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->speed = speed;
}


// Callback
/**
 * Appelé si le joueur fonce dans un truc
 *
 * @param animator      Infos sur la colision
 */
bool Player::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    const irr::scene::ISceneNode* cible = animator.getCollisionNode();
    irr::core::vector3df newPosition = animator.getCollisionResultPosition();

    irr::core::line2d<irr::f32> ray;

    switch(cible->getID()) {
        case SCENE_NODE_MOBS:       // MOBS
            ray.start.X = cible->getPosition().X;
            ray.start.Y = cible->getPosition().Z;
            ray.end.X = newPosition.X;
            ray.end.Y = newPosition.Z;

            newPosition.X += ray.getUnitVector().X * 0.2f;
            newPosition.Z += ray.getUnitVector().Y * 0.2f;
            break;
        default:                    // MAP
            break;
    }

    // Met a jour la position du joueur
    setPosition(newPosition);

    return true;
}
