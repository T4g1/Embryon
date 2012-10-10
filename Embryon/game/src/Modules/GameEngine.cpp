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
    Module(GAME, "Game", core)
{
    //ctor
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
    loadGameConfig();

    while(core->getIsRunning()) {
        // Attend que la file de message ne soit plus vide
        boost::mutex::scoped_lock lockQueue(mutexQueue);
        while(message_queue.empty())
            condQueue.wait(lockQueue);
        lockQueue.unlock();

        // Traitement des messages
        processQueue();
    }

    log("Fin de la gestion du jeu");
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

    case ACTION_QUITTER_PARTIE:                 // Fin de partie
        core->setPartieEnCours(false);
        break;

    //***************************************
    //  DEPLACEMENTS DU JOUEUR
    //***************************************
    case ACTION_START_WALKING_FORWARDS:
        core->getPlayer()->setForwards(true);
        break;

    case ACTION_START_WALKING_BACKWARDS:
        core->getPlayer()->setBackwards(true);
        break;

    case ACTION_START_STRAFE_LEFT:
        core->getPlayer()->setStrafeLeft(true);
        break;

    case ACTION_START_STRAFE_RIGHT:
        core->getPlayer()->setStrafeRight(true);
        break;

    case ACTION_STOP_WALKING_FORWARDS:
        core->getPlayer()->setForwards(false);
        break;

    case ACTION_STOP_WALKING_BACKWARDS:
        core->getPlayer()->setBackwards(false);
        break;

    case ACTION_STOP_STRAFE_LEFT:
        core->getPlayer()->setStrafeLeft(false);
        break;

    case ACTION_STOP_STRAFE_RIGHT:
        core->getPlayer()->setStrafeRight(false);
        break;

    case ACTION_PLAYER_ACTION:
        msg.idDestination = RENDERING;
        core->sendMessage(msg);
        break;

    default: break;
    }
}


/**
 * Initialise une nouvelle partie
 *
 * @param niveau        Numero du niveau
 */
void GameEngine::newGame(int niveau)
{
    if(niveau < 0 || niveau > 5) {
        log("Niveau spécifié inexistant");
        return;
    }

    // Nom du niveau
    ostringstream name;
    name << "../../media/maps/niveau" << (niveau+1) << ".pk3";

    // Chargement du niveau
    module_message msg(getId(), RENDERING, ACTION_INIT_GAME);
    msg.strData["niveau"] = name.str();
    core->sendMessage(msg);

    // Affichage de la GUI
    msg = module_message(getId(), GUI, ACTION_CHANGE_MENU);
    msg.intData["menu"] = IN_GAME;
    core->sendMessage(msg);

    core->setPartieEnCours(true);
    core->setPartieEnPause(false);
}


/**
 * Charge la config du module
 */
void GameEngine::loadGameConfig()
{
    // Chargement de la config
    config = core->loadConfig("GAME");

    if(config.find("level1") == config.end())       config["level1"] = -1;
    if(config.find("level2") == config.end())       config["level2"] = -1;
    if(config.find("level3") == config.end())       config["level3"] = -1;
    if(config.find("level4") == config.end())       config["level4"] = -1;
    if(config.find("level5") == config.end())       config["level5"] = -1;
    if(config.find("level6") == config.end())       config["level6"] = -1;

    core->saveConfig("GAME", config);
}
