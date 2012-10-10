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
    public irr::scene::ICollisionCallback,
    public irr::video::IShaderConstantSetCallBack
{
    public:
        RenderingEngine(
                Core* core, EventsEngine* eventsEngine);
        virtual ~RenderingEngine();

        void initialize();
        void unInitialize();
        void frame();

        void loadRenderingConfig();

        // Callback de collisions joueur
        bool onCollision (
                const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
    protected:
    private:
        EventsEngine* eventsEngine;

        irr::IrrlichtDevice* mDevice;
        irr::video::IVideoDriver* mDriver;
        irr::scene::ISceneManager* mSmgr;
        irr::gui::IGUIEnvironment* mGuienv;

        irr::core::line3d<irr::f32> mouseRay;

        // Modéles du jeu
        irr::scene::ICameraSceneNode* camera;
        irr::scene::IAnimatedMeshSceneNode* nodePlayer;

        irr::s32 celMat;

        map<int, irr::gui::IGUIElement*> l_guiElement;

        int currentMenu;

        // DEBUG
        irr::scene::ISceneNode* selectedSceneNode;
        irr::scene::ISceneNode* mob;
        irr::scene::IBillboardSceneNode* bill;

        //Mouse
        irr::video::ITexture* cursor;
        irr::core::dimension2d<irr::s32> cursorDimension;

        void processMessage(module_message& msg);
        void constructLevel(string name);
        void applyConfigChanges();

        // Récupére les informations du joueur
        void refreshPlayer();

        // Callback de shaders
        virtual void OnSetConstants(
                irr::video::IMaterialRendererServices* services,
                irr::s32 userData
        );
};

#endif // RENDERINGENGINE_H
