/** \file   GameMenu.cpp
 *  \brief  Implémente la classe GameMenu
 */
#include "GameMenu.h"


/**
 * Constructeur de GameMenu
 */
GameMenu::GameMenu(Core* core) :
    GUIPage(core)
{
    setId(IN_GAME);
}


/**
 * Destructeur de GameMenu
 */
GameMenu::~GameMenu()
{
    //dtor
}


/**
 * Crée la page du menu
 */
irr::gui::IGUIElement* GameMenu::createPage(irr::gui::IGUIEnvironment* guienv)
{
    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
            guienv->getRootGUIElement()->getAbsolutePosition()
    );

    parent->setVisible(false);

    return parent;
}


// Callback
/**
 * Action lors de l'appuit sur escape
 */
void GameMenu::onEscape()
{
    core->setMenuState(IN_PAUSE_MENU);
    core->setPartieEnPause(true);
}
