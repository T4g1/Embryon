/** \file   GameEngine.cpp
 *  \brief  Implémente la classe GameEngine
 */
#include "GameEngine.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "../Core/Core.h"


/**
 * Constructeur du module de gestion du jeu
 *
 * @param core          Reference vers le Core du programme
 */
GameEngine::GameEngine(Core* core) :
    Module(GAME, "Game", core),
    player()
{
    partieEnCours = false;
}

/**
 * Destructeur du module de gestion du jeu
 */
GameEngine::~GameEngine()
{
    //dtor
}


/**
 * Traitement du module
 */
void GameEngine::frame()
{
    log("Debut de la gestion du jeu");

    while(core->getIsRunning()) {
        // Attend que la file de message ne soit plus vide
        boost::mutex::scoped_lock lockQueue(mutexQueue);
        while(message_queue.empty())
            condQueue.wait(lockQueue);
        lockQueue.unlock();

        // Traitement des messages
        processQueue();
    }
}


/**
 * Traitement d'un message recu par le module
 *
 * @param msg           Message recu recu
 */
void GameEngine::processMessage(module_message& msg)
{
    switch(msg.codeAction) {
    case ACTION_NOUVELLE_PARTIE:                // Demarre une partie
        newGame(msg.intData["niveau"]);
        break;

    case ACTION_PAUSE:                          // Mise en pause
        setPartieEnPause(true);
        break;

    case ACTION_UNPAUSE:                        // Sortie de pause
        setPartieEnPause(false);
        break;

    case ACTION_QUITTER_PARTIE:                 // Fin de partie
        setPartieEnCours(false);
        break;

    //***************************************
    //  DEPLACEMENTS DU JOUEUR
    //***************************************
    case ACTION_START_WALKING_FORWARDS:
        getPlayer()->setForwards(true);
        break;

    case ACTION_START_WALKING_BACKWARDS:
        getPlayer()->setBackwards(true);
        break;

    case ACTION_START_STRAFE_LEFT:
        getPlayer()->setStrafeLeft(true);
        break;

    case ACTION_START_STRAFE_RIGHT:
        getPlayer()->setStrafeRight(true);
        break;

    case ACTION_STOP_WALKING_FORWARDS:
        getPlayer()->setForwards(false);
        break;

    case ACTION_STOP_WALKING_BACKWARDS:
        getPlayer()->setBackwards(false);
        break;

    case ACTION_STOP_STRAFE_LEFT:
        getPlayer()->setStrafeLeft(false);
        break;

    case ACTION_STOP_STRAFE_RIGHT:
        getPlayer()->setStrafeRight(false);
        break;

    default: break;
    }
}


/**
 * Initialise une nouvelle partie
 */
void GameEngine::newGame(int niveau)
{
    // Nom du niveau
    char buffer[10];
    itoa(niveau + 1, buffer, 10);
    string name = "../../media/maps/niveau" + string(buffer) + ".pk3";

    // Chargement du niveau
    module_message msg(getId(), RENDERING, ACTION_INIT_GAME);
    msg.strData["niveau"] = name;
    core->sendMessage(msg);

    // Affichage de la GUI
    msg = module_message(getId(), GUI, ACTION_CHANGE_MENU);
    msg.intData["menu"] = IN_GAME;
    core->sendMessage(msg);

    setPartieEnCours(true);
    setPartieEnPause(false);
}


// Accesseur
/**
 * Indique si une partie est en cours
 *
 * @return      true si une partie a lieu
 */
bool GameEngine::isPartieEnCours()
{
    boost::mutex::scoped_lock l(mutexPartieEnCours);
    return partieEnCours;
}


/**
 * Indique si une partie est en pause
 *
 * @return      true si une partie est en pause
 */
bool GameEngine::isPartieEnPause()
{
    boost::mutex::scoped_lock l(mutexPartieEnPause);
    return partieEnPause;
}


/**
 * Donne acces au niveau
 *
 * @return      Pointeur sur le niveau
 */
Level* GameEngine::getLevel()
{
    return &level;
}


/**
 * Donne acces au niveau
 *
 * @return      Pointeur sur le niveau
 */
Player* GameEngine::getPlayer()
{
    return &player;
}


// Mutateur
/**
 * Definit si une partie a lieu ou non
 * Garantit que partieEnPause vaut false si on partieEnCours vaut false
 */
void GameEngine::setPartieEnCours(bool partieEnCours)
{
    // Si on quitte la partie
    if(partieEnCours)
        setPartieEnPause(false);

    boost::mutex::scoped_lock l(mutexPartieEnCours);
    this->partieEnCours = partieEnCours;
}


/**
 * Definit si une partie est en pause
 * Garantit que partieEnPause vaut false si on partieEnCours vaut false
 */
void GameEngine::setPartieEnPause(bool partieEnPause)
{
    boost::mutex::scoped_lock l(mutexPartieEnPause);
    // Si aucune partie en cours, ne peut valoir que false
    if(!isPartieEnCours())
        this->partieEnPause = false;
    else {
        this->partieEnPause = partieEnPause;
        l.unlock();

        // Previent le RenderingEngine
        module_message msg(getId(), RENDERING, ACTION_UNPAUSE);
        if(partieEnPause)
            msg.codeAction = ACTION_PAUSE;

        core->sendMessage(msg);
    }
}
