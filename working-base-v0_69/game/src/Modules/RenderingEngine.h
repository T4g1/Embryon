/** \file   RenderingEngine.h
 *  \brief  Définit la classe RenderingEngine.
 */
#ifndef RENDERINGENGINE_H
#define RENDERINGENGINE_H

#include <irrlicht.h>
#include <map>

#include "Module.h"

using namespace std;

class Core;
class GameEngine;
class EventsEngine;


/** \class  RenderingEngine
 *  \brief  Gére l'affichage et les différents rendu via Irrlicht.
 *
 * Tout les appels concerant Irrlicht sont confinés dans ce Module, la création
 * de chaque menu se fait par l'une des méthodes de ce Module.
 *
 * La création du rendu des différents éléments du jeu se fait dans ce Module.
 * Gére également les collisions du joueur avec l'environnement.
 */
class RenderingEngine :
    public Module,
    public irr::video::IShaderConstantSetCallBack
{
    public:
        RenderingEngine(
                Core* core, GameEngine* gameEngine, EventsEngine* eventsEngine);
        virtual ~RenderingEngine();

        void initialize();
        void frame();

        void loadRenderingConfig();
    protected:
    private:
        GameEngine* gameEngine;
        EventsEngine* eventsEngine;

        irr::IrrlichtDevice* mDevice;
        irr::video::IVideoDriver* mDriver;
        irr::scene::ISceneManager* mSmgr;
        irr::gui::IGUIEnvironment* mGuienv;

        irr::s32 celMat;

        map<int, irr::gui::IGUIElement*> l_guiElement;

        int currentMenu;

        // DEBUG
        irr::scene::ISceneNode* highlightedSceneNode;
        irr::scene::IBillboardSceneNode* bill;

        void processMessage(module_message& msg);

        void constructLevel(string name);

        // Fonctions d'affichage de menu
        void createMenuPrincipal();
        void createMenuOptions();
        void createMenuChooseLevel();
        void createMenuJeu();
        void createMenuPause();
        void applyConfigChanges(int);

        // Callback de shaders
        virtual void OnSetConstants(
                irr::video::IMaterialRendererServices* services,
                irr::s32 userData
        );
};

#endif // RENDERINGENGINE_H
