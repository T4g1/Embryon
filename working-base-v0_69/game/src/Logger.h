/** \file   Logger.h
 *  \brief  Définit la classe Logger
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>

#include "common.h"

using namespace std;

/** \enum   EnumLogLevel
 *  \brief  Niveaux de log que le logger peut gérer
 */
enum EnumLogLevel {
    AUCUN=0, SIMPLE=1, WARNING=2, ERROR=3
};


/** \class  Logger
 *  \brief  Permet d'afficher des messages dans la console.
 *
 * Les messages affichés peuvent être accompagné de différentes options
 * permettant de les faire ressortir dans le log en fonction de leurs
 * importance.
 */
class Logger
{
    public:
        Logger(string name, bool debugEnabled);
        virtual ~Logger();

        void log(string message, EnumLogLevel level=SIMPLE);

        // Mutateurs
        void toggleDebugMode(bool debugEnabled);

        // Accesseurs
        bool getDebugMode();
    protected:
    private:
        string name;
        bool debugEnabled;

        ofstream fichier;
};

#endif // LOGGER_H
