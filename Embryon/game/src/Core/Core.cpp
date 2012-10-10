/** \file   Core.cpp
 *  \brief  Implémente la classe Core
 */
#include "Core.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "../Modules/Module.h"
#include "../GUI/ChooseLevelMenu.h"
#include "../GUI/GameMenu.h"
#include "../GUI/MainMenu.h"
#include "../GUI/OptionMenu.h"
#include "../GUI/PauseMenu.h"


/**
 * Constructeur de Core. Instancie le Logger
 * et met isRunning a true
 */
Core::Core() :
#ifdef _DEBUG
    logger("Core", true),
#else
    logger("Core", false),
#endif
    configLoader(),
    player(),
    level()
{
    log("Initialisation du Core");

    boost::mutex::scoped_lock l(mutexRunning);
    isRunning = true;
    setPartieEnCours(false);

    // Définitions des GUI
    l_GUIPage[IN_MAIN_MENU] = new MainMenu(this);
    l_GUIPage[IN_CHOOSE_LEVEL_MENU] = new ChooseLevelMenu(this);
    l_GUIPage[IN_OPTIONS_MENU] = new OptionMenu(this);
    l_GUIPage[IN_PAUSE_MENU] = new PauseMenu(this);
    l_GUIPage[IN_GAME] = new GameMenu(this);

    menuState = IN_MAIN_MENU;
}

/**
 * Destructeur de Core
 */
Core::~Core()
{
    //dtor
}


/**
 * Fonction appellée après l'enregistrement de tout les modules.
 * Lance les thread et attend leurs fin
 */
void Core::main()
{
    map<EnumModuleId, Module*>::iterator iteratorModule;

    // Lance les thread
    for(iteratorModule = l_module.begin();
        iteratorModule != l_module.end();
        iteratorModule++)
    {
        Module* module = iteratorModule->second;
        string name = module->getName();

        log("Lancement du thread '" + name + "'");
        module->setThread(new boost::thread(
                boost::bind(&Module::frame, module)
        ));
    }

    // Attend la fin des thread
    log("Attente de la fin des thread ...");
    for(iteratorModule = l_module.begin();
        iteratorModule != l_module.end();
        iteratorModule++)
    {
        Module* module = iteratorModule->second;

        module->getThread()->join();
    }

    log("Tout les thread sont termine");
    log("Fermeture de l'application");
}


/**
 * Fonction permettant d'enregistrer un module dans le core
 *
 * @ param module       Module que l'on souhaite enregistrer
 */
void Core::registerModule(Module* module)
{
    log("Enregistrement du module '" + module->getName() + "'");
    l_module[module->getId()] = module;
}


/**
 * Envoit un message vers un module
 *
 * @param msg       Message que l'on souhaite envoyer
 */
void Core::sendMessage(module_message& msg)
{
    try {
        l_module[msg.idDestination]->pushMessage(msg);
    } catch(int e) {}
}


/**
 * Indique si le programme tourne
 */
bool Core::getIsRunning() {
    boost::mutex::scoped_lock l(mutexRunning);
    return isRunning;
};


/**
 * Arrete le programme
 */
void Core::stop() {
    boost::mutex::scoped_lock l(mutexRunning);
    isRunning = false;

    map<EnumModuleId, Module*>::iterator iteratorModule;

    // Previent tout les modules que l'application doit finir
    for(iteratorModule = l_module.begin();
        iteratorModule != l_module.end();
        iteratorModule++)
    {
        Module* module = iteratorModule->second;

        module_message message = module_message(
                CORE, module->getId(),
                ACTION_QUITTER
        );

        module->pushMessage(message);
    }
}


/**
 * Sauvegarde la config donnée via le ConfigLoader
 *
 * @param section       Section de destination pour la sauvegarder
 * @param config        Map contenant les informations à
 *                      sauvegarder dans la section
 */
void Core::saveConfig(string section, map<string, int>& config)
{
    boost::mutex::scoped_lock l(mutexConfig);
    configLoader.saveConfig(section, config);
}


/**
 * Fonction de chargement de la configuration définie par section
 * via le configLoader
 *
 * @param section               Le nom de la section à charger
 *
 * @return                      Une map contenant les informations
 *                              chargées dans le fichier
 */
map<string, int> Core::loadConfig(string section)
{
    boost::mutex::scoped_lock l(mutexConfig);
    return configLoader.loadConfig(section);
}


/**
 * Simplifie l'utilisation du Logger
 *
 * @param message       Message que l'on souhaite afficher
 * @param level         Niveau d'importance du message (SIMPLE par défaut)
 */
void Core::log(string message, EnumLogLevel level)
{
    logger.log(message, level);
}


/**
 * Crée les pages de la GUI
 *
 * @param guienv            Environnment de la GUI
 * @param l_guiElement      Liste des GUI elements a modifier
 */
void Core::createGUIPage(irr::gui::IGUIEnvironment* guienv,
        map<int, irr::gui::IGUIElement*>* l_guiElement)
{
    map<EnumGameState, GUIPage*>::iterator iteratorPage;

    // Lance les thread
    for(iteratorPage = l_GUIPage.begin();
        iteratorPage != l_GUIPage.end();
        iteratorPage++)
    {
        EnumGameState pageId = iteratorPage->first;
        GUIPage* page = iteratorPage->second;

        (*l_guiElement)[pageId] = page->createPage(guienv);
    }
}


// Accesseur
/**
 * Indique si une partie est en cours
 *
 * @return      true si une partie a lieu
 */
bool Core::isPartieEnCours()
{
    boost::mutex::scoped_lock l(mutexPartieEnCours);
    return partieEnCours;
}


/**
 * Indique si une partie est en pause
 *
 * @return      true si une partie est en pause
 */
bool Core::isPartieEnPause()
{
    boost::mutex::scoped_lock l(mutexPartieEnPause);
    return partieEnPause;
}


/**
 * Donne acces au niveau
 *
 * @return      Pointeur sur le niveau
 */
Level* Core::getLevel()
{
    return &level;
}


/**
 * Donne acces au joueur
 *
 * @return      Pointeur sur le joueur
 */
Player* Core::getPlayer()
{
    return &player;
}


/**
 * Donne le menu affiché actuellement
 *
 * @return      Le numero du menu affiché
 */
EnumGameState Core::getMenuState()
{
    boost::mutex::scoped_lock l(mutexMenuState);
    return menuState;
}


/**
 * Recupére les menu
 *
 * @return              Liste des menu
 */
map<EnumGameState, GUIPage*>* Core::getGUIPages()
{
    return &l_GUIPage;
}

// Mutateur
/**
 * Definit si une partie a lieu ou non
 * Garantit que partieEnPause vaut false si on partieEnCours vaut false
 */
void Core::setPartieEnCours(bool partieEnCours)
{
    // Si on quitte la partie
    if(partieEnCours)
        setPartieEnPause(false);

    boost::mutex::scoped_lock l(mutexPartieEnCours);
    this->partieEnCours = partieEnCours;
}


/**
 * Definit si une partie est en pause
 * Garantit que partieEnPause vaut false si on partieEnCours vaut false
 */
void Core::setPartieEnPause(bool partieEnPause)
{
    boost::mutex::scoped_lock l(mutexPartieEnPause);
    // Si aucune partie en cours, ne peut valoir que false
    if(!isPartieEnCours())
        this->partieEnPause = false;
    else {
        this->partieEnPause = partieEnPause;
        l.unlock();

        // Previent le RenderingEngine
        module_message msg(CORE, RENDERING, ACTION_UNPAUSE);
        if(partieEnPause)
            msg.codeAction = ACTION_PAUSE;

        sendMessage(msg);
    }
}


/**
 * Modifie le menu affiché actuellement
 */
void Core::setMenuState(EnumGameState menuState)
{
    boost::mutex::scoped_lock l(mutexMenuState);
    this->menuState = menuState;

    module_message msg = module_message(CORE, RENDERING, ACTION_AFFICHER_MENU);
    msg.intData["menu"] = menuState;
    sendMessage(msg);
}

