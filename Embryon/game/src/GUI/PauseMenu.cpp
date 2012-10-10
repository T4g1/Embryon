/** \file   PauseMenu.cpp
 *  \brief  Implémente la classe PauseMenu
 */
#include "PauseMenu.h"


/**
 * Constructeur de PauseMenu
 */
PauseMenu::PauseMenu(Core* core) :
    GUIPage(core)
{
    setId(IN_PAUSE_MENU);
}


/**
 * Destructeur de PauseMenu
 */
PauseMenu::~PauseMenu()
{
    //dtor
}


/**
 * Crée la page du menu
 */
irr::gui::IGUIElement* PauseMenu::createPage(irr::gui::IGUIEnvironment* guienv)
{
    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
        irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
        guienv->getRootGUIElement()->getAbsolutePosition()
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 50, 110, 82),
            parent, GUI_PAUSEMENU_CONTINUER,
            L"Continuer", L"Reprends la partie"
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 90, 110, 122),
            parent, GUI_PAUSEMENU_OPTIONS,
            L"Options", L"Modifier les options"
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 130, 110, 162),
            parent, GUI_PAUSEMENU_QUITTERPARTIE,
            L"Quitter la partie", L"Retourne a l'ecran titre"
    );

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 170, 110, 202),
            parent, GUI_PAUSEMENU_QUITTER,
            L"Quitter", L"Termine l'application"
    );

    parent->setVisible(false);

    return parent;
}


// Callback
/**
 * Action lors de l'appuit sur escape
 */
void PauseMenu::onEscape()
{
    core->setMenuState(IN_GAME);
    core->setPartieEnPause(false);
}
