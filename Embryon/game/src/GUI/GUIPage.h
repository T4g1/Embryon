/** \file   GUIPage.h
 *  \brief  Définit la classe GUIPage
 */
#ifndef GUIPAGE_H
#define GUIPAGE_H

#include <irrlicht.h>

#include "../common.h"
#include "../Core/Core.h"

using namespace std;


/** \class  GUIPage
 *  \brief  Classe permettant de définir une page de GUI.
 *
 * Une "page de GUI" est un etat du programme plus communement appellé menu.
 */
class GUIPage
{
    public:
        GUIPage(Core* core);
        virtual ~GUIPage();

        virtual irr::gui::IGUIElement* createPage(
                irr::gui::IGUIEnvironment* guienv);

        // Accesseurs
        EnumGameState getId();

        // Mutateurs
        void setId(EnumGameState id);

        // Callback
        virtual void onEscape();
    private:
    protected:
        Core* core;
        EnumGameState id;
};

#endif // GUIPAGE_H
