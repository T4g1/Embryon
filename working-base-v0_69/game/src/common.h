/** \file   common.h
 *  \brief  Définit plusieurs fonctons et énumérations
 *          utile dans tout le programme.
 */
#ifndef COMMON_H
#define COMMON_H

#include <boost/multi_array.hpp>
#include <string>

using namespace std;

#define FPS             50


/** \enum   EnumModuleId
 *  \brief  Contient les identifiants des différents modules.
 */
enum EnumModuleId {
    CORE, RENDERING, EVENTS, GAME, GUI
};

/** \enum   EnumCodesAction
 *  \brief  Définit tout les codes possibles pour les messages.
 */
enum EnumCodesAction {
    ACTION_AUCUNE=0,
    ACTION_QUITTER,

    // CODE ACTION CORE

    // CODE ACTION RENDERING
    ACTION_AFFICHER_MENU,
    ACTION_INIT_GAME,

    // CODE ACTION EVENTS

    // CODE ACTION GAME
    ACTION_NOUVELLE_PARTIE,
    ACTION_PAUSE,
    ACTION_UNPAUSE,
    ACTION_QUITTER_PARTIE,
    ACTION_START_WALKING_FORWARDS,
    ACTION_START_WALKING_BACKWARDS,
    ACTION_START_STRAFE_LEFT,
    ACTION_START_STRAFE_RIGHT,
    ACTION_STOP_WALKING_FORWARDS,
    ACTION_STOP_WALKING_BACKWARDS,
    ACTION_STOP_STRAFE_LEFT,
    ACTION_STOP_STRAFE_RIGHT,

    // CODE ACTION GUI
    ACTION_CHANGE_MENU,
    ACTION_SAVE_CONFIG_KEYS,
    ACTION_RELOAD_CONFIG_KEYS,
    ACTION_SAVE_CONFIG_VIDEO
};

/** \enum   EnumGameState
 *  \brief  Définit tout les états de jeu possible.
 */
enum EnumGameState {
    IN_MAIN_MENU=0,
    IN_CHOOSE_LEVEL_MENU,
    IN_OPTIONS_MENU,
    IN_PAUSE_MENU,
    IN_GAME
};


/** \enum   EnumGameSceneNode
 *  \brief  Définit tout les scenes nodes utilisé
 */
enum EnumGameSceneNode {
    SCENE_NODE_PLAYER
};

/** \enum   EnumCodeMenuItems
 *  \brief  Définiti tout les éléments de GUI utilisé.
 */
enum EnumCodeMenuItems {
    // MENU PRINCIPAL
    GUI_MAINMENU_JOUER=101,
    GUI_MAINMENU_OPTIONS,
    GUI_MAINMENU_QUITTER,

    // MENU CHOOSE LEVEL
    GUI_CHOOSELEVELMENU_NIVEAU1,
    GUI_CHOOSELEVELMENU_NIVEAU2,
    GUI_CHOOSELEVELMENU_NIVEAU3,
    GUI_CHOOSELEVELMENU_NIVEAU4,
    GUI_CHOOSELEVELMENU_NIVEAU5,
    GUI_CHOOSELEVELMENU_NIVEAU6,
    GUI_CHOOSELEVELMENU_RETOUR,

    // MENU OPTIONS
    GUI_OPTIONSMENU_TABCONTROL,
    GUI_OPTIONSMENU_RETOUR,
    GUI_OPTIONSMENU_KEYS,
    GUI_OPTIONSMENU_KEYS_APPLIQUER,
    GUI_OPTIONSMENU_KEYS_FORWARD,
    GUI_OPTIONSMENU_KEYS_BACKWARD,
    GUI_OPTIONSMENU_KEYS_LEFT,
    GUI_OPTIONSMENU_KEYS_RIGHT,
    GUI_OPTIONSMENU_KEYS_SHOOT,
    GUI_OPTIONSMENU_KEYS_ACTION,
    GUI_OPTIONSMENU_VIDEO,
    GUI_OPTIONSMENU_VIDEO_APPLIQUER,
    GUI_OPTIONSMENU_VIDEO_RESOLUTION,
    GUI_OPTIONSMENU_VIDEO_FULLSCREEN,
    GUI_OPTIONSMENU_VIDEO_VSYNC,
    GUI_OPTIONSMENU_AUDIO,
    GUI_OPTIONSMENU_MISC,


    // MENU PAUSE
    GUI_PAUSEMENU_CONTINUER,
    GUI_PAUSEMENU_OPTIONS,
    GUI_PAUSEMENU_QUITTERPARTIE,
    GUI_PAUSEMENU_QUITTER
};

/** \enum   EnumCodePlayerAction
 *  \brief  Définit toutes les actions que le joueur peut réaliser
 */
enum EnumCodePlayerAction {
    AVANCER,
    RECULER,
    GAUCHE,
    DROITE
};

string wchar_to_string(const wchar_t *);

#endif // COMMON_H
