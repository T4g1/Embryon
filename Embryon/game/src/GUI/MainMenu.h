/** \file   MainMenu.h
 *  \brief  Définit la classe MainMenu
 */
#ifndef MAINMENU_H
#define MAINMENU_H

#include "GUIPage.h"


/** \class  MainMenu
 *  \brief  Definition du menu principal.
 */
class MainMenu : public GUIPage
{
    public:
        MainMenu(Core* core);
        virtual ~MainMenu();

        irr::gui::IGUIElement* createPage(irr::gui::IGUIEnvironment* guienv);

        // Callback
        void onEscape();
    protected:
    private:
};

#endif // MAINMENU_H
