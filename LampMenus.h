//
// Created by charles on 02/09/23.
//

#ifndef LAMP_LAMPMENUS_H
#define LAMP_LAMPMENUS_H


#include "imgui/imgui.h"

class LampMenus {
public:
    void Menu();
private:
    void MainMenu();
    ImGuiIO io;
    bool bg3_conf = false;
    void MenuBar();
    ImGuiWindowFlags static CreateFlags();
};


#endif //LAMP_LAMPMENUS_H
