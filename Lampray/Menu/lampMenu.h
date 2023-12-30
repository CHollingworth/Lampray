//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPMENU_H
#define LAMP_LAMPMENU_H

#include "../../third-party/imgui/imgui.h"
#include "../Filesystem/lampFS.h"
#include "../Control/lampGames.h"
#include "../Control/lampControl.h"
namespace Lamp::Core{
    class lampMenu{
    public:
        enum Menus{
            LICENCE_MENU,
            GAME_MOD_MENU,
            GAME_CONFIG_MENU,
            CUSTOMIZE
        };

        Menus currentMenu = LICENCE_MENU;
        bool userRequestedQuit = false;

        void RunMenus();


        bool show7zerror = false;

    private:

        char profileBuffer[250]{};

        bool deployCheck = false;

        void IntroMenu();
        void ModMenu();
        void GameConfigMenu();
        std::string pfCopyStr;
        bool pfCopy;
        bool displayProfileCreateMenu;
        void createProfileDialog();
        void DefaultMenuBar();
        void DisplayNotifications();
    };
}
#endif //LAMP_LAMPMENU_H
