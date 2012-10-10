/** \file   Core.cpp
 *  \brief  Implémente la classe Core
 */
#include "Core.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "../Modules/Module.h"


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
    configLoader()
{
    log("Initialisation du Core");

    boost::mutex::scoped_lock l(mutexRunning);
    isRunning = true;
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
        module->thread = boost::thread(
                boost::bind(&Module::frame, module)
        );
    }

    // Attend la fin des thread
    log("Attente de la fin des thread ...");
    for(iteratorModule = l_module.begin();
        iteratorModule != l_module.end();
        iteratorModule++)
    {
        Module* module = iteratorModule->second;

        module->thread.join();
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
