/** \file   GUIPage.cpp
 *  \brief  Implémente la classe GUIPage
 */
#include "GUIPage.h"


/**
 * Constructeur de GUIPage
 */
GUIPage::GUIPage(Core* core)
{
    this->core = core;

    setId(IN_MAIN_MENU);
}


/**
 * Destructeur de GUIPage
 */
GUIPage::~GUIPage()
{
    //dtor
}


/**
 * Crée le menu
 */
irr::gui::IGUIElement* GUIPage::createPage(
        irr::gui::IGUIEnvironment* guienv)
{
    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
            guienv->getRootGUIElement()->getAbsolutePosition()
    );

    parent->setVisible(false);

    return parent;
}


// Accesseurs
/**
 * Donne l'id de la page
 *
 * @return          Retourne l'id de la page
 */
EnumGameState GUIPage::getId()
{
    return id;
}


// Mutateurs
/**
 * Modifie l'id de la page
 *
 * @param id        Nouvel id de page
 */
void GUIPage::setId(EnumGameState id)
{
    this->id = id;
}


// Callback
/**
 * Lorsqu'on appuye sur escape
 */
void GUIPage::onEscape()
{
}
