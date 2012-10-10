/** \file   Player.cpp
 *  \brief  Implémente la classe Player
 */
#include "Player.h"


/**
 * Constructeur de l'objet Player
 */
Player::Player()
{
    setForwards(false);
    setBackwards(false);
    setStrafeLeft(false);
    setStrafeRight(false);

    speed = 3.0f;
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

    if(forwards)    x += 1;
    if(backwards)   x -= 1;
    if(strafeLeft)  z += 1;
    if(strafeRight) z -= 1;

    position.X += speed * x;
    position.Y -= speed * 5;
    position.Z += speed * z;

    setPosition(position);
}


/**
 * Met a jour la ligne de visée du joueur
 */
void Player::updateViseurRay()
{
    irr::core::vector3df viseurPosition;
    float y = node->getPosition().Y;

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
 * Donne le node du joueur
 *
 * @return      Node du joueur
 */
irr::scene::ISceneNode* Player::getNode()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return node;
}


/**
 * Donne la position du joueur
 *
 * @return      Position du joueur
 */
irr::core::vector3df Player::getPosition()
{
    boost::mutex::scoped_lock l(mutexPlayer);
    return node->getPosition();
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


// Mutateurs
/**
 * Donne le node du joueur. Ne doit être appellée que par RrenderingEngine.
 *
 * @param node          SceneNode du joueur
 */
void Player::setNode(irr::scene::ISceneNode* node)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->node = node;
}


/**
 * Donne la caméra qui regarde le joueur
 *
 * @param camera        Camera qui regarde le joueur
 */
void Player::setCamera(irr::scene::ICameraSceneNode* camera)
{
    boost::mutex::scoped_lock l(mutexPlayer);
    this->camera = camera;
}


/**
 * modifie la position du joueur et celle de la caméra
 *
 * @param position          Nouvelle position du joueur
 */
void Player::setPosition(irr::core::vector3df position)
{
    boost::mutex::scoped_lock l(mutexPlayer);

    // Calcule la position de la camera
    irr::core::vector3df cameraPosition = position;
    cameraPosition.X -= 50;
    cameraPosition.Y += 100;

    // Modifie la position du joueur
    this->position = position;
    node->setPosition(position);        // Node
    viseurRay.start = position;         // Viseur

    // Modifie la position de la caméra
    camera->setTarget(position);
    camera->setPosition(cameraPosition);

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
    node->setRotation(rotation);
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


// Callback
/**
 * Appelé si le joueur fonce dans un truc
 *
 * @param animator      Infos sur la colision
 */
bool Player::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    setPosition(animator.getCollisionResultPosition());

    return false;
}
