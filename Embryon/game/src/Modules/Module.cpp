/** \file   Module.cpp
 *  \brief  Implémente la classe Module
 */
#include "Module.h"

#include "../Core/Core.h"


/**
 * Constructeur d'un module
 *
 * @param id                            Identifiant du module
 * @param name                          Nom du module
 * @param core                          Reference vers le Core du programme
 */
Module::Module(EnumModuleId id, string name, Core* core) :
#ifdef _DEBUG
    logger(name, true)
#else
    logger(name, false)
#endif
{
    this->id = id;
    this->name = name;
    this->core = core;

    thread = NULL;

    log("Initialisation du module '" + getName() + "'");

    core->registerModule(this);
}

/**
 * Destructeur d'un module
 */
Module::~Module()
{
    if(thread)  delete thread;
}


/**
 * Ajout d'un message dans la file
 *
 * @param msg       Message que l'on ajoute
 */
void Module::pushMessage(module_message& msg)
{
    boost::mutex::scoped_lock l(mutexQueue);
    message_queue.push(msg);
    l.unlock();

    condQueue.notify_one();
}


/**
 * Traite la file de message
 */
void Module::processQueue() {
    while(true) {
        boost::mutex::scoped_lock l(mutexQueue);

        if(message_queue.empty())
            return;

        module_message e = message_queue.front();
        message_queue.pop();
        l.unlock();

        processMessage(e);
    }
};


/**
 * Simplifie l'utilisation du Logger
 *
 * @param message       Message que l'on souhaite afficher
 * @param level         Niveau d'importance du message (SIMPLE par défaut)
 */
void Module::log(string message, EnumLogLevel level)
{
    logger.log(message, level);
}


// Accesseurs
/**
 * Donne l'identifiant du Module
 *
 * @return  Identifiant du Module
 */
EnumModuleId Module::getId()
{
    return id;
}


/**
 * Donne le nom du Module.
 * Seulement utile pour le Logger.
 *
 * @return  Nom du Module
 */
string Module::getName()
{
    return name;
}


/**
 * Donne un pointeur sur le thread
 *
 * @return          Pointeur sur le thread
 */
boost::thread* Module::getThread()
{
    return thread;
}


// Mutateurs
/**
 * Modifie le thread
 *
 * @param thread        Pointeur sur le thread
 */
void Module::setThread(boost::thread* thread)
{
    this->thread = thread;
}
