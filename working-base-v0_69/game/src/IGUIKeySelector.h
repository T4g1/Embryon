/** \file   IGUIKeySelector.h
 *  \brief  Définit la classe IGUIKeySelector
 */
#ifndef IGUIKEYSELECTOR_H
#define IGUIKEYSELECTOR_H

#include <irrlicht.h>
#include <string>

#include "common.h"

using namespace std;


/** \class  IGUIKeySelector
 *  \brief  IGUIElement personnalisé permettant de demander une touche
 *          a l'utilisateur.
 */
class IGUIKeySelector : public irr::gui::IGUIElement
{
    public:
        IGUIKeySelector(irr::gui::IGUIEnvironment* guiEnv,
                irr::gui::IGUIElement *parent, irr::s32 id,
                irr::core::rect<irr::s32> rect);
        ~IGUIKeySelector();

        virtual void draw();
        virtual bool OnEvent(const irr::SEvent& event);

        // Mutateurs
        void setKey(irr::EKEY_CODE);
        void closeInput();

        // Accesseurs
        irr::EKEY_CODE getValue();

        static string KeyNames[];
    protected:
    private:
        irr::EKEY_CODE key;
        string text;
};

#endif // IGUIKEYSELECTOR_H
