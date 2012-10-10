/** \file   RenderingEngine.cpp
 *  \brief  Implémente la classe RenderingEngine
 */
#include "RenderingEngine.h"

#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>

#include "../Core/Core.h"
#include "../Level.h"
#include "EventsEngine.h"
#include "../IGUIKeySelector.h"


/**
 * Constructeur du module de gestion du jeu
 *
 * @param core              Reference vers le Core du programme
 * @param gameEngine        Reference vers le module gameEngine
 * @param eventsEngine      Reference vers le module eventsEngine
 */
RenderingEngine::RenderingEngine(
        Core* core, EventsEngine* eventsEngine) :
    Module(RENDERING, "Rendering", core)
{
    this->eventsEngine = eventsEngine;

    currentMenu = IN_MAIN_MENU;

    mDevice = NULL;

    l_guiElement[IN_MAIN_MENU] = NULL;
    l_guiElement[IN_CHOOSE_LEVEL_MENU] = NULL;
    l_guiElement[IN_OPTIONS_MENU] = NULL;
    l_guiElement[IN_GAME] = NULL;
    l_guiElement[IN_PAUSE_MENU] = NULL;
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
            irr::core::dimension2d<irr::u32>(config["width"], config["height"]),
            config["bitdepth"], config["fullscreen"],
            true, config["vsync"], eventsEngine
    );

    mDevice->setWindowCaption(L"Projet Embryon");

    mDriver = mDevice->getVideoDriver();
    mSmgr = mDevice->getSceneManager();
    mGuienv = mDevice->getGUIEnvironment();

    // Initialise les GUIPage
    core->createGUIPage(mGuienv, &l_guiElement);

    l_guiElement[currentMenu]->setVisible(true);

    // Shaders
    //irr::video::IGPUProgrammingServices* gpu = mDriver->getGPUProgrammingServices();

    //celMat = gpu->addHighLevelShaderMaterialFromFiles(
    //    "../../media/shaders/celVertex.hlsl", "main", irr::video::EVST_VS_1_1,
    //   "../../media/shaders/celPixel.hlsl", "main", irr::video::EPST_PS_1_1, this
    //);

    mDriver->enableMaterial2D();
    cursor = mDriver->getTexture("../../media/pointer_shoot.png");
    mDriver->makeColorKeyTexture (cursor, irr::core::position2d<irr::s32> (0,0));
    cursorDimension = cursor->getSize();
}


/**
 * Clotûre la fenêtre propement
 */
void RenderingEngine::unInitialize()
{
    // Suppression des elements de GUI
    if(l_guiElement[IN_MAIN_MENU])          l_guiElement[IN_MAIN_MENU]->drop();
    if(l_guiElement[IN_CHOOSE_LEVEL_MENU])  l_guiElement[IN_CHOOSE_LEVEL_MENU]->drop();
    if(l_guiElement[IN_OPTIONS_MENU])       l_guiElement[IN_OPTIONS_MENU]->drop();
    if(l_guiElement[IN_GAME])               l_guiElement[IN_GAME]->drop();
    if(l_guiElement[IN_PAUSE_MENU])         l_guiElement[IN_PAUSE_MENU]->drop();

    if(mDevice)                             mDevice->drop();
}


/**
 * Traitement du module
 * Tout les appels a Irrlicht doivent être fait ici
 */
void RenderingEngine::frame()
{
    log("Debut de la gestion de l'affichage");
    loadRenderingConfig();

    irr::u32 beginTime, endTime, deltaTime, sleepTime;
    irr::f32 frameTime = (1.0 / (float)FPS) * CLOCKS_PER_SEC;

    initialize();

    irr::core::position2d<irr::s32> mousePos;
    irr::core::vector3df position;
    irr::core::line3d<irr::f32> mouseRay;

    // Gestionnaire de collision
    irr::scene::ISceneCollisionManager* collisionManager;
    collisionManager = mSmgr->getSceneCollisionManager();

    beginTime = getTime();
    while(core->getIsRunning() && mDevice->run()) {
        // Gére sa liste de message
        processQueue();

        if(core->isPartieEnCours() && !core->isPartieEnPause()) {
            /******************
            // PLAYER
            ******************/

            // Ligne de visée avec la souris
            mousePos = mDevice->getCursorControl()->getPosition();
            mouseRay = collisionManager->getRayFromScreenCoordinates(mousePos);

            // Donne le mouseRay au joueur
            core->getPlayer()->setMouseRay(mouseRay);

            // Modifie la position du joueur
            core->getPlayer()->updatePosition();

            // Rafraichit le joueur
            refreshPlayer();


            /******************
            // SURVOL DE NODE
            ******************/

            irr::core::vector3df intersection;
            irr::core::triangle3df hitTriangle;

            // Selection d'obejts
            selectedSceneNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
                    mouseRay, intersection, hitTriangle, SCENE_NODE_ENTITY, 0
            );

            if(selectedSceneNode) {
                // Place le marqueur sur le point de collision
                bill->setPosition(intersection);    // DEBUG
            }
        }

        mDriver->beginScene(true, true, irr::video::SColor(0xff88aadd));

        if(!core->isPartieEnPause())
            mSmgr->drawAll();

        if(core->getMenuState() == IN_GAME) {
            mDriver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

            // Récupére la ligne de visée
            irr::core::line3df viseur3D = core->getPlayer()->getViseurRay();

            // Affiche la ligne de visée
            mDriver->draw3DLine(
                    viseur3D.start, viseur3D.end,
                    irr::video::SColor(255,0,255,255)
            );

            mousePos.X -= cursorDimension.Width / 2;
            mousePos.Y -= cursorDimension.Height / 2;
            // Curseur
            mDriver->draw2DImage(cursor, mousePos, irr::core::rect<irr::s32>(0,0,128,128), 0, irr::video::SColor(255, 255, 255, 255), true);
        }

        l_guiElement[currentMenu]->draw();


        mDriver->endScene();

        irr::core::stringw tmp(L"Projet Embryon [");
        tmp += mDriver->getFPS();
        tmp += L" fps]";

        mDevice->setWindowCaption(tmp.c_str());

        // Limitation du FPS
        endTime = getTime();
        deltaTime = endTime - beginTime;
        beginTime = endTime;
        if(deltaTime < frameTime) {
            sleepTime = frameTime - deltaTime;
            boost::this_thread::sleep(
                    boost::posix_time::milliseconds(sleepTime)
            );
        }
    }

    // Si on a pas prevenu de l'arret du jeu
    if(core->getIsRunning())
        core->stop();

    unInitialize();

    log("Fin de la gestion de l'affichage");
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
    if(config.find("bitdepth") == config.end())     config["bitdepth"] = 16;

    core->saveConfig("VIDEO", config);
}


/**
 * Traitement d'un message recu par le module
 *
 * @param msg           Message recu recu
 */
void RenderingEngine::processMessage(module_message& msg)
{
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
            mDevice->getCursorControl()->setVisible(false);
            mDevice->getCursorControl()->setPosition(
                    config["width"]/2,
                    config["height"]/2
            );
            break;
        case ACTION_SAVE_CONFIG:
            applyConfigChanges();
            break;
        case ACTION_PLAYER_ACTION:
            core->getLevel()->triggerEntityBySceneNode(
                    selectedSceneNode, ENTITY_ACTIVATION_ACTIONNED);
            break;
        default: break;
    }
}


/**
 * Charge le niveau
 *
 * @param niveau        Nom du niveau que l'on veut charger
 */
void RenderingEngine::constructLevel(string name)
{
    // Supprimme ce qui pourrait deja exister
    mSmgr->clear();

    // Chargement du niveau
    mDevice->getFileSystem()->addFileArchive (
            ("../../media/maps/" + name).c_str()
    );

    mDevice->getFileSystem()->addFileArchive ("../../media/maps/test1.zip");
    irr::scene::IQ3LevelMesh* meshMap =
            (irr::scene::IQ3LevelMesh*) mSmgr->getMesh("test1.bsp");

    // On l'ajoute a la scene
    irr::scene::IMeshSceneNode* nodeMap = mSmgr->addOctreeSceneNode(
            meshMap->getMesh(irr::scene::quake3::E_Q3_MESH_GEOMETRY),
            0, SCENE_NODE_MAP, 1024
    );
    nodeMap->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    nodeMap->setMaterialType(irr::video::EMT_LIGHTMAP_LIGHTING);

    // Charge les effets spéciaux
    irr::scene::IMesh* const additional_mesh =
            meshMap->getMesh(irr::scene::quake3::E_Q3_MESH_GEOMETRY);
    cout << meshMap->getMeshBufferCount() << endl;
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

    /********************************************
    // JOUEUR
    ********************************************/

    // Création du joueur
    irr::scene::IAnimatedMesh* meshPlayer = mSmgr->getMesh("../../media/models/sydney.md2");
    nodePlayer = mSmgr->addAnimatedMeshSceneNode(meshPlayer, nodeMap, SCENE_NODE_PLAYER);
    nodePlayer->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    nodePlayer->setMD2Animation(irr::scene::EMAT_STAND);
    nodePlayer->setMaterialTexture(0, mDriver->getTexture("../../media/textures/sydney.bmp") );
    nodePlayer->addShadowVolumeSceneNode(meshPlayer, -1, false);

    irr::scene::IMetaTriangleSelector* MetaColisionTriangle;
    MetaColisionTriangle = mSmgr->createMetaTriangleSelector();

    // Ajoute la gestion des collisions sur le joueur
    irr::scene::ITriangleSelector* selector;
    selector = mSmgr->createOctreeTriangleSelector(
            nodeMap->getMesh(), nodeMap, 128);
    nodeMap->setTriangleSelector(selector);
    MetaColisionTriangle->addTriangleSelector(selector);
    selector->drop();

    /********************************************
    // ENTITY
    ********************************************/

    // Charge les entitées
    Level* level = core->getLevel();
    level->loadEntityList(meshMap);
    level->initializeCollisionsEntities(mSmgr, meshMap, nodePlayer);
    level->attachEntitiesToCore(core);

    level->setEntityList(meshMap->getEntityList());

    // Récupére le point de départ du joueur
    map<irr::core::stringc, irr::core::stringc> entity =
            level->getEntity("info_player_start");

    irr::u32 pos = 0;
    irr::core::vector3df playerStart =
            irr::scene::quake3::getAsVector3df(entity["origin"], pos);

    // Enregistre la position du joueur
    core->getPlayer()->setPosition(playerStart);

    /********************************************
    // CAMERA
    ********************************************/

    // Ajoute une caméra à la scène
    camera = mSmgr->addCameraSceneNode();

    /*****************************************
    // DEBUG MODELE
    *****************************************/

    // DEBUG MOB
    irr::scene::IAnimatedMesh* mesh = mSmgr->getMesh("../../media/models/sydney.md2");
    irr::scene::IAnimatedMeshSceneNode* node = mSmgr->addAnimatedMeshSceneNode(mesh, nodeMap, SCENE_NODE_MOBS);
    node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    node->setMD2Animation(irr::scene::EMAT_STAND);
    playerStart.X -= 40;
    node->setPosition(playerStart);
    node->setMaterialTexture(0, mDriver->getTexture("../../media/textures/sydney.bmp") );
    node->addShadowVolumeSceneNode(meshPlayer, -1, false);

    //! Collisions avec la map
    irr::scene::ISceneNodeAnimatorCollisionResponse* anim;
    anim = mSmgr->createCollisionResponseAnimator(
            nodeMap->getTriangleSelector(), node,
            irr::core::vector3df(10,24,10),     // Taille de l'ellipse
            irr::core::vector3df(0,GRAVITY,0),  // Gravité
            irr::core::vector3df(0,0,0)         // Translation dans l'ellipse
    );

    node->addAnimator(anim);
    anim->drop();

    //! Collisions avec le joueur
    // Selecteur
    selector = mSmgr->createTriangleSelector(node->getMesh()->getMesh(0), node);
    node->setTriangleSelector(selector);

    // Collision reponse
    anim = mSmgr->createCollisionResponseAnimator(
            selector, nodePlayer,
            irr::core::vector3df(10,25,10),
            irr::core::vector3df(0,0,0),
            irr::core::vector3df(0,0,0)
    );
    anim->setCollisionCallback(this);

    selector->drop();
    nodePlayer->addAnimator(anim);
    anim->drop();

    // DEBUG BILLBOARD
    bill = mSmgr->addBillboardSceneNode();
    bill->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR );
    bill->setMaterialTexture(0, mDriver->getTexture("../../media/textures/lialique.bmp"));
    bill->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    bill->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    bill->setSize(irr::core::dimension2d<irr::f32>(10.0f, 10.0f));

    /***********************************************
    // COLLISIONS
    ***********************************************/

    // Collision du jouer avec les selector définit
    anim = mSmgr->createCollisionResponseAnimator(
            MetaColisionTriangle, nodePlayer,
            irr::core::vector3df(10,25,10),     // Taille de l'ellipse
            irr::core::vector3df(0,0,0),  // Gravité
            irr::core::vector3df(0,0,0)         // Translation dans l'ellipse
    );
    anim->setCollisionCallback(this);

    nodePlayer->addAnimator(anim);
    anim->drop();

    mSmgr->setAmbientLight(irr::video::SColorf(0.1, 0.1, 0.1,0.0));

    playerStart.Y += 40;
    playerStart.Z -= 40;
    irr::scene::ILightSceneNode* lumiere = mSmgr->addLightSceneNode(
            nodeMap, playerStart,
            irr::video::SColorf(0.6f, 0.6f, 0.6f, 0.0f),
            150.0f
    );
    lumiere->enableCastShadow();
}


/**
 * Fonction appelée lors du click du bouton "appliquer" dans
 * les menus de configuration
 *
 * @param idsection     identifiant de la section concernée (utilisation des #define  enumerés)
 */
void RenderingEngine::applyConfigChanges()
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

    module_message msg(getId(), EVENTS, ACTION_RELOAD_CONFIG_KEYS);

    wstring infoMessage = L"La configuration a été modifiée avec succés !";

    irr::gui::IGUITabControl* tabControl = (irr::gui::IGUITabControl*)l_guiElement[IN_OPTIONS_MENU]->getElementFromId(
            GUI_OPTIONSMENU_TABCONTROL
    );

    int idsection = tabControl->getTab(tabControl->getActiveTab())->getID();

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

            // Demande a Events de recharger sa config
            core->sendMessage(msg);
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

            infoMessage += L"\nCertains changement nécéssitent toutefois un redémarrage pour être effectif ...";

        break;
        case GUI_OPTIONSMENU_AUDIO:

        break;
        case GUI_OPTIONSMENU_MISC:

        break;
    }

    irr::gui::IGUIElement* box;
    box = mGuienv->addMessageBox(
            L"Config appliquée",
            infoMessage.c_str(),
            true, irr::gui::EMBF_OK, l_guiElement[IN_OPTIONS_MENU]
    );
}


/**
 * Applique la position du joueur sur la camera et le nodePlayer
 */
void RenderingEngine::refreshPlayer()
{
    Player* player = core->getPlayer();

    nodePlayer->setPosition(player->getPosition());
    nodePlayer->setRotation(player->getRotation());

    camera->setPosition(player->getCameraPosition());
    camera->setTarget(player->getPosition());
}


// Callback
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
 * Appelé en cas de colision
 *
 * @param animator      Infos sur la colision
 */
bool RenderingEngine::onCollision(
        const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
    // Objet qui sur lequel la colision a lieu
    irr::scene::ISceneNode* nodeCollide = animator.getTargetNode();

    // Si c'est le joueur
    if(nodeCollide->getID() == SCENE_NODE_PLAYER) {
        core->getPlayer()->onCollision(animator);
        refreshPlayer();
    } else {
        cout << "Collision non geree: " << nodeCollide->getID() <<endl;
    }

    return true;
}
