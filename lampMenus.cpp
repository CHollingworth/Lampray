//
// Created by charles on 11/09/23.
//

#include "lampMenus.h"
#include "lampConfig.h"
#include "lampFilesystem.h"
#include "game-data/BG3/BG3.h"
#include "game-data/gameControl.h"
#include "lampWarn.h"

void Lamp::Core::lampMenus::CreateMenus() {

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize, 0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    switch (currentMenu) {
        case LAMP_INTRO_MENU:
            if(Lamp::Core::lampConfig::getInstance().ShowIntroMenu) {
                IntroMenu();
            }else{
                currentMenu = LAMP_GAME_SELECT;
            }
            break;
        case LAMP_GAME_SELECT:
            if(Lamp::Core::lampConfig::getInstance().CurrentGame == lampConfig::UNK){
                GameSelect();
            }else{
                currentMenu = GAME_MOD_MENU;
            }
            break;
        case GAME_MOD_MENU:
            ModMenu();
            break;
        case GAME_CONFIG_MENU:
            GameConfigMenu();
            break;
    }
}

void Lamp::Core::lampMenus::IntroMenu() {
    ImGui::Begin("Requires a title, but we've disabled it.", NULL, DefaultFlags());
    ImGui::Separator();
    ImGuiStyle& style = ImGui::GetStyle();
    float size = ImGui::CalcTextSize("Lamp").x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("Lamp");

    size = ImGui::CalcTextSize("Linux Application Modding Platform").x + style.FramePadding.x * 2.0f;
    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("Linux Application Modding Platform");
    ImGui::Separator();
    ImGui::Text("This is free and unencumbered software released into the public domain.\n"
                "\n"
                "Anyone is free to copy, modify, publish, use, compile, sell, or\n"
                "distribute this software, either in source code form or as a compiled\n"
                "binary, for any purpose, commercial or non-commercial, and by any\n"
                "means.\n"
                "\n"
                "In jurisdictions that recognize copyright laws, the author or authors\n"
                "of this software dedicate any and all copyright interest in the\n"
                "software to the public domain. We make this dedication for the benefit\n"
                "of the public at large and to the detriment of our heirs and\n"
                "successors. We intend this dedication to be an overt act of\n"
                "relinquishment in perpetuity of all present and future rights to this\n"
                "software under copyright law.\n"
                "\n"
                "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n"
                "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
                "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"
                "IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR\n"
                "OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\n"
                "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR\n"
                "OTHER DEALINGS IN THE SOFTWARE.\n"
                "\n"
                "For more information, please refer to <https://unlicense.org>");



    if(ImGui::Button("I have read and understood the text above.")){
        Lamp::Core::lampConfig::getInstance().ShowIntroMenu = false;
        Lamp::Core::lampFilesystem::getInstance().save_config();
    }
    ImGui::End();
}

void Lamp::Core::lampMenus::GameSelect() {
    ImGui::Begin("Blank Menu", NULL, DefaultFlags());
    DefaultMenuBar();
    ImGui::Text("Welcome to Lamp! At the moment there is only support for BG3, hopefully more to come!");
    if(ImGui::Button(lampConfig::getInstance().GameStringMap[lampConfig::BG3].c_str())){
        lampConfig::getInstance().CurrentGame = lampConfig::BG3;
    }
    ImGui::End();
}


void Lamp::Core::lampMenus::ModMenu() {
    ImGui::Begin("Blank Menu", NULL, DefaultFlags());
    DefaultMenuBar();

    ImGuiStyle& style = ImGui::GetStyle();
    float size = ImGui::CalcTextSize("Drag archives onto this window to start adding mods to your game!").x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("Drag archives onto this window to start adding mods to your game!");

    size = ImGui::CalcTextSize("Deploy").x + style.FramePadding.x * 2.0f;
    avail = ImGui::GetContentRegionAvail().x;

    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    if(ImGui::Button("Deploy")) {
        deployCheck = true;
    }

    if(deployCheck) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Checks", NULL, DefaultFlags());
        ImGui::Text("Start Deployment?");

        if(ImGui::Button("Start")){
            deployCheck = !deployCheck;
            switch (lampConfig::getInstance().CurrentGame) {
                case lampConfig::BG3:
                    Lamp::Game::BG3::getInstance().startDeployment();
                    break;
            }

        }
        ImGui::SameLine();
        if(ImGui::Button("Go Back")){ deployCheck = !deployCheck; }

        ImGui::End();
    }

    ImGui::Separator();
    switch (lampConfig::getInstance().CurrentGame) {
        case lampConfig::UNK:
            currentMenu = LAMP_GAME_SELECT;
            break;
        case lampConfig::BG3:
            Lamp::Game::BG3::getInstance().listArchives();
            break;
    }
    ImGui::End();
}

void Lamp::Core::lampMenus::GameConfigMenu() {
    ImGui::Begin("GameConfig", NULL, DefaultFlags());
    DefaultMenuBar();
    switch (lampConfig::getInstance().CurrentGame) {
        case lampConfig::UNK:
            currentMenu = LAMP_GAME_SELECT;
            break;
        case lampConfig::BG3:
            if(Lamp::Game::BG3::getInstance().ConfigMenu()){
                currentMenu = GAME_MOD_MENU;
            }
            break;
    }
    ImGui::End();
}

ImGuiWindowFlags Lamp::Core::lampMenus::DefaultFlags() {
    ImGuiWindowFlags windowFlags = 0;
    windowFlags += ImGuiWindowFlags_NoMove;
    windowFlags += ImGuiWindowFlags_NoResize;
    windowFlags += ImGuiWindowFlags_NoCollapse;
    windowFlags += ImGuiWindowFlags_NoTitleBar;
    windowFlags += ImGuiWindowFlags_MenuBar;
    return windowFlags;
}

void Lamp::Core::lampMenus::DefaultMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game Selection")) {
            Lamp::Core::lampConfig::getInstance().CurrentGame = Lamp::Core::lampConfig::UNK;
            currentMenu = LAMP_GAME_SELECT;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game Configuration")) {
            currentMenu = GAME_CONFIG_MENU;
            ImGui::EndMenu();
        }


        if (ImGui::BeginMenu("About")) {
            ImGui::MenuItem("Created by Charles Hollingworth");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}


