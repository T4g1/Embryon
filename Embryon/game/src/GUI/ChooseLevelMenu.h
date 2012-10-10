/** \file   ChooseLevelMenu.h
 *  \brief  Définit la classe ChooseLevelMenu.
 */
#ifndef CHOOSELEVELMENU_H
#define CHOOSELEVELMENU_H

#include "GUIPage.h"


/** \class  ChooseLevelMenu
 *  \brief  Definition du menu de choix de niveau.
 */
class ChooseLevelMenu : public GUIPage
{
    public:
        ChooseLevelMenu(Core* core);
        virtual ~ChooseLevelMenu();

        irr::gui::IGUIElement* createPage(irr::gui::IGUIEnvironment* guienv);

        // Callback
        void onEscape();
    protected:
    private:
};

#endif // CHOOSELEVELMENU_H
