/** \file   EventsEngine.cpp
 *  \brief  Implémente la classe EventsEngine
 */
#include "EventsEngine.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "../Core/Core.h"


/**
 * Constructeur du module de gestin des evenements.
 *
 * @param core          Reference vers le Core du programme
 */
EventsEngine::EventsEngine(Core* core) :
    Module(EVENTS, "Events", core),
    irrEventLogger("Irrlicht", true)
{
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

    log("Fin de la gestion des evenements");
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
            break;
        default: break;
    }
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

    module_message msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);

    switch(event.KeyInput.Key) {
    case irr::KEY_ESCAPE:
        /// \todo ne pas gérer si GUIKeySelector a le focus
        if(!event.KeyInput.PressedDown) break;

        core->sendMessage(msg);
        break;

    default:
        if(core->isPartieEnCours() && !core->isPartieEnPause())
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
    module_message msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);

    switch(event.GUIEvent.EventType) {
    case irr::gui::EGET_BUTTON_CLICKED:
        switch(event.GUIEvent.Caller->getID()) {
        //*************************************
        // MAIN MENU
        //*************************************
        case GUI_MAINMENU_JOUER:
            core->setMenuState(IN_CHOOSE_LEVEL_MENU);
            break;

        case GUI_MAINMENU_OPTIONS:
            core->setMenuState(IN_OPTIONS_MENU);
            break;

        case GUI_MAINMENU_QUITTER:
            msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);
            core->sendMessage(msg);
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
            core->setMenuState(IN_GAME);
            msg = module_message(getId(), GAME, ACTION_NOUVELLE_PARTIE);
            msg.intData["niveau"] = event.GUIEvent.Caller->getID();
            msg.intData["niveau"] -= GUI_CHOOSELEVELMENU_NIVEAU1;
            core->sendMessage(msg);
            break;

        case GUI_CHOOSELEVELMENU_RETOUR:
            msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);
            core->sendMessage(msg);
            break;

        //*************************************
        // MENU OPTIONS
        //*************************************
        case GUI_OPTIONSMENU_KEYS_APPLIQUER:
        case GUI_OPTIONSMENU_VIDEO_APPLIQUER:
            msg = module_message(getId(), RENDERING, ACTION_SAVE_CONFIG);
            core->sendMessage(msg);
            break;

        case GUI_OPTIONSMENU_RETOUR:
            msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);
            core->sendMessage(msg);
            break;

        //*************************************
        // MENU PAUSE
        //*************************************
        case GUI_PAUSEMENU_CONTINUER:
            msg = module_message(getId(), GUI, ACTION_MENU_ON_ESCAPE);
            core->sendMessage(msg);
            break;

        case GUI_PAUSEMENU_OPTIONS:
            core->setMenuState(IN_OPTIONS_MENU);
            break;

        case GUI_PAUSEMENU_QUITTERPARTIE:
            core->setMenuState(IN_MAIN_MENU);
            msg = module_message(getId(), GAME, ACTION_QUITTER_PARTIE);
            core->sendMessage(msg);
            break;

        case GUI_PAUSEMENU_QUITTER:
            core->stop();
            return;

        default: return;
        }
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
    } else if(config["action"] == key) {
        msg.codeAction = ACTION_PLAYER_ACTION;
    } else
        return;

    core->sendMessage(msg);
}
