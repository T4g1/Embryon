/** \file   GUIEngine.h
 *  \brief  Définit la classe GUIEngine.
 */
#ifndef GUIENGINE_H
#define GUIENGINE_H

#include <boost/thread/mutex.hpp>

#include "Module.h"

using namespace std;

class Core;


/** \class  GUIEngine
 *  \brief  Gére la GUI dans son ensemble.
 *
 * Permet de savoir dans quel menu le programme se trouve et définit
 * différentes méthodes permettant à la GUI d'effectuer des actions
 * complexes.
 */
class GUIEngine : public Module
{
    public:
        GUIEngine(Core* core);
        virtual ~GUIEngine();

        void frame();

        EnumGameState getMenuState();
    protected:
    private:
        boost::mutex mutexMenuState;
        EnumGameState menuState;      // Menu ou l'on se trouve

        void processMessage(module_message& msg);
        void setMenuState(EnumGameState menuState);

        void saveOptions();
};

#endif // GUIENGINE_H
