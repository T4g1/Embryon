/** \file   ConfigLoader.cpp
 *  \brief  Implémente la classe ConfigLoader
 */
#include "ConfigLoader.h"
#include "irrlicht.h"

#include <iostream>
#include <string>
#include <map>
#include <fstream>


/**
 * Constructeur de la class ConfigLoader
 */
ConfigLoader::ConfigLoader()
{
    // ctor
}

/**
 * Destructeur de ConfigLoader
 */
ConfigLoader::~ConfigLoader()
{
    // dtor
}


/**
 * Fonction permettant de créer ou de recréer le fichier "config.ini"
 * si celui-ci est inexistant ou corrompu
 */
void ConfigLoader::createConfig()
{
    ofstream file("config.ini", ios::in | ios::trunc);

    if(file)
        file.close();
}


/**
 * Fonction permettant de sauvegarder la configuration dans la section convenue
 *
 * @param section       Section de destination pour la sauvegarder
 * @param config        Map contenant les informations à
 *                      sauvegarder dans la section
 */
void ConfigLoader::saveConfig(string section, map<string, int>& config)
{
    map<string, int>::iterator iterator;
    map<string, int> configTemp;
    ifstream file("config.ini", ios::in);
    ofstream file2("config.tmp", ios::out | ios::trunc);
    string readLine;

    if(file && file2){
        // On se rend a la section qui nous interesse
        do {
            getline(file, readLine);
            file2 << readLine << endl;
        } while(!file.eof() && readLine != "[" + section + "]");

        // On ne trouve pas
        if(readLine != "[" + section + "]") {
            cout << "Sauvegarde de " << section << ": Ajout de la section" << endl;
            file2 << "[" << section << "]" << endl;
        }

        // On charge la section que l'on souhaite modifier
        configTemp = loadConfig(section);

        // On applique les modifications a la config chargée
        for(iterator = config.begin();
            iterator != config.end();
            iterator++)
        {
            string key = iterator->first;
            int value = iterator->second;

            configTemp[key] = value;
        }

        // Ecrit les nouvelles valeurs et les anciennes
        for(iterator = configTemp.begin();
            iterator != configTemp.end();
            iterator++)
        {
            string key = iterator->first;
            int value = iterator->second;

            file2 << key << '=' << value << endl;
        }

        // Se rend a la prochaine section
        while(getline(file, readLine)) {
            if(readLine[0] == '[') {
                file2 << readLine << endl;
                break;
            }
        }

        // Reecrit le reste du fichier
        while(getline(file, readLine))
            file2 << readLine << endl;

        file.close();
        file2.close();

        remove("config.ini");
        rename("config.tmp", "config.ini");
    }
}


/**
 * Fonction de chargement de la configuration définie par section
 *
 * @param section               Le nom de la section à charger
 *
 * @return                      Une map contenant les informations
 *                              chargées dans le fichier
 */
map<string, int> ConfigLoader::loadConfig(string section)
{
    map<string, int> config;
    map<string, int>::iterator iteratorConfig;
    ifstream file("config.ini", ios::in);
    string readLine, param, value;

    if(!file.fail()){
        do {
            getline(file, readLine);
        } while(!file.eof() && readLine != "[" + section + "]");

        // On ne trouve pas
        if(readLine != "[" + section + "]") {
            cout << "Chargement de " << section << ": Section non trouvée" << endl;
            file.close();
            return config;
        }

        while(getline(file, readLine)){
            // S'arrete a la section suivante
            if(readLine[0] == '[')
                break;

            param = readLine.substr(0, readLine.find("="));
            value = readLine.substr(readLine.find("=")+1, readLine.size() - readLine.find("="));

            // Si la ligne n'est pas vide
            if(param != "")
                config[param] = atoi(value.c_str());
        }

        file.close();
    } else {
        file.clear(ios::failbit);
        ConfigLoader::createConfig();
    }

    return config;
}
