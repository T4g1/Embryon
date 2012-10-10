/** \file   Module.h
 *  \brief  Définit la classe Module
 */
#ifndef MODULE_H
#define MODULE_H

#include <map>
#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "../common.h"
#include "../Logger.h"
#include "../module_message.h"

using namespace std;

class Core;


/** \class  Module
 *  \brief  Classe mére de tout les module du jeu.
 *
 * Définit les composants de base de chaque module.
 * Fournit une interface permettant au Core d'envoyer des messages
 * vers n'importe quel module.
 */
class Module
{
    public:
        Module(EnumModuleId id, string name, Core* core);
        virtual ~Module();


        /**
         * Boucle principale de chaque module
         */
        virtual void frame() = 0;

        // Simplifie l'appel au logger
        void log(string message, EnumLogLevel level=SIMPLE);

        void pushMessage(module_message& msg);

        // Accesseurs
        EnumModuleId getId();
        string getName();
        boost::thread* getThread();

        // Mutateurs
        void setThread(boost::thread* thread);
    private:
    protected:
        EnumModuleId id;
        string name;
        Core* core;

        boost::thread* thread;

        map<string, int> config;

        Logger logger;

        // File de message recu
        boost::mutex mutexQueue;
        boost::condition_variable condQueue;
        queue<module_message> message_queue;

        void processQueue();
        virtual void processMessage(module_message&) = 0;
};

#endif // MODULE_H
