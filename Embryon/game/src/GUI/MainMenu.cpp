/** \file   MainMenu.cpp
 *  \brief  Implémente la classe MainMenu
 */
#include "MainMenu.h"


/**
 * Constructeur de MainMenu
 */
MainMenu::MainMenu(Core* core) :
    GUIPage(core)
{
    setId(IN_MAIN_MENU);
}


/**
 * Destructeur de MainMenu
 */
MainMenu::~MainMenu()
{
    //dtor
}


/**
 * Crée la page du menu
 */
irr::gui::IGUIElement* MainMenu::createPage(irr::gui::IGUIEnvironment* guienv)
{
    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
            guienv->getRootGUIElement()->getAbsolutePosition()
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 50, 110, 82),
            parent, GUI_MAINMENU_JOUER,
            L"Jouer", L"Lance une nouvelle partie"
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 90, 110, 122),
            parent, GUI_MAINMENU_OPTIONS,
            L"Options", L"Modifie les options de jeu"
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 130, 110, 162),
            parent, GUI_MAINMENU_QUITTER,
            L"Quitter", L"Termine l'application"
    );

    parent->setVisible(false);

    return parent;
}


// Callback
/**
 * Action lors de l'appuit sur escape
 */
void MainMenu::onEscape()
{
    core->stop();
}
