/** \file   OptionMenu.h
 *  \brief  Définit la classe OptionMenu
 */
#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "GUIPage.h"


/** \class  OptionMenu
 *  \brief  Definition du menu d'options.
 */
class OptionMenu : public GUIPage
{
    public:
        OptionMenu(Core* core);
        virtual ~OptionMenu();

        irr::gui::IGUIElement* createPage(irr::gui::IGUIEnvironment* guienv);

        // Callback
        void onEscape();
    protected:
    private:
};

#endif // OPTIONMENU_H
