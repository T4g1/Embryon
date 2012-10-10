/** \file   EventsEngine.h
 *  \brief  Définit la classe EventsEngine
 */
#ifndef EVENTSENGINE_H
#define EVENTSENGINE_H

#include <irrlicht.h>
#include <boost/thread/mutex.hpp>

#include "Module.h"

using namespace std;

class Core;
class GameEngine;
class GUIEngine;


/** \class  EventsEngine
 *  \brief  Gére les événements clavier, souris, log
 *          (Irrlicht) et GUI (Irrlicht).
 *
 * Les événements GUI sont relayés vers GUIEngine s'ils sont trop complexe
 * (sauvegarde d'options, ...).
 */
class EventsEngine :
    public Module,
    public irr::IEventReceiver
{
    public:
        EventsEngine(Core* core, GameEngine* gameEngine, GUIEngine* guiEngine);
        virtual ~EventsEngine();

        virtual bool OnEvent(const irr::SEvent& event);

        void frame();

        void loadKeyConfig();
    protected:
    private:
        GameEngine* gameEngine;
        GUIEngine* guiEngine;

        bool *l_keyState;       // Etat des touches
        map<EnumCodePlayerAction, irr::EKEY_CODE> l_keyAffectation;

        Logger irrEventLogger;

        queue<irr::SEvent> event_queue;

        void processEventQueue();
        void processEvent(irr::SEvent& event);
        void processMouseEvent(irr::SEvent& event);
        void processKeyboardEvent(irr::SEvent& event);
        void processGUIEvent(irr::SEvent& event);

        void processGameEvent(irr::SEvent& event);

        void processMessage(module_message& msg);
};

#endif // EVENTSENGINE_H
