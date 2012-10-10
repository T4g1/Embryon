/** \file   IGUIKeySelector.cpp
 *  \brief  Implémente IGUIKeySelector
 */
#include "IGUIKeySelector.h"

#include <iostream>

using namespace std;


/**
 * Tableau associant Key et leurs noms
 */
string IGUIKeySelector::KeyNames[] = {
    "-",
    "Left Button",
    "Right Button",
    "Cancel",
    "Middle Button",
    "X Button 1",
    "X Button 2",
    "-",
    "Back",
    "Tab",
    "-",
    "-",
    "Clear",
    "Return",
    "-",
    "-",
    "Shift",
    "Control",
    "Menu",
    "Pause",
    "Capital",
    "Kana",
    "-",
    "Junja",
    "Final",
    "Kanji",
    "-",
    "Escape",
    "Convert",
    "Nonconvert",
    "Accept",
    "Mode Change",
    "Space",
    "Priot",
    "Next",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Select",
    "Print",
    "Execute",
    "Snapshot",
    "Insert",
    "Delete",
    "Help",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "Left Windows",
    "Right Windows",
    "Apps",
    "-",
    "Sleep",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Numpad *",
    "Numpad +",
    "Numpad /",
    "Numpad -",
    "Numpad .",
    "Numpad /",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Num Lock",
    "Scroll Lock",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Left Shift",
    "Right Shight",
    "Left Control",
    "Right Control",
    "Left Menu",
    "Right Menu",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Plus",
    "Comma",
    "Minus",
    "Period",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Attn",
    "CrSel",
    "ExSel",
    "Erase OEF",
    "Play",
    "Zoom",
    "PA1",
    "OEM Clear",
    "-"
};

/**
 * Constructeur de IGUIKeySelector
 */
IGUIKeySelector::IGUIKeySelector(irr::gui::IGUIEnvironment* guiEnv,
        irr::gui::IGUIElement* parent, irr::s32 id,
        irr::core::rect<irr::s32> rect) :
    irr::gui::IGUIElement(irr::gui::EGUIET_BUTTON, guiEnv, parent, id, rect)
{
    setKey(irr::KEY_KEY_CODES_COUNT);

    this->setEnabled(true);
    this->setVisible(true);
}

/**
 * Destructeur de IGUIKeySelector
 */
IGUIKeySelector::~IGUIKeySelector()
{
    //dtor
}


/**
 * Affiche l'element
 */
void IGUIKeySelector::draw()
{
    if(!IsVisible)
        return;

    irr::gui::IGUISkin* skin = Environment->getSkin();
    //irr::video::IVideoDriver* driver = Environment->getVideoDriver();

    //driver->draw2DRectangle(skin->getColor(irr::gui::EGDC_TOOLTIP_BACKGROUND), AbsoluteRect, &AbsoluteClippingRect);

    // Affiche le texte
    if (Text.size()) {
        irr::gui::IGUIFont* font = skin->getFont(irr::gui::EGDF_BUTTON);

        irr::core::rect<irr::s32> rect = AbsoluteRect;

        font->draw(Text.c_str(), rect,
            skin->getColor(irr::gui::EGDC_BUTTON_TEXT),
            true, true, &AbsoluteClippingRect
        );
    }

    irr::gui::IGUIElement::draw();
}


/**
 * Appellée lorsqu'on recoit un evenement
 */
bool IGUIKeySelector::OnEvent(const irr::SEvent& event)
{
    switch(event.EventType) {
    case irr::EET_MOUSE_INPUT_EVENT:
        if(event.MouseInput.isLeftPressed())
            Environment->setFocus(this);
        break;
    case irr::EET_KEY_INPUT_EVENT:
        if(!event.KeyInput.PressedDown) break;

        if(event.KeyInput.Key == irr::KEY_ESCAPE) {
            closeInput();
            return true;
        }

        Environment->removeFocus(this);
        setKey(event.KeyInput.Key);
        break;
    case irr::EET_GUI_EVENT:
        switch(event.GUIEvent.EventType) {
        case irr::gui::EGET_ELEMENT_FOCUSED:
            Text = "?";
            break;
        case irr::gui::EGET_ELEMENT_FOCUS_LOST:
            setKey(key);
            break;
        default: break;
        }
    default: break;
    }

    return false;
}


// Accesseurs
/**
 * Récupére la touche séléctionnée
 *
 * @return          Key séléctionnée
 */
irr::EKEY_CODE IGUIKeySelector::getValue()
{
    return key;
}


// Mutateurs
/**
 * Modifie la touche séléctionnée
 *
 * @param key       Nouvelle touche séléctionnée
 */
void IGUIKeySelector::setKey(irr::EKEY_CODE key)
{
    this->key = key;

    if(key == irr::KEY_KEY_CODES_COUNT)
        Text = " ";
    else
        Text = KeyNames[key].c_str();
}


/**
 * Annule la saisie
 */
void IGUIKeySelector::closeInput()
{
    Environment->removeFocus(this);
    setKey(key);
}
