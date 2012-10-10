/** \file   Logger.cpp
 *  \brief  Implémente la classe Logger
 */
#include "Logger.h"

#include <time.h>


/**
 * Constructeur du logger. Crée le fichier de log
 */
/// \todo n'ouvrir le fichier qu'a son utilisation
Logger::Logger(string name, bool debugEnabled) :
    fichier((name + ".log").c_str(), ios::out)
{
    this->name = name;

    toggleDebugMode(debugEnabled);
}

/**
 * Destructeur du Logger
 */
Logger::~Logger()
{
    //dtor
}


/**
 * Fonction permettant d'afficher un message de debugage
 *
 * @param message       Message
 * @param level         Importance du message
 */
void Logger::log(string message, EnumLogLevel level)
{
    if(!debugEnabled)
        return;

    // Message personnalisé en fonction du niveau d'erreur
    if(level == WARNING)
        cout << "<WARNING> ";
    else if(level == ERROR)
        cout << "<-ERROR-> ";

    cout << "(" << clock() << ") " << name << ": " << message << endl;
    fichier << "(" << clock() << ") " << name << ": " << message << endl;
}


// Mutateurs
/**
 * Active ou désactive le mode de debugage
 *
 * @param debugEnabled      Activer ou desactiver
 */
void Logger::toggleDebugMode(bool debugEnabled)
{
    this->debugEnabled = true;

    if(debugEnabled)
        log("Mode DEBUG active");
    else
        log("Mode DEBUG desactive");

    this->debugEnabled = debugEnabled;
}


// Accesseurs
/**
 * Permet de savoir si le mode debug est activé
 *
 * @return      true si le mode debug est activé, false sinon
 */
bool Logger::getDebugMode()
{
    return debugEnabled;
}
