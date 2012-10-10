/** \file   GUIEngine.cpp
 *  \brief  Implémente la classe GUIEngine
 */
#include "GUIEngine.h"

#include "../Core/Core.h"


/**
 * Constructeur du module de GUI
 *
 * @param core          Reference vers le Core du programme
 */
GUIEngine::GUIEngine(Core* core) :
    Module(GUI, "Gui", core)
{
    menuState = IN_MAIN_MENU;
}

/**
 * Destructeur du module de GUI
 */
GUIEngine::~GUIEngine()
{
    //dtor
}


/**
 * Traitement du module
 */
void GUIEngine::frame()
{
    log("Debut de la gestion de la GUI");

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
void GUIEngine::processMessage(module_message& msg)
{
    int newMenuState;

    switch(msg.codeAction) {
    case ACTION_CHANGE_MENU:                // On a changé de menu
        newMenuState = msg.intData["menu"];

        // Sortie de pause
        if(menuState == IN_PAUSE_MENU &&
           msg.intData["menu"] == IN_GAME)
        {
            msg = module_message(getId(), GAME, ACTION_UNPAUSE);
            core->sendMessage(msg);
        }
        // Mise en pause
        else if(menuState == IN_GAME &&
                msg.intData["menu"] == IN_PAUSE_MENU)
        {
            msg = module_message(getId(), GAME, ACTION_PAUSE);
            core->sendMessage(msg);
        }
        // Quitter la partie
        else if(menuState == IN_PAUSE_MENU &&
                msg.intData["menu"] == IN_MAIN_MENU)
        {
            msg = module_message(getId(), GAME, ACTION_QUITTER_PARTIE);
            core->sendMessage(msg);
        }

        menuState = (EnumGameState)newMenuState;

        msg = module_message(getId(), RENDERING, ACTION_AFFICHER_MENU);
        msg.intData["menu"] = newMenuState;
        core->sendMessage(msg);
        break;

    case ACTION_SAVE_CONFIG_VIDEO:
        log("Sauvegarde la configuration");
        break;

    default: break;
    }
}


// Accesseurs
/**
 * Donne le menu affiché actuellement
 *
 * @return      Le numero du menu affiché
 */
EnumGameState GUIEngine::getMenuState()
{
    boost::mutex::scoped_lock l(mutexMenuState);
    return menuState;
}


// Mutateur
/**
 * Modifie le menu affiché actuellement
 */
void GUIEngine::setMenuState(EnumGameState menuState)
{
    boost::mutex::scoped_lock l(mutexMenuState);
    this->menuState = menuState;
}
