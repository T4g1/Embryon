/** \file   main.cpp
 *  \brief  Instancie le Core et les Module
 *
 *  \mainpage Projet Embryon
 *
 *  \section intro_sec Introduction
 *
 * Ce document contient la documentation compléte concernant le projet Embryon
 * générée par Doxygen à partir des sources du projet.
 *
 * \section descr_sec Membres du projet Embryon
 *
 * \subsection dev Développeurs
 *
 * T4g1<br/>
 * Aichan<br/>
 * Gh0st<br/>
 * Manorad
 *
 * \subsection inf Ingoraphistes
 *
 * Legrey<br/>
 * Monkiller
 *
 * \subsection sce Scénariste
 *
 * Manorad
 */
#include <iostream>

#include "Core/Core.h"

#include "Modules/EventsEngine.h"
#include "Modules/GameEngine.h"
#include "Modules/GUIEngine.h"
#include "Modules/RenderingEngine.h"

using namespace std;


int main()
{
    // Déclaration du core
    Core core;

    // Déclaration des modules
    GameEngine      game(&core);
    GUIEngine       gui(&core);
    EventsEngine    events(&core, &game, &gui);
    RenderingEngine rendering(&core, &game, &events);
    //Module ia("IA", &core);

    try {
        core.main();
    } catch(int e) {
    }

    return 0;
}
