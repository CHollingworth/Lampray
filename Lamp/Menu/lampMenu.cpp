//
// Created by charles on 27/09/23.
//
#include "lampMenu.h"
#include "lampColour.h"

void Lamp::Core::lampMenu::RunMenus() {



    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize, 0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if(Lamp::Core::lampControl::getInstance().inDeployment){
        ImGui::Begin("DEPLOYMENT", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
        ImGui::Text(Lamp::Core::lampControl::getInstance().deploymentStageTitle.c_str());
        ImGui::Text((std::to_string(Lamp::Core::lampControl::getInstance().deplopmentTracker.first) + "/" + std::to_string(Lamp::Core::lampControl::getInstance().deplopmentTracker.second)).c_str());
        ImGui::End();
    }else {
        switch (currentMenu) {
            case LICENCE_MENU:
                if (Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"].as_bool()) {
                    IntroMenu();
                } else {
                    currentMenu = GAME_MOD_MENU;
                }
                break;
            case GAME_MOD_MENU:
                ModMenu();
                break;
            case GAME_CONFIG_MENU:
                GameConfigMenu();
                break;
            case CUSTOMIZE:
                ImGuiWindowFlags windowFlags = 0;
                windowFlags += ImGuiWindowFlags_NoMove;
                windowFlags += ImGuiWindowFlags_NoResize;
                windowFlags += ImGuiWindowFlags_NoCollapse;
                windowFlags += ImGuiWindowFlags_NoTitleBar;
                windowFlags += ImGuiWindowFlags_MenuBar;
                ImGui::Begin("Colour Menu",NULL,windowFlags);
                DefaultMenuBar();
                if(Lamp::Core::lampColour::displayColourMenu()){
                    currentMenu = GAME_MOD_MENU;
                }
                break;
        }
        //currentMenu = LICENCE_MENU;
        createProfileDialog();
    }
}

void Lamp::Core::lampMenu::IntroMenu() {

    ImGui::Begin("Requires a title, but we've disabled it.", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
    DefaultMenuBar();
    ImGui::Separator();
    ImGuiStyle& style = ImGui::GetStyle();
    float size = ImGui::CalcTextSize("Lampray").x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("Lampray");

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
        Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"] = "0";
        Lamp::Core::FS::lampIO::saveKeyData("showIntroMenu", Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"], "LAMP CONFIG");
        currentMenu = GAME_MOD_MENU;
    }
    ImGui::End();
}


void Lamp::Core::lampMenu::ModMenu() {

    ImGui::Begin("Blank Menu", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
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

    size = ImGui::CalcTextSize("Reset").x + style.FramePadding.x * 2.0f;
    avail = ImGui::GetContentRegionAvail().x;

    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    if(ImGui::Button("Reset")) {
        Lamp::Core::FS::lampTrack::reset(Lamp::Games::getInstance().currentGame->Ident().ReadableName);
    }


    if(deployCheck) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Checks", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
        ImGui::Text("Start Deployment?");

        if(ImGui::Button("Start")){
            deployCheck = !deployCheck;

            std::thread([] {
                Lamp::Core::Base::LampSequencer::add("Deployment Start",[]() -> lampReturn {
                    Lamp::Core::FS::lampTrack::reset(Lamp::Games::getInstance().currentGame->Ident().ReadableName);

                    return Lamp::Core::Base::lampLog::getInstance().pLog({1, "Task Complete."});
                });
                Lamp::Core::Base::LampSequencer::add("Deployment Start",[]() -> lampReturn {
                    Lamp::Games::getInstance().currentGame->startDeployment();
                    return Lamp::Core::Base::lampLog::getInstance().pLog({1, "Task Complete."});
                });

                Lamp::Core::Base::LampSequencer::run("Deployment Start");
            }).detach();

        }
        ImGui::SameLine();
        if(ImGui::Button("Go Back")){ deployCheck = !deployCheck; }

        ImGui::End();
    }

    ImGui::Separator();
    Lamp::Games::getInstance().currentGame->listArchives();
    ImGui::End();
}

void Lamp::Core::lampMenu::GameConfigMenu() {

    ImGuiWindowFlags windowFlags = 0;
    windowFlags += ImGuiWindowFlags_NoMove;
    windowFlags += ImGuiWindowFlags_NoResize;
    windowFlags += ImGuiWindowFlags_NoCollapse;
    windowFlags += ImGuiWindowFlags_NoTitleBar;
    windowFlags += ImGuiWindowFlags_MenuBar;
    ImGui::Begin("testConfig",NULL,windowFlags);
    Lamp::Games::getInstance().currentGame->ConfigMenu();
    ImGui::Separator();
    if(ImGui::Button("Back")){
        currentMenu = GAME_MOD_MENU;
    }


    ImGui::End();
}

void Lamp::Core::lampMenu::DefaultMenuBar() {
    if (ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("Lampray - " + Lamp::Games::getInstance().currentGame->Ident().ReadableName +" - "+Lamp::Games::getInstance().currentProfile)){

            if (ImGui::BeginMenu("Game Selection")) {
                for (Game::gameControl *element: Lamp::Games::getInstance().gameList){
                    if(ImGui::MenuItem(element->Ident().ReadableName)){
                        Lamp::Games::getInstance().currentGame = element;
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::MenuItem("--------");
            if(currentMenu == GAME_MOD_MENU){
            if (ImGui::MenuItem("Game Configuration")) {
                currentMenu = GAME_CONFIG_MENU;
            }

            if(ImGui::BeginMenu("Profile Selection")){

                for (std::string x: Lamp::Core::Base::lampMod::Profile::splitString(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"])) {

                    if (x == Lamp::Games::getInstance().currentProfile) {
                        if (ImGui::BeginMenu(("* "+x+" *").c_str())) {
                            if (ImGui::MenuItem("Load Profile")) {
                                Lamp::Games::getInstance().currentProfile = x;
                                Lamp::Games::getInstance().currentGame->getModList().clear();
                                Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                                        Lamp::Games::getInstance().currentGame->Ident().ShortHand, x);
                                FS::lampIO::saveKeyData("CurrentProfile", x, Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                            }
                            if (x != "Default") {
                                if (ImGui::MenuItem("Delete Profile")) {

                                }
                            }
                            ImGui::EndMenu();
                        }
                    }else{
                        if (ImGui::BeginMenu(x.c_str())) {
                            if (ImGui::MenuItem("Load Profile")) {
                                Lamp::Games::getInstance().currentProfile = x;
                                Lamp::Games::getInstance().currentGame->getModList().clear();
                                Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                                        Lamp::Games::getInstance().currentGame->Ident().ShortHand, x);
                                FS::lampIO::saveKeyData("CurrentProfile", x, Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                            }
                            if (x != "Default") {
                                if (ImGui::MenuItem("Delete Profile")) {
                                    Lamp::Core::Base::lampMod::Profile::removeValue(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"],x);
                                    FS::lampIO::saveKeyData("ProfileList", Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"], Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }



                }
                ImGui::MenuItem("--------");


                if(ImGui::MenuItem("Create New Profile")){
                    displayProfileCreateMenu = true;
                }
                ImGui::EndMenu();
            }



            ImGui::MenuItem("--------");

            if (ImGui::BeginMenu("Export Profile")) {
                for (std::string x: Lamp::Core::Base::lampMod::Profile::splitString(
                        Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"])) {
                        if (ImGui::MenuItem((x).c_str())) {
                            Lamp::Core::FS::lampShare::exportProfile(x);
                        }
                }
                ImGui::EndMenu();
            }


            if (ImGui::MenuItem("Import Profile")) {
                Lamp::Core::FS::lampShare::importProfile();
            }



            ImGui::MenuItem("--------");
            }



            if (ImGui::MenuItem("Check for Updates")) {
                Lamp::Core::FS::lampUpdate::getInstance().checkForUpdates();
            }

            if (ImGui::MenuItem("Customise Lampray")) {
                currentMenu = CUSTOMIZE;
            }


            if (ImGui::BeginMenu("About")) {
                ImGui::MenuItem("Created by Charles Hollingworth");
                ImGui::EndMenu();
            }



            ImGui::EndMenu();
        }










        Lamp::Core::FS::lampUpdate::getInstance().getExpression();
        ImGui::EndMenuBar();
}
}

void Lamp::Core::lampMenu::createProfileDialog() {
        if(!displayProfileCreateMenu) return;
        ImGui::Begin("Create Profile");

        if(ImGui::BeginMenu("Copy From")){

            for (std::string x: Lamp::Core::Base::lampMod::Profile::splitString(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"])) {
                if(pfCopyStr == x){
                    if (ImGui::MenuItem(("*" + x + "*").c_str())) {
                        pfCopyStr = x;
                        pfCopy = true;
                    }
                }else {
                    if (ImGui::MenuItem(x.c_str())) {
                        pfCopyStr = x;
                        pfCopy = true;
                    }
                }
            }
            ImGui::MenuItem("--------");
            if(ImGui::MenuItem("None")){
                pfCopyStr = "";
                pfCopy = false;
            }
            ImGui::EndMenu();
        }


        ImGui::Text("New Profile Name");
        if(ImGui::InputText("##inputProf",profileBuffer,250, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Create")){
            std::string pb(profileBuffer);
            Lamp::Core::Base::lampMod::Profile::addValue(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"],pb);

            if(pfCopyStr != "" && pfCopy){
                if(Lamp::Games::getInstance().currentProfile != pfCopyStr) {
                    Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                            Lamp::Games::getInstance().currentGame->Ident().ShortHand, pfCopyStr);
                }
                FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand,Lamp::Games::getInstance().currentGame->getModList(),pb);
            }
            Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand,pb);
            Lamp::Games::getInstance().currentProfile = pb;
            FS::lampIO::saveKeyData("ProfileList", Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"], Lamp::Games::getInstance().currentGame->Ident().ShortHand);
            memset(profileBuffer, 0, sizeof(profileBuffer));
            pfCopyStr = "";
            pfCopy = false;
            displayProfileCreateMenu = false;
        }
        ImGui::SameLine();
    if(ImGui::Button("Close")){
        displayProfileCreateMenu = false;
    }
        ImGui::End();
}
