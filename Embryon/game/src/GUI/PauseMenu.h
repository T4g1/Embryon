/** \file   PauseMenu.h
 *  \brief  Définit la classe PauseMenu
 */
#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "GUIPage.h"


/** \class  PauseMenu
 *  \brief  Definition du menu de pause.
 */
class PauseMenu : public GUIPage
{
    public:
        PauseMenu(Core* core);
        virtual ~PauseMenu();

        irr::gui::IGUIElement* createPage(irr::gui::IGUIEnvironment* guienv);

        // Callback
        void onEscape();
    protected:
    private:
};

#endif // PAUSEMENU_H
