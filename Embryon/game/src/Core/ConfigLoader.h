/** \file   ConfigLoader.h
 *  \brief  Définit la classe ConfigLoader.
 */
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <iostream>
#include <map>

using namespace std;

/** \class  ConfigLoader
 *  \brief  Permet de charger la configuration.
 *
 *  Fournit différentes méthodes permettant d'accéder aux différentes
 *  section définie dans le fichier de config.
 */
class ConfigLoader
{
    public:
        ConfigLoader();
        virtual ~ConfigLoader();

        void saveConfig(string section, map<string, int>& config);
        map<string, int> loadConfig(string section);
    protected:
    private:
        void createConfig();
};


#endif
