/** \file   OptionMenu.cpp
 *  \brief  Implémente la classe OptionMenu
 */
#include "OptionMenu.h"

#include "../IGUIKeySelector.h"


/**
 * Constructeur de OptionMenu
 */
OptionMenu::OptionMenu(Core* core) :
    GUIPage(core)
{
    setId(IN_OPTIONS_MENU);
}


/**
 * Destructeur de OptionMenu
 */
OptionMenu::~OptionMenu()
{
    //dtor
}


/**
 * Crée la page du menu
 */
irr::gui::IGUIElement* OptionMenu::createPage(irr::gui::IGUIEnvironment* guienv)
{
    irr::gui::IGUITabControl* tabControl;
    irr::gui::IGUITab *keys, *video, *audio, *misc;
    irr::gui::IGUIListBox *resolutionListBox;
    map<string, int> temp;

    irr::gui::IGUIElement* parent = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, guienv, guienv->getRootGUIElement(), -1,
            guienv->getRootGUIElement()->getAbsolutePosition()
    );

    /// \TODO Placer les éléments de la GUI en pourcentage
    tabControl = guienv->addTabControl(irr::core::rect<irr::s32>(150, 32, 600, 500), parent, false, false, GUI_OPTIONSMENU_TABCONTROL);

    //Tab KEYS
    keys = tabControl->addTab(L"KEYS", GUI_OPTIONSMENU_KEYS);
    temp = core->loadConfig("KEYS");
        //Forward
    guienv->addStaticText(L"Forward", irr::core::rect<irr::s32>(16, 16, 80, 32), false, false, keys, -1);
    IGUIKeySelector* keySelector = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_FORWARD, irr::core::rect<irr::s32>(96, 16, 112, 32));
    keys->addChild(keySelector);
    keySelector->setKey((irr::EKEY_CODE) temp["forward"]);
        //Backward
    guienv->addStaticText(L"Backward", irr::core::rect<irr::s32>(16, 48, 80, 64), false, false, keys, -1);
    IGUIKeySelector* keySelector1 = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_BACKWARD, irr::core::rect<irr::s32>(96, 48, 112, 64));
    keys->addChild(keySelector1);
    keySelector1->setKey((irr::EKEY_CODE) temp["backward"]);
        //Left
    guienv->addStaticText(L"Left : ", irr::core::rect<irr::s32>(16, 80, 80, 96), false, false, keys, -1);
    IGUIKeySelector* keySelector2 = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_LEFT, irr::core::rect<irr::s32>(96, 80, 112, 96));
    keys->addChild(keySelector2);
    keySelector2->setKey((irr::EKEY_CODE) temp["left"]);
        //Right
    guienv->addStaticText(L"Right : ", irr::core::rect<irr::s32>(16, 112, 80, 128), false, false, keys, -1);
    IGUIKeySelector* keySelector3 = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_RIGHT, irr::core::rect<irr::s32>(96, 112, 112, 128));
    keys->addChild(keySelector3);
    keySelector3->setKey((irr::EKEY_CODE) temp["right"]);
        //Shoot
    guienv->addStaticText(L"Shoot", irr::core::rect<irr::s32>(16, 144, 80, 160), false, false, keys, -1);
    IGUIKeySelector* keySelector4 = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_SHOOT, irr::core::rect<irr::s32>(96, 144, 112, 160));
    keys->addChild(keySelector4);
    keySelector4->setKey((irr::EKEY_CODE) temp["shoot"]);
        //Action
    guienv->addStaticText(L"Action", irr::core::rect<irr::s32>(16, 176, 80, 192), false, false, keys, -1);
    IGUIKeySelector* keySelector5 = new IGUIKeySelector(guienv, keys, GUI_OPTIONSMENU_KEYS_ACTION, irr::core::rect<irr::s32>(96, 176, 112, 192));
    keys->addChild(keySelector5);
    keySelector5->setKey((irr::EKEY_CODE) temp["action"]);
        //Apply/Return
    guienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), keys, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    guienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), keys, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab VIDEO
    video = tabControl->addTab(L"VIDEO", GUI_OPTIONSMENU_VIDEO);
    temp = core->loadConfig("VIDEO");

    // Resolution séléctionnée
    wchar_t selectedResoluction[20];
    swprintf(selectedResoluction, L"%dx%d %dbits",
            temp["width"], temp["height"],
            temp["bitdepth"]
    );

    //Resolution
    guienv->addStaticText(L"Resolution", irr::core::rect<irr::s32>(8, 16, 72, 36), false, false, video, -1);
    resolutionListBox = guienv->addListBox(irr::core::rect<irr::s32>(88, 16, 188, 86), video, GUI_OPTIONSMENU_VIDEO_RESOLUTION);

    irr::IrrlichtDevice* device = irr::createDevice(
            irr::video::EDT_NULL,
            irr::core::dimension2d<irr::u32>(800, 600),
            8, false,
            false, false
    );

    irr::video::IVideoModeList* l_videoMode = device->getVideoModeList();
    for(irr::s32 i=0; i<l_videoMode->getVideoModeCount(); i++) {
        irr::core::dimension2du resolution;
        resolution = l_videoMode->getVideoModeResolution(i);

        irr::s32 bitDepth = l_videoMode->getVideoModeDepth(i);

        if(bitDepth < 16)
            continue;

        wchar_t resoluctionString[20];
        swprintf(resoluctionString, L"%dx%d %dbits",
                resolution.Width, resolution.Height,
                l_videoMode->getVideoModeDepth(i)
        );

        resolutionListBox->addItem(resoluctionString);
    }
    resolutionListBox->setSelected(selectedResoluction);

    //Fullscreen
    guienv->addStaticText(L"Fullscreen", irr::core::rect<irr::s32>(8, 102, 72, 118), false, false, video, -1);
    guienv->addCheckBox(temp["fullscreen"], irr::core::rect<irr::s32>(88, 102, 104, 118), video, GUI_OPTIONSMENU_VIDEO_FULLSCREEN, L"");

    //Vsync
    guienv->addStaticText(L"Vsync", irr::core::rect<irr::s32>(8, 118, 72, 134), false, false, video, -1);
    guienv->addCheckBox(temp["vsync"], irr::core::rect<irr::s32>(88, 118, 104, 134), video, GUI_OPTIONSMENU_VIDEO_VSYNC, L"");

    //Apply/Return
    guienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), video, GUI_OPTIONSMENU_VIDEO_APPLIQUER, L"Appliquer");
    guienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), video, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab AUDIO
    audio = tabControl->addTab(L"AUDIO", GUI_OPTIONSMENU_AUDIO);

    //Apply/Return
    guienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), audio, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    guienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), audio, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab MISC
    misc = tabControl->addTab(L"MISC", GUI_OPTIONSMENU_MISC);

    //Apply/Return
    guienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), misc, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    guienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), misc, GUI_OPTIONSMENU_RETOUR, L"Retour");

    parent->setVisible(false);

    return parent;
}


// Callback
/**
 * Action lors de l'appuit sur escape
 */
void OptionMenu::onEscape()
{
    if(core->isPartieEnCours())
        core->setMenuState(IN_PAUSE_MENU);
    else
        core->setMenuState(IN_MAIN_MENU);
}
