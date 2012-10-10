/** \file   GameMenu.h
 *  \brief  Définit la classe GameMenu
 */
#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "GUIPage.h"


/** \class  GameMenu
 *  \brief  Definition du menu principal.
 */
class GameMenu : public GUIPage
{
    public:
        GameMenu(Core* core);
        virtual ~GameMenu();

        irr::gui::IGUIElement* createPage(irr::gui::IGUIEnvironment* guienv);

        // Callback
        void onEscape();
    protected:
    private:
};

#endif // GAMEMENU_H
