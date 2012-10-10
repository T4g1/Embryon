/** \file   Core.h
 *  \brief  Définit la classe Core.
 */
#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <map>
#include <boost/thread/mutex.hpp>

#include "ConfigLoader.h"
#include "../common.h"
#include "../Logger.h"
#include "../module_message.h"

using namespace std;

class Module;


/** \class  Core
 *  \brief  Gére les interactions entre les différents Module
 *
 * Enregistre tout les Module lors de leurs définition et leurs fournit
 * différentes méthodes leurs permettant de s'envoyer des messages et des
 * données.
 */
class Core
{
    public:
        Core();
        virtual ~Core();

        void main();
        void registerModule(Module* module);

        void sendMessage(module_message& msg);

        // Simplifie l'acces au logger
        void log(string message, EnumLogLevel level=SIMPLE);

        bool getIsRunning();
        void stop();

        void saveConfig(string section, map<string, int>& config);
        map<string, int> loadConfig(string section);
    protected:
    private:
        map<EnumModuleId, Module*>  l_module;
        Logger logger;

        boost::mutex mutexConfig;
        ConfigLoader configLoader;

        // L'application tourne
        boost::mutex mutexRunning;
        bool isRunning;
};

#endif // CORE_H
