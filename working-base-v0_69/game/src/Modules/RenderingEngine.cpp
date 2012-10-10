/** \file   RenderingEngine.cpp
 *  \brief  Implémente la classe RenderingEngine
 */
#include "RenderingEngine.h"

#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>
#include <time.h>

#include "../Core/Core.h"
#include "GameEngine.h"
#include "EventsEngine.h"
#include "../Level.h"
#include "../IGUIKeySelector.h"


/**
 * Constructeur du module de gestion du jeu
 *
 * @param core              Reference vers le Core du programme
 * @param gameEngine        Reference vers le module gameEngine
 * @param eventsEngine      Reference vers le module eventsEngine
 */
RenderingEngine::RenderingEngine(
        Core* core, GameEngine* gameEngine, EventsEngine* eventsEngine) :
    Module(RENDERING, "Rendering", core)
{
    this->gameEngine = gameEngine;
    this->eventsEngine = eventsEngine;

    currentMenu = IN_MAIN_MENU;
}

/**
 * Destructeur du module de gestion de l'affichage
 */
RenderingEngine::~RenderingEngine()
{
}


/**
 * Initialisation du module
 */
void RenderingEngine::initialize()
{
    mDevice = irr::createDevice(
            irr::video::EDT_OPENGL,
            irr::core::dimension2d<irr::u32>(config["width"], config["height"]), 32,
            config["fullscreen"], false, config["vsync"], eventsEngine
    );

    mDevice->setWindowCaption(L"Embryon - Initialisation");

    mDriver = mDevice->getVideoDriver();
    mSmgr = mDevice->getSceneManager();
    mGuienv = mDevice->getGUIEnvironment();

    // Initialise les sceneManager
    createMenuPrincipal();
    createMenuChooseLevel();
    createMenuOptions();
    createMenuJeu();
    createMenuPause();

    l_guiElement[IN_MAIN_MENU]->setVisible(false);
    l_guiElement[IN_CHOOSE_LEVEL_MENU]->setVisible(false);
    l_guiElement[IN_OPTIONS_MENU]->setVisible(false);
    l_guiElement[IN_GAME]->setVisible(false);
    l_guiElement[IN_PAUSE_MENU]->setVisible(false);

    l_guiElement[currentMenu]->setVisible(true);

    // Shaders
    irr::video::IGPUProgrammingServices* gpu = mDriver->getGPUProgrammingServices();

    celMat = gpu->addHighLevelShaderMaterialFromFiles(
        "../../media/shaders/celVertex.hlsl", "main", irr::video::EVST_VS_1_1,
        "../../media/shaders/celPixel.hlsl", "main", irr::video::EPST_PS_1_1, this
    );
}


/**
 * Traitement du module
 * Tout les appels a Ogre doivent être fait ici
 */
void RenderingEngine::frame()
{
    clock_t beginTime, endTime;
    clock_t sleepTime, frameTime = (1.0 / (float)FPS) * CLOCKS_PER_SEC;

    loadRenderingConfig();
    initialize();

    irr::core::position2d<irr::s32> mousePos;
    irr::core::vector3df position;
    irr::core::line3d<irr::f32> mouseRay;

    // Gestionnaire de collision
    irr::scene::ISceneCollisionManager* collisionManager;
    collisionManager = mSmgr->getSceneCollisionManager();

    while(core->getIsRunning() && mDevice->run()) {
        beginTime = clock();

        // Gére sa liste de message
        processQueue();

        if(gameEngine->isPartieEnCours() && !gameEngine->isPartieEnPause()) {
            // Modifie la position du joueur
            gameEngine->getPlayer()->updatePosition();

            if (highlightedSceneNode) {
                highlightedSceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, true);
                highlightedSceneNode = 0;
            }

            // Ligne de visée avec la souris
            mousePos = mDevice->getCursorControl()->getPosition();
            mouseRay = collisionManager->getRayFromScreenCoordinates(mousePos);

            // Donne le mouseRay au joueur
            gameEngine->getPlayer()->setMouseRay(mouseRay);

            irr::core::vector3df intersection;
            irr::core::triangle3df hitTriangle;

            // Selection d'obejts
            irr::scene::ISceneNode * selectedSceneNode;
            selectedSceneNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
                    mouseRay, intersection, hitTriangle, 0, 0
            );

            if(selectedSceneNode) {
                // Place le marqueur sur le point de collision
                bill->setPosition(intersection);    // DEUBG

                highlightedSceneNode = selectedSceneNode;
                highlightedSceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            }
        }

        mDriver->beginScene(true, true, irr::video::SColor(0xff88aadd));

        if(!gameEngine->isPartieEnPause())
            mSmgr->drawAll();

        if(gameEngine->isPartieEnCours() && !gameEngine->isPartieEnPause()) {
            mDriver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

            // Affiche la ligne de visée
            mDriver->draw3DLine(
                    gameEngine->getPlayer()->getViseurRay().start,
                    gameEngine->getPlayer()->getViseurRay().end,
                    irr::video::SColor(255,0,255,255)
            );
        }

        l_guiElement[currentMenu]->draw();

        mDriver->endScene();

        // Limitation du FPS
        endTime = clock();
        if(endTime - beginTime < frameTime) {
            sleepTime = frameTime - (endTime - beginTime);
            boost::this_thread::sleep(
                    boost::posix_time::milliseconds(sleepTime)
            );
        }
    }

    // Si on a pas prevenu de l'arret du jeu
    if(core->getIsRunning())
        core->stop();

    // Suppression des elements de GUI
    l_guiElement[IN_MAIN_MENU]->drop();
    l_guiElement[IN_CHOOSE_LEVEL_MENU]->drop();
    l_guiElement[IN_OPTIONS_MENU]->drop();
    l_guiElement[IN_GAME]->drop();
    l_guiElement[IN_PAUSE_MENU]->drop();

    mDevice->drop();
}


/**
 * Charge la config du module
 */
void RenderingEngine::loadRenderingConfig()
{
    // Chargement de la config
    config = core->loadConfig("VIDEO");

    if(config.find("width") == config.end())        config["width"] = 800;
    if(config.find("height") == config.end())       config["height"] = 600;
    if(config.find("fullscreen") == config.end())   config["fullscreen"] = 0;
    if(config.find("vsync") == config.end())        config["vsync"] = 0;

    core->saveConfig("VIDEO", config);
}


/**
 * Traitement d'un message recu par le module
 *
 * @param msg           Message recu recu
 */
void RenderingEngine::processMessage(module_message& msg)
{
    module_message mesg = module_message(getId(), GUI, ACTION_CHANGE_MENU);
    switch(msg.codeAction) {
        case ACTION_AFFICHER_MENU:
            l_guiElement[currentMenu]->setVisible(false);
            currentMenu = msg.intData["menu"];
            l_guiElement[currentMenu]->setVisible(true);
            break;
        case ACTION_INIT_GAME:
            constructLevel(msg.strData["niveau"]);
            mDevice->getCursorControl()->setVisible(false);
            break;
        case ACTION_PAUSE:
            mDevice->getTimer()->stop();
            mSmgr->getActiveCamera()->setInputReceiverEnabled(false);
            mDevice->getCursorControl()->setVisible(true);
            break;
        case ACTION_UNPAUSE:
            mDevice->getTimer()->start();
            mSmgr->getActiveCamera()->setInputReceiverEnabled(true);
            //mDevice->getCursorControl()->setVisible(false);
            mDevice->getCursorControl()->setPosition(
                    config["width"]/2,
                    config["height"]/2
            );
            break;
        case ACTION_SAVE_CONFIG_KEYS:
            applyConfigChanges(GUI_OPTIONSMENU_KEYS);
            mesg.idDestination = EVENTS;
            mesg.codeAction = ACTION_RELOAD_CONFIG_KEYS;
            core->sendMessage(mesg);
        break;
        case ACTION_SAVE_CONFIG_VIDEO:
            applyConfigChanges(GUI_OPTIONSMENU_VIDEO);
            break;
        default: break;


    }
}


/**
 * Cree le menu principal
 */
void RenderingEngine::createMenuPrincipal()
{
    l_guiElement[IN_MAIN_MENU] = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, mGuienv, mGuienv->getRootGUIElement(), -1,
            mGuienv->getRootGUIElement()->getAbsolutePosition()
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 50, 110, 82),
            l_guiElement[IN_MAIN_MENU],
            GUI_MAINMENU_JOUER,
            L"Jouer", L"Lance une nouvelle partie"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 90, 110, 122),
            l_guiElement[IN_MAIN_MENU],
            GUI_MAINMENU_OPTIONS,
            L"Options", L"Modifie les options de jeu"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 130, 110, 162),
            l_guiElement[IN_MAIN_MENU],
            GUI_MAINMENU_QUITTER,
            L"Quitter", L"Termine l'application"
    );
}


/**
 * Cree le menu de choix de partie
 */
void RenderingEngine::createMenuChooseLevel()
{
    l_guiElement[IN_CHOOSE_LEVEL_MENU] = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, mGuienv, mGuienv->getRootGUIElement(), -1,
            mGuienv->getRootGUIElement()->getAbsolutePosition()
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 50, 110, 82),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU1,
            L"Chapitre 1", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 90, 110, 122),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU2,
            L"Chapitre 2", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 130, 110, 162),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU3,
            L"Chapitre 3", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 170, 110, 202),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU4,
            L"Chapitre 4", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 210, 110, 242),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU5,
            L"Chapitre 5", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 250, 110, 282),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_NIVEAU6,
            L"Chapitre 6", L"Martine et le parc naturel"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 290, 110, 322),
            l_guiElement[IN_CHOOSE_LEVEL_MENU],
            GUI_CHOOSELEVELMENU_RETOUR,
            L"Retour", L"Retour a l'ecran titre"
    );
}


/**
 * Cree le menu options
 */
void RenderingEngine::createMenuOptions()
{
    irr::gui::IGUITabControl* tabControl;
    irr::gui::IGUITab *keys, *video, *audio, *misc;
    irr::gui::IGUIListBox *resolutionListBox;
    map<string, int> temp;



    l_guiElement[IN_OPTIONS_MENU] = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, mGuienv, mGuienv->getRootGUIElement(), -1,
            mGuienv->getRootGUIElement()->getAbsolutePosition()
    );









    /// \TODO Placer les éléments de la GUI en pourcentage
    tabControl = mGuienv->addTabControl(irr::core::rect<irr::s32>(150, 32, 600, 500), l_guiElement[IN_OPTIONS_MENU], false, false, GUI_OPTIONSMENU_TABCONTROL);

    //Tab KEYS
    keys = tabControl->addTab(L"KEYS", GUI_OPTIONSMENU_KEYS);
    temp = core->loadConfig("KEYS");
        //Forward
    mGuienv->addStaticText(L"Forward", irr::core::rect<irr::s32>(16, 16, 80, 32), false, false, keys, -1);
    IGUIKeySelector* keySelector = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_FORWARD, irr::core::rect<irr::s32>(96, 16, 112, 32));
    keys->addChild(keySelector);
    keySelector->setKey((irr::EKEY_CODE) temp["forward"]);
        //Backward
    mGuienv->addStaticText(L"Backward", irr::core::rect<irr::s32>(16, 48, 80, 64), false, false, keys, -1);
    IGUIKeySelector* keySelector1 = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_BACKWARD, irr::core::rect<irr::s32>(96, 48, 112, 64));
    keys->addChild(keySelector1);
    keySelector1->setKey((irr::EKEY_CODE) temp["backward"]);
        //Left
    mGuienv->addStaticText(L"Left : ", irr::core::rect<irr::s32>(16, 80, 80, 96), false, false, keys, -1);
    IGUIKeySelector* keySelector2 = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_LEFT, irr::core::rect<irr::s32>(96, 80, 112, 96));
    keys->addChild(keySelector2);
    keySelector2->setKey((irr::EKEY_CODE) temp["left"]);
        //Right
    mGuienv->addStaticText(L"Right : ", irr::core::rect<irr::s32>(16, 112, 80, 128), false, false, keys, -1);
    IGUIKeySelector* keySelector3 = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_RIGHT, irr::core::rect<irr::s32>(96, 112, 112, 128));
    keys->addChild(keySelector3);
    keySelector3->setKey((irr::EKEY_CODE) temp["right"]);
        //Shoot
    mGuienv->addStaticText(L"Shoot", irr::core::rect<irr::s32>(16, 144, 80, 160), false, false, keys, -1);
    IGUIKeySelector* keySelector4 = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_SHOOT, irr::core::rect<irr::s32>(96, 144, 112, 160));
    keys->addChild(keySelector4);
    keySelector4->setKey((irr::EKEY_CODE) temp["shoot"]);
        //Action
    mGuienv->addStaticText(L"Action", irr::core::rect<irr::s32>(16, 176, 80, 192), false, false, keys, -1);
    IGUIKeySelector* keySelector5 = new IGUIKeySelector(mGuienv, keys, GUI_OPTIONSMENU_KEYS_ACTION, irr::core::rect<irr::s32>(96, 176, 112, 192));
    keys->addChild(keySelector5);
    keySelector5->setKey((irr::EKEY_CODE) temp["action"]);
        //Apply/Return
    mGuienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), keys, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    mGuienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), keys, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab VIDEO
    video = tabControl->addTab(L"VIDEO", GUI_OPTIONSMENU_VIDEO);
    temp = core->loadConfig("VIDEO");

    //Conversion int -> STR
    string width, height;
    stringstream out, out2;
    out << temp["width"];
    width = out.str();
    out2 << temp["height"];
    height = out2.str();
    string item(width + "x" + height);
    wstring ws;
    ws.assign(item.begin(), item.end());
     const wchar_t* res = ws.c_str();
        //Resolution
    mGuienv->addStaticText(L"Resolution", irr::core::rect<irr::s32>(8, 16, 72, 36), false, false, video, -1);
    resolutionListBox = mGuienv->addListBox(irr::core::rect<irr::s32>(88, 16, 188, 86), video, GUI_OPTIONSMENU_VIDEO_RESOLUTION);
    resolutionListBox->addItem(L"800x600");
    resolutionListBox->addItem(L"1024x768");
    resolutionListBox->addItem(L"1280x600");
    resolutionListBox->addItem(L"1280x720");
    resolutionListBox->addItem(L"1280x768");
    resolutionListBox->addItem(L"1360x768");
    resolutionListBox->addItem(L"1366x768");
    resolutionListBox->setSelected(res);

    bool FSBool, VSBool;
    if(temp["fullscreen"])
        FSBool = true;
    if(temp["vsync"])
        VSBool = true;
        //Fullscreen
    mGuienv->addStaticText(L"Fullscreen", irr::core::rect<irr::s32>(8, 102, 72, 118), false, false, video, -1);
    mGuienv->addCheckBox(FSBool, irr::core::rect<irr::s32>(88, 102, 104, 118), video, GUI_OPTIONSMENU_VIDEO_FULLSCREEN, L"");
        //Vsync
    mGuienv->addStaticText(L"Vsync", irr::core::rect<irr::s32>(8, 118, 72, 134), false, false, video, -1);
    mGuienv->addCheckBox(VSBool, irr::core::rect<irr::s32>(88, 118, 104, 134), video, GUI_OPTIONSMENU_VIDEO_VSYNC, L"");
        //Apply/Return
    mGuienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), video, GUI_OPTIONSMENU_VIDEO_APPLIQUER, L"Appliquer");
    mGuienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), video, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab AUDIO
    audio = tabControl->addTab(L"AUDIO", GUI_OPTIONSMENU_AUDIO);

        //Apply/Return
    mGuienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), audio, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    mGuienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), audio, GUI_OPTIONSMENU_RETOUR, L"Retour");

    //Tab MISC
    misc = tabControl->addTab(L"MISC", GUI_OPTIONSMENU_MISC);

        //Apply/Return
    mGuienv->addButton(irr::core::rect<irr::s32>(96, 200, 196, 232), misc, GUI_OPTIONSMENU_KEYS_APPLIQUER, L"Appliquer");
    mGuienv->addButton(irr::core::rect<irr::s32>(212, 200, 312, 232), misc, GUI_OPTIONSMENU_RETOUR, L"Retour");
}


/**
 * Cree la GUI du jeu
 */
void RenderingEngine::createMenuJeu()
{
    l_guiElement[IN_GAME] = new irr::gui::IGUIElement(
            irr::gui::EGUIET_ELEMENT, mGuienv, mGuienv->getRootGUIElement(), -1,
            mGuienv->getRootGUIElement()->getAbsolutePosition()
    );
}


/**
 * Cree le menu pause
 */
void RenderingEngine::createMenuPause()
{
    l_guiElement[IN_PAUSE_MENU] = new irr::gui::IGUIElement(
        irr::gui::EGUIET_ELEMENT, mGuienv, mGuienv->getRootGUIElement(), -1,
        mGuienv->getRootGUIElement()->getAbsolutePosition()
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 50, 110, 82),
            l_guiElement[IN_PAUSE_MENU],
            GUI_PAUSEMENU_CONTINUER,
            L"Continuer", L"Reprends la partie"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 90, 110, 122),
            l_guiElement[IN_PAUSE_MENU],
            GUI_PAUSEMENU_OPTIONS,
            L"Options", L"Modifier les options"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 130, 110, 162),
            l_guiElement[IN_PAUSE_MENU],
            GUI_PAUSEMENU_QUITTERPARTIE,
            L"Quitter la partie", L"Retourne a l'ecran titre"
    );

    mGuienv->addButton(
            irr::core::rect<irr::s32>(10, 170, 110, 202),
            l_guiElement[IN_PAUSE_MENU],
            GUI_PAUSEMENU_QUITTER,
            L"Quitter", L"Termine l'application"
    );
}


/**
 * Fonction callback appellée pour lors de l'utilisation des shaders.
 * Permet de fournir des valeurs dynamique aux shaders
 *
 * @param services      Permet de communiquer avec les shaders
 * @param userData      Paramétre donné a la fonction (pas utilisé)
 */
void RenderingEngine::OnSetConstants(
        irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    irr::core::matrix4 world = mDriver->getTransform(irr::video::ETS_WORLD);

    irr::core::matrix4 invWorld;
    world.getInverse(invWorld);

    //irr::core::vector3df lightPosOS;
    //invWorld.transformVect(lightPosOS, lightPosition);
    //services->setPixelShaderConstant("LightDir", &lightPosOS.X, 3);

    int tex_1 = 0; //the index you previously set up the texture
    services->setPixelShaderConstant("Texture0",(float*)(&tex_1),1);

    float val = 0.2; // Taille des bords
    services->setPixelShaderConstant("silhouetteThreshold", &val, 1);
}


/**
 * Charge le niveau
 *
 * @param niveau        Nom du niveau que l'on veut charger
 */
void RenderingEngine::constructLevel(string name)
{
    // Supprimme ce qui pourrait deja exister
    gameEngine->getPlayer()->setCamera(0);
    gameEngine->getPlayer()->setNode(0);
    mSmgr->clear();

    highlightedSceneNode = 0;

    // Chargement du niveau
    mDevice->getFileSystem()->addZipFileArchive(
            ("../../media/maps/" + name).c_str()
    );

    mDevice->getFileSystem()->addZipFileArchive("../../media/maps/test1.zip");
    irr::scene::IQ3LevelMesh* const meshMap =
            (irr::scene::IQ3LevelMesh*) mSmgr->getMesh("test1.bsp");

    // On l'ajoute a la scene
    irr::scene::IMeshSceneNode* nodeMap = mSmgr->addOctreeSceneNode(
            meshMap->getMesh(irr::scene::quake3::E_Q3_MESH_GEOMETRY),
            0, -1, 1024
    );

    nodeMap->setPosition(irr::core::vector3df(0,0,0));
    nodeMap->setScale(irr::core::vector3df(1,1,1));

    // Charge les effets spéciaux
    irr::scene::IMesh* const additional_mesh =
            meshMap->getMesh(irr::scene::quake3::E_Q3_MESH_ITEMS);

    for(irr::u32 i=0; i!=additional_mesh->getMeshBufferCount(); i++) {
        const irr::scene::IMeshBuffer* meshBuffer =
                additional_mesh->getMeshBuffer(i);
        const irr::video::SMaterial& material = meshBuffer->getMaterial();

        const irr::s32 shaderIndex = (irr::s32) material.MaterialTypeParam2;
        const irr::scene::quake3::IShader* shader = meshMap->getShader(shaderIndex);
        if(shader == 0)
            continue;

        mSmgr->addQuake3SceneNode(meshBuffer, shader);
    }

    //for(int i=0; i<node->getMaterialCount(); i++)
    //    nodeMap->getMaterial(i).MaterialType = (irr::video::E_MATERIAL_TYPE)celMat;

    // Charge les entitées
    Level* level = gameEngine->getLevel();
    level->setEntityList(meshMap->getEntityList());

    // Récupére le point de départ du joueur
    map<irr::core::stringc, irr::core::stringc> entity =
            level->getEntity("info_player_start");

    irr::u32 pos = 0;
    irr::core::vector3df playerStart =
            irr::scene::quake3::getAsVector3df(entity["origin"], pos);

    // Création du joueur
    irr::scene::ISceneNode* playerNode;
    playerNode = mSmgr->addCubeSceneNode(10.0f, 0, SCENE_NODE_PLAYER);
    playerNode->setMaterialFlag(irr::video::EMF_LIGHTING, true);

    // Ajoute la gestion des sur le joueur
    irr::scene::ITriangleSelector* selector =
            mSmgr->createOctreeTriangleSelector(
                    nodeMap->getMesh(), nodeMap, 128);
    nodeMap->setTriangleSelector(selector);

    irr::scene::ISceneNodeAnimatorCollisionResponse* anim;
    anim = mSmgr->createCollisionResponseAnimator(
            selector, playerNode,
            irr::core::vector3df(20,20,20),     // Taille de l'ellipse
            irr::core::vector3df(0,0,0),        // Gravité
            irr::core::vector3df(0,0,0)         // Translation dans l'ellipse
    );
    anim->setCollisionCallback(gameEngine->getPlayer());

    selector->drop();

    playerNode->addAnimator(anim);
    anim->drop();

    // Ajoute une caméra à la scène
    irr::scene::ICameraSceneNode* camera = mSmgr->addCameraSceneNode();

    // Enregistre le playerNode et la camera dans l'objet Player
    gameEngine->getPlayer()->setCamera(camera);
    gameEngine->getPlayer()->setNode(playerNode);
    gameEngine->getPlayer()->setPosition(playerStart);

    // DEBUG MODELE
    irr::scene::IAnimatedMesh* mesh = mSmgr->getMesh("../../media/models/sydney.md2");
    irr::scene::IAnimatedMeshSceneNode* node = mSmgr->addAnimatedMeshSceneNode( mesh );
    node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    node->setMD2Animation(irr::scene::EMAT_STAND);
    node->setPosition(playerStart);
    node->setMaterialTexture( 0, mDriver->getTexture("../../media/textures/sydney.bmp") );

    selector = mSmgr->createTriangleSelector(node);
    node->setTriangleSelector(selector);
    selector->drop();

    // DEBUG BILLBOARD
    bill = mSmgr->addBillboardSceneNode();
    bill->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR );
    //bill->setMaterialTexture(0, mDriver->getTexture("../../media/textures/lialique.bmp"));
    bill->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    bill->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    bill->setSize(irr::core::dimension2d<irr::f32>(10.0f, 10.0f));
}


/**
 * Fonction appelée lors du click du bouton "appliquer" dans
 * les menus de configuration
 *
 * @param idsection     identifiant de la section concernée (utilisation des #define  enumerés)
 */
void RenderingEngine::applyConfigChanges(int idsection)
{
    map<string, int> config;

    //! \TODO : Ajouter une gestion de KEYS, AUDIO et MISC


    //Video Stuff
    bool fullscreenBool, vsyncBool;
    irr::s32 resolution;
    const wchar_t * resolutionName;
    int fullscreen = 0, vsync = 0, width, height;
    string resolutionString, widthString, heightString;
    irr::gui::IGUIElement* checkFull;
    irr::gui::IGUIElement* checkVsync ;
    irr::gui::IGUIElement* resolutionList;
    //KEYS stuff
    IGUIKeySelector* keyForward;
    IGUIKeySelector* keyBackward;
    IGUIKeySelector* keyLeft;
    IGUIKeySelector* keyRight;
    IGUIKeySelector* keyShoot;
    IGUIKeySelector* keyAction;


    switch(idsection){
        case GUI_OPTIONSMENU_KEYS:
            //On recupere les elements, et on les map
            keyForward = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_FORWARD, true);
            config["forward"] = keyForward->getValue();
            keyBackward = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_BACKWARD, true);
            config["backward"] = keyBackward->getValue();
            keyLeft = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_LEFT, true);
            config["left"] = keyLeft->getValue();
            keyRight = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_RIGHT, true);
            config["right"] = keyRight->getValue();
            keyShoot = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_SHOOT, true);
            config["shoot"] = keyShoot->getValue();
            keyAction = (IGUIKeySelector*) l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_KEYS_ACTION, true);
            config["action"] = keyAction->getValue();
            core->saveConfig("KEYS", config);
        break;

        case GUI_OPTIONSMENU_VIDEO:

            //On recupere les elements
            checkFull = l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_VIDEO_FULLSCREEN, true);
            checkVsync = l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_VIDEO_VSYNC, true);
            resolutionList = l_guiElement[IN_OPTIONS_MENU]->getElementFromId(GUI_OPTIONSMENU_VIDEO_RESOLUTION, true);
            //On recupere les valeurs en "raw"
            resolution = ((irr::gui::IGUIListBox*)(resolutionList))->getSelected();
            resolutionName = ((irr::gui::IGUIListBox*)(resolutionList))->getListItem(resolution);
            fullscreenBool = ((irr::gui::IGUICheckBox*)(checkFull))->isChecked();
            vsyncBool = ((irr::gui::IGUICheckBox*)(checkVsync))->isChecked();
            // Bool -> int
            if(fullscreenBool)
                fullscreen = 1;
            if(vsyncBool)
                vsync = 1;

            //wchar_t * -> str | split str | str -> int width & height
            resolutionString = wchar_to_string(resolutionName);
            widthString = resolutionString.substr(0, resolutionString.find("x"));
            heightString = resolutionString.substr(resolutionString.find("x")+1, resolutionString.size() - resolutionString.find("x"));
            width = atoi(widthString.c_str());
            height = atoi(heightString.c_str());

            //mapping des valeurs
            config["width"] = width;
            config["height"] = height;
            config["fullscreen"] = fullscreen;
            config["vsync"] = vsync;
            core->saveConfig("VIDEO", config);

        break;
        case GUI_OPTIONSMENU_AUDIO:

        break;
        case GUI_OPTIONSMENU_MISC:

        break;
    }
}
