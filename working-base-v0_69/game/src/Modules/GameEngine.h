/** \file   GameEngine.h
 *  \brief  Définit la classe GameEngine.
 */
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <boost/thread/mutex.hpp>

#include "Module.h"
#include "../Player.h"
#include "../Level.h"

using namespace std;

class Core;


/** \class GameEngine
 *  \brief Gére la dynamique du jeu.
 *
 * Gére tout les éléments constituant le jeu en lui même. Gére le joueur, le
 * niveau, les éléments de décors, les items, ...
 *
 * Gére la mise en pause du jeu, la fin des niveaux, le lancement
 * des cinématiques, ...
 */
class GameEngine : public Module
{
    public:
        GameEngine(Core* core);
        virtual ~GameEngine();

        void frame();

        // Accesseurs
        bool isPartieEnCours();
        bool isPartieEnPause();
        Level* getLevel();
        Player* getPlayer();
    protected:
    private:
        Level level;

        Player player;

        // Si une partie est en cours
        boost::mutex mutexPartieEnCours;
        bool partieEnCours;

        // Si la pause est activée
        boost::mutex mutexPartieEnPause;
        bool partieEnPause;

        void newGame(int niveau);

        void setPartieEnCours(bool partieEnCours);
        void setPartieEnPause(bool partieEnPause);

        void processMessage(module_message& msg);
};

#endif // GAMEENGINE_H
