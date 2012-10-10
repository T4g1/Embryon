/** \file   EventsEngine.cpp
 *  \brief  Implémente la classe EventsEngine
 */
#include "EventsEngine.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "../Core/Core.h"
#include "GameEngine.h"
#include "GUIEngine.h"


/**
 * Constructeur du module de gestin des evenements.
 *
 * @param core          Reference vers le Core du programme
 * @param gameEngine    Reference vers le gameEngine
 * @param guiEngine     Reference vers la guiEngine
 */
EventsEngine::EventsEngine(Core* core, GameEngine* gameEngine, GUIEngine* guiEngine) :
    Module(EVENTS, "Events", core),
    irrEventLogger("Irrlicht", true)
{
    this->gameEngine = gameEngine;
    this->guiEngine = guiEngine;

    l_keyState = new bool[255];
    for(int i=0; i<255; i++)
        l_keyState[i] = false;

    loadKeyConfig();
}

/**
 * Destructeur du module de gestion des evenements
 */
EventsEngine::~EventsEngine()
{
    //dtor
}


/**
 * Traitement du module d'evenements
 */
void EventsEngine::frame()
{
    log("Debut de la gestion des evenements");

    while(core->getIsRunning()) {
        // Attend que la file d'event ou de message ne soit plus vide
        boost::mutex::scoped_lock lockQueue(mutexQueue);
        while(event_queue.empty() && message_queue.empty())
            condQueue.wait(lockQueue);
        lockQueue.unlock();

        // Traitement des messages
        processQueue();

        // Traitement des events
        processEventQueue();
    }
}


/**
 * Charge la config du module
 */
void EventsEngine::loadKeyConfig()
{
    // Chargement de la config
    config = core->loadConfig("KEYS");

    if(config.find("forward") == config.end())     config["forward"]      = irr::KEY_KEY_Z;
    if(config.find("backward") == config.end())    config["backward"]     = irr::KEY_KEY_S;
    if(config.find("left") == config.end())   config["left"]    = irr::KEY_KEY_Q;
    if(config.find("right") == config.end())  config["right"]   = irr::KEY_KEY_D;
    if(config.find("shoot") == config.end())  config["shoot"]   = irr::KEY_KEY_E;
    if(config.find("action") == config.end())  config["action"]   = irr::KEY_KEY_A;

    core->saveConfig("KEYS", config);
}


/**
 * Gére les messages recu par le module
 *
 * @param msg           Message recu
 */
void EventsEngine::processMessage(module_message& msg)
{
    switch(msg.codeAction) {
        case ACTION_RELOAD_CONFIG_KEYS:
            loadKeyConfig();
            cout << "Reloading config keys" << endl;
            break;
        default: break;


    }
    /// \todo gérer les events, mais y'en aura surement pas ...
}


/**
 * Reception d'un evenement. Appellée par Rendering
 *
 * @param event         Event recu
 *
 * @return true         Si l'event est géré, false sinon
 */
bool EventsEngine::OnEvent(const irr::SEvent& event)
{
    switch(event.EventType) {
        case irr::EET_LOG_TEXT_EVENT:       // LOG
            irrEventLogger.log(event.LogEvent.Text);
            return true;

        default:
            boost::mutex::scoped_lock l(mutexQueue);
            event_queue.push(event);
            l.unlock();

            condQueue.notify_one();
            return false;
    }
}


/**
 * Traite la file d'events
 */
void EventsEngine::processEventQueue() {
    while(true) {
        if(event_queue.empty())
            break;

        boost::mutex::scoped_lock l(mutexQueue);
        irr::SEvent e = event_queue.front();
        event_queue.pop();
        l.unlock();

        processEvent(e);
    }
}


/**
 * Traitement d'un event
 *
 * @param event         Event a traiter
 */
void EventsEngine::processEvent(irr::SEvent& event)
{
    switch(event.EventType) {
        case irr::EET_MOUSE_INPUT_EVENT:    // SOURIS
            //processMouseEvent(event);
            break;

        case irr::EET_KEY_INPUT_EVENT:      // CLAVIER
            processKeyboardEvent(event);
            break;

        case irr::EET_GUI_EVENT:            // GUI
            processGUIEvent(event);
            break;

        default: break;
    }
}


/**
 * Traitement des events keyboard
 *
 * @param event         Event keyboard a traiter
 */
void EventsEngine::processKeyboardEvent(irr::SEvent& event)
{
    // Verifie que la touche a changé d'état depuis le dernier appel
    if(l_keyState[event.KeyInput.Key] == event.KeyInput.PressedDown)
        return;

    l_keyState[event.KeyInput.Key] = event.KeyInput.PressedDown;

    module_message msg = module_message(getId(), GUI, ACTION_CHANGE_MENU);

    switch(event.KeyInput.Key) {
    case irr::KEY_ESCAPE:
        if(!event.KeyInput.PressedDown) break;

        switch(guiEngine->getMenuState()) {
        case IN_MAIN_MENU:
            core->stop();
            return;

        case IN_CHOOSE_LEVEL_MENU:
            msg.intData["menu"] = IN_MAIN_MENU;
            break;

        case IN_OPTIONS_MENU:
            if(gameEngine->isPartieEnCours())
                msg.intData["menu"] = IN_PAUSE_MENU;
            else
                msg.intData["menu"] = IN_MAIN_MENU;
            break;

        case IN_PAUSE_MENU:
            msg.intData["menu"] = IN_GAME;
            break;

        case IN_GAME:
            msg.intData["menu"] = IN_PAUSE_MENU;
            break;
        }

        core->sendMessage(msg);
        break;

    default:
        if(gameEngine->isPartieEnCours() && !gameEngine->isPartieEnPause())
            processGameEvent(event);
        break;
    }
}


/**
 * Traitement des events en rapport avec la GUI
 *
 * @param event         Event GUI a traiter
 */
void EventsEngine::processGUIEvent(irr::SEvent& event)
{
    module_message msg = module_message(getId(), GUI, ACTION_CHANGE_MENU);

    switch(event.GUIEvent.EventType) {
    case irr::gui::EGET_BUTTON_CLICKED:
        switch(event.GUIEvent.Caller->getID()) {
        //*************************************
        // MAIN MENU
        //*************************************
        case GUI_MAINMENU_JOUER:
            msg.intData["menu"] = IN_CHOOSE_LEVEL_MENU;
            break;

        case GUI_MAINMENU_OPTIONS:
            msg.intData["menu"] = IN_OPTIONS_MENU;
            break;

        case GUI_MAINMENU_QUITTER:
            core->stop();
            return;

        //*************************************
        // MENU CHOOSE LEVEL
        //*************************************
        case GUI_CHOOSELEVELMENU_NIVEAU1:
        case GUI_CHOOSELEVELMENU_NIVEAU2:
        case GUI_CHOOSELEVELMENU_NIVEAU3:
        case GUI_CHOOSELEVELMENU_NIVEAU4:
        case GUI_CHOOSELEVELMENU_NIVEAU5:
        case GUI_CHOOSELEVELMENU_NIVEAU6:
            msg.idDestination = GAME;
            msg.codeAction = ACTION_NOUVELLE_PARTIE;
            msg.intData["niveau"] = event.GUIEvent.Caller->getID();
            msg.intData["niveau"] -= GUI_CHOOSELEVELMENU_NIVEAU1;
            break;

        case GUI_CHOOSELEVELMENU_RETOUR:
            msg.intData["menu"] = IN_MAIN_MENU;
            break;

        //*************************************
        // MENU OPTIONS
        //*************************************
        case GUI_OPTIONSMENU_KEYS_APPLIQUER:
            msg.idDestination = RENDERING;
            msg.codeAction = ACTION_SAVE_CONFIG_KEYS;
            break;
        case GUI_OPTIONSMENU_VIDEO_APPLIQUER:

            msg.idDestination = RENDERING;
            msg.codeAction = ACTION_SAVE_CONFIG_VIDEO;
            break;

        case GUI_OPTIONSMENU_RETOUR:
            if(gameEngine->isPartieEnCours())
                msg.intData["menu"] = IN_PAUSE_MENU;
            else
                msg.intData["menu"] = IN_MAIN_MENU;
            break;

        //*************************************
        // MENU PAUSE
        //*************************************
        case GUI_PAUSEMENU_CONTINUER:
            msg.intData["menu"] = IN_GAME;
            break;

        case GUI_PAUSEMENU_OPTIONS:
            msg.intData["menu"] = IN_OPTIONS_MENU;
            break;

        case GUI_PAUSEMENU_QUITTERPARTIE:
            msg.intData["menu"] = IN_MAIN_MENU;
            break;

        case GUI_PAUSEMENU_QUITTER:
            core->stop();
            return;

        default: return;
        }

        core->sendMessage(msg);
        break;

    default: break;
    }
}


/**
 * Gére les events d'une partie en cours
 *
 * @param event         Event GUI a traiter
 */
void EventsEngine::processGameEvent(irr::SEvent& event)
{
    irr::EKEY_CODE key = event.KeyInput.Key;
    bool isDown = l_keyState[key];

    module_message msg(getId(), GAME, ACTION_AUCUNE);

    if(config["forward"] == key) {
        if(isDown)  msg.codeAction = ACTION_START_WALKING_FORWARDS;
        else        msg.codeAction = ACTION_STOP_WALKING_FORWARDS;
    } else if(config["backward"] == key) {
        if(isDown)  msg.codeAction = ACTION_START_WALKING_BACKWARDS;
        else        msg.codeAction = ACTION_STOP_WALKING_BACKWARDS;
    } else if(config["left"] == key) {
        if(isDown)  msg.codeAction = ACTION_START_STRAFE_LEFT;
        else        msg.codeAction = ACTION_STOP_STRAFE_LEFT;
    } else if(config["right"] == key) {
        if(isDown)  msg.codeAction = ACTION_START_STRAFE_RIGHT;
        else        msg.codeAction = ACTION_STOP_STRAFE_RIGHT;
    } else
        return;

    core->sendMessage(msg);
}
