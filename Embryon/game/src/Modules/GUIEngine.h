/** \file   GUIEngine.h
 *  \brief  Définit la classe GUIEngine.
 */
#ifndef GUIENGINE_H
#define GUIENGINE_H

#include <boost/thread/mutex.hpp>

#include "Module.h"
#include "../GUI/GUIPage.h"

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
    protected:
    private:
        map<EnumGameState, GUIPage*>* l_GUIPage;

        void processMessage(module_message& msg);

        void saveOptions();
};

#endif // GUIENGINE_H
