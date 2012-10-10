/** \file   ChooseLevelMenu.cpp
 *  \brief  Implémente la classe ChooseLevelMenu
 */
#include "ChooseLevelMenu.h"

#include <map>

#include "../Core/Core.h"


/**
 * Constructeur de ChooseLevelMenu
 */
ChooseLevelMenu::ChooseLevelMenu(Core* core) :
    GUIPage(core)
{
    setId(IN_CHOOSE_LEVEL_MENU);
}


/**
 * Destructeur de ChooseLevelMenu
 */
ChooseLevelMenu::~ChooseLevelMenu()
{
    //dtor
}


/**
 * Crée la page du menu
 */
irr::gui::IGUIElement* ChooseLevelMenu::createPage(irr::gui::IGUIEnvironment* guienv)
{
    stringstream bufferLevelName;
    wchar_t levelName[50];
    int currentLevel = 1;

    map<string, int> config = core->loadConfig("GAME");

    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
            guienv->getRootGUIElement()->getAbsolutePosition()
    );

    // Affiche les boutons des niveaux
    do {
        swprintf(levelName, L"Chapitre %d", currentLevel);
        guienv->addButton(
                irr::core::rect<irr::s32>(
                        10, 50 + (currentLevel-1)*40,
                        110, 82 + (currentLevel-1)*40),
                parent, GUI_CHOOSELEVELMENU_NIVEAU1 + (currentLevel-1),
                levelName, L"Martine et le parc naturel"
        );

        // Verifie que le niveau suivant est débloqué
        bufferLevelName << "level" << currentLevel;
        if(config.find(bufferLevelName.str()) == config.end() ||
            config[bufferLevelName.str()] == -1)
        {
            break;
        }
    } while(currentLevel <= 6);

    guienv->addButton(
            irr::core::rect<irr::s32>(10, 290, 110, 322),
            parent, GUI_CHOOSELEVELMENU_RETOUR,
            L"Retour", L"Retour a l'ecran titre"
    );

    parent->setVisible(false);

    return parent;
}


// Callback
/**
 * Action lors de l'appuit sur escape
 */
void ChooseLevelMenu::onEscape()
{
    core->setMenuState(IN_MAIN_MENU);
}
