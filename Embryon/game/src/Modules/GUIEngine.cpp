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
    l_GUIPage = core->getGUIPages();
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

    log("Fin de la gestion de la GUI");
}


/**
 * Traitement d'un message recu par le module
 *
 * @param msg           Message recu recu
 */
void GUIEngine::processMessage(module_message& msg)
{
    switch(msg.codeAction) {
    case ACTION_MENU_ON_ESCAPE:
        (*l_GUIPage)[core->getMenuState()]->onEscape();
        break;

    default: break;
    }
}
