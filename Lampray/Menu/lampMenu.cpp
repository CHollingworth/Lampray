//
// Created by charles on 27/09/23.
//
#include "lampMenu.h"
#include "lampCustomise .h"
#include "../Lang/lampLang.h"
#include <cstdlib>
#include "../Control/lampNotification.h"

void Lamp::Core::lampMenu::RunMenus() {



    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize, 0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if(Lamp::Core::lampControl::getInstance().inDeployment){
        ImGui::Begin("DEPLOYMENT", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
        ImGui::Text("%s", Lamp::Core::lampControl::getInstance().deploymentStageTitle.c_str());
        ImGui::Text("%s", (std::to_string(Lamp::Core::lampControl::getInstance().deplopmentTracker.first) + "/" + std::to_string(Lamp::Core::lampControl::getInstance().deplopmentTracker.second)).c_str());
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
                if(Lamp::Core::lampCustomise::displayMenu()){
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

    size = ImGui::CalcTextSize(lampLang::LS("LAMPRAY_LONGNAME")).x + style.FramePadding.x * 2.0f;
    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("%s", lampLang::LS("LAMPRAY_LONGNAME").c_str());
    ImGui::Separator();
    ImGui::Text("%s", lampLang::LS("LAMPRAY_LICENSE").c_str());



    if(ImGui::Button(lampLang::LS("LAMPRAY_AKNOWL"))){
        Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"] = "0";
        Lamp::Core::FS::lampIO::saveKeyData("showIntroMenu", Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"], "LAMP CONFIG");
        currentMenu = GAME_MOD_MENU;
    }
    ImGui::End();
}


void Lamp::Core::lampMenu::ModMenu() {

    ImGui::Begin("Blank Menu", NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
    DefaultMenuBar();

    Lamp::Core::lampNotification::getInstance().DisplayNotifications();

    ImGuiStyle& style = ImGui::GetStyle();
    float size = ImGui::CalcTextSize(lampLang::LS("LAMPRAY_DRAGANDDROP")).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    ImGui::Text("%s", lampLang::LS("LAMPRAY_DRAGANDDROP").c_str());

    size = ImGui::CalcTextSize(lampLang::LS("LAMPRAY_DEPLOY")).x + style.FramePadding.x * 2.0f;
    avail = ImGui::GetContentRegionAvail().x;

    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    if(ImGui::Button(lampLang::LS("LAMPRAY_DEPLOY"))) {
        deployCheck = true;
    }

    size = ImGui::CalcTextSize(lampLang::LS("LAMPRAY_RESET")).x + style.FramePadding.x * 2.0f;
    avail = ImGui::GetContentRegionAvail().x;

    off = (avail - size) * 0.5f;
    if (off > 0.0f){ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);}
    if(ImGui::Button(lampLang::LS("LAMPRAY_RESET"))) {
        Lamp::Core::FS::lampTrack::reset(Lamp::Games::getInstance().currentGame->Ident().ReadableName);
        std::filesystem::path installPath(Lamp::Games::getInstance().currentGame->KeyInfo()["installDirPath"]);
        system(("pkexec umount \""+Lamp::Games::getInstance().currentGame->KeyInfo()["installDirPath"]+"\"").c_str());
        std::filesystem::rename(installPath.parent_path() / ("Lampray Managed - " + installPath.stem().string()), Lamp::Games::getInstance().currentGame->KeyInfo()["installDirPath"]);
        system(("pkexec umount \""+Lamp::Games::getInstance().currentGame->KeyInfo()["appDataPath"]+"/Mods\"").c_str());
        std::filesystem::rename(std::filesystem::path(Lamp::Games::getInstance().currentGame->KeyInfo()["appDataPath"]+"/Mods").parent_path() / ("Lampray Managed - " + std::filesystem::path(Lamp::Games::getInstance().currentGame->KeyInfo()["appDataPath"]+"/Mods").stem().string()), std::filesystem::path(Lamp::Games::getInstance().currentGame->KeyInfo()["appDataPath"]+"/Mods"));
    }


    if(deployCheck) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin(lampLang::LS("LAMPRAY_CHECK"), NULL, Lamp::Core::lampConfig::getInstance().DefaultWindowFlags());
        ImGui::Text("%s", lampLang::LS("LAMPRAY_STARTTEXT").c_str());

        if(ImGui::Button(lampLang::LS("LAMPRAY_START"))){
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
        if(ImGui::Button(lampLang::LS("LAMPRAY_GOBACK"))){ deployCheck = !deployCheck; }

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
    if(ImGui::Button(lampLang::LS("LAMPRAY_GOBACK"))){
        currentMenu = GAME_MOD_MENU;
    }


    ImGui::End();
}

void Lamp::Core::lampMenu::DefaultMenuBar() {
    if (ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("Lampray - " + Lamp::Games::getInstance().currentGame->Ident().ReadableName +" - "+Lamp::Games::getInstance().currentProfile)){

            if (ImGui::BeginMenu(lampLang::LS("LAMPRAY_GAMEPICK"))) {
                int gameCount = 0;
                for (Game::gameControl *element: Lamp::Games::getInstance().gameList){
                    if(ImGui::MenuItem(element->Ident().ReadableName)){
                        Lamp::Games::getInstance().currentGame = element;
                        Lamp::Games::getInstance().currentGameInt = gameCount;
                        Lamp::Core::FS::lampIO::saveKeyData("PreviousGame",std::to_string(gameCount), "LAMP CONFIG");
                    }
                    gameCount++;
                }
                ImGui::EndMenu();
            }

            ImGui::MenuItem("--------");
            if(currentMenu == GAME_MOD_MENU){
            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_GAMECONFIG"))) {
                currentMenu = GAME_CONFIG_MENU;
            }

            if(ImGui::BeginMenu(lampLang::LS("LAMPRAY_PROFSELECT"))){

                for (std::string x: Lamp::Core::Base::lampMod::Profile::splitString(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"])) {

                    if (x == Lamp::Games::getInstance().currentProfile) {
                        if (ImGui::BeginMenu(("* "+x+" *").c_str())) {
                            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFLOAD"))) {
                                Lamp::Games::getInstance().currentProfile = x;
                                Lamp::Games::getInstance().currentGame->getModList().clear();
                                Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                                        Lamp::Games::getInstance().currentGame->Ident().ShortHand, x);
                                FS::lampIO::saveKeyData("CurrentProfile", x, Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                            }
                            if (x != "Default") {
                                if (ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFDEL"))) {

                                }
                            }
                            ImGui::EndMenu();
                        }
                    }else{
                        if (ImGui::BeginMenu(x.c_str())) {
                            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFLOAD"))) {
                                Lamp::Games::getInstance().currentProfile = x;
                                Lamp::Games::getInstance().currentGame->getModList().clear();
                                Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                                        Lamp::Games::getInstance().currentGame->Ident().ShortHand, x);
                                FS::lampIO::saveKeyData("CurrentProfile", x, Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                            }
                            if (x != "Default") {
                                if (ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFDEL"))) {
                                    Lamp::Core::Base::lampMod::Profile::removeValue(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"],x);
                                    FS::lampIO::saveKeyData("ProfileList", Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"], Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }



                }
                ImGui::MenuItem("--------");


                if(ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFCRE"))){
                    displayProfileCreateMenu = true;
                }
                ImGui::EndMenu();
            }



            ImGui::MenuItem("--------");

            if (ImGui::BeginMenu(lampLang::LS("LAMPRAY_PROFEXP"))) {
                for (std::string x: Lamp::Core::Base::lampMod::Profile::splitString(
                        Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"])) {
                        if (ImGui::MenuItem((x).c_str())) {
                            Lamp::Core::FS::lampShare::exportProfile(x);
                        }
                }
                ImGui::EndMenu();
            }


            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFIMP"))) {
                Lamp::Core::FS::lampShare::importProfile();
            }



            ImGui::MenuItem("--------");
            }



            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_UPDATE_CHECK"))) {
                Lamp::Core::FS::lampUpdate::getInstance().checkForUpdates();
            }

            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_CUSTOM"))) {
                currentMenu = CUSTOMIZE;
            }


            if (ImGui::BeginMenu(lampLang::LS("LAMPRAY_ABT"))) {
                ImGui::MenuItem("Lampray (Linux Application Modding Platform)");
                ImGui::MenuItem("--------------------------------------");
                ImGui::MenuItem(lampLang::LS("LAMPRAY_ABT1")+" Charles Hollingworth");
                ImGui::MenuItem("-------------Contributors-------------");
                ImGui::MenuItem("Jinxtaposition");
                ImGui::MenuItem("Airtonix");
                ImGui::MenuItem("SnazzyPanda");
                ImGui::MenuItem("-------------Donators-------------");
                ImGui::MenuItem("Plarpoon");
                ImGui::EndMenu();
            }
            ImGui::MenuItem("--------");
            if (ImGui::MenuItem(lampLang::LS("LAMPRAY_QUIT"))) {
                this->userRequestedQuit = true;
            }


            ImGui::EndMenu();
        }


        Lamp::Core::FS::lampUpdate::getInstance().getExpression();
        ImGui::EndMenuBar();
}
}

void Lamp::Core::lampMenu::createProfileDialog() {
        if(!displayProfileCreateMenu) return;
        ImGui::Begin(lampLang::LS("LAMPRAY_PROFCRE"));

        if(ImGui::BeginMenu(lampLang::LS("LAMPRAY_PROFFRM"))){

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
            if(ImGui::MenuItem(lampLang::LS("LAMPRAY_PROFNON"))){
                pfCopyStr = "";
                pfCopy = false;
            }
            ImGui::EndMenu();
        }


        ImGui::Text("%s", lampLang::LS("LAMPRAY_PROFNM").c_str());
        if(ImGui::InputText("##inputProf",profileBuffer,250, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button(lampLang::LS("LAMPRAY_PROFCRES"))){
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
    if(ImGui::Button(lampLang::LS("LAMPRAY_GOBACK"))){
        displayProfileCreateMenu = false;
    }
        ImGui::End();
}
