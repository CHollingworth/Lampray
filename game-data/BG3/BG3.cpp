//
// Created by charles on 27/09/23.
//

#include <regex>
#include "BG3.h"
#include "../../Lamp/Control/lampControl.h"
#include "../../third-party/json/json.hpp"

Lamp::Game::lampReturn Lamp::Game::BG3::registerArchive(Lamp::Game::lampString Path) {
    Core::Base::lampMod::Mod  * newArchive = new Core::Base::lampMod::Mod{Path,ModType::NaN, false};
    newArchive->timeOfUpdate = Lamp::Core::lampControl::getFormattedTimeAndDate();
    ModList.push_back(newArchive);
    return Lamp::Core::FS::lampIO::saveModList(Ident().ShortHand,ModList,Games::getInstance().currentProfile);
}

Lamp::Game::lampReturn Lamp::Game::BG3::ConfigMenu() {
    ImGui::Separator();
    Lamp::Core::lampControl::lampGameSettingsDisplayHelper("BG3 Steam Directory", keyInfo["installDirPath"],
                                                           "This is usually (steampath)/steamapps/common/Baldurs Gate 3",
                                                            "installDirPath").createImGuiMenu();
    ImGui::Separator();
    Lamp::Core::lampControl::lampGameSettingsDisplayHelper("BG3 AppData Directory", keyInfo["appDataPath"],
                                                           "This is usually (steampath)/steamapps/compatdata/1086940/pfx/drive_c/users/steamuser/AppData/Local/Larian Studios/Baldur's Gate 3",
                                                           "appDataPath").createImGuiMenu();
    return false;
}

Lamp::Game::lampReturn Lamp::Game::BG3::startDeployment() {
    Lamp::Core::lampControl::getInstance().inDeployment = true;
    if(KeyInfo()["installDirPath"] == "" | KeyInfo()["appDataPath"] == "" ) {
        return Core::Base::lampLog::getInstance().pLog({0, "Game Configuration not set."}, Core::Base::lampLog::warningLevel::WARNING, true, Core::Base::lampLog::LMP_NOCONFIG);
    }

    Core::Base::LampSequencer::add("BG3 Deployment Queue", [this]() -> lampReturn {
        Lamp::Core::lampControl::getInstance().deploymentStageTitle = "Preparing";
        auto result = preCleanUp();
        if(!result) Core::Base::lampLog::getInstance().log("Pre cleanup has failed.", Core::Base::lampLog::warningLevel::ERROR, true, Core::Base::lampLog::LMP_CLEANUPFAILED);
        return result;
    });

    Core::Base::LampSequencer::add("BG3 Deployment Queue", [this]() -> lampReturn {
        Lamp::Core::lampControl::getInstance().deploymentStageTitle = "Pre Deployment";
        auto result = preDeployment();
        if(!result) Core::Base::lampLog::getInstance().log("Pre Deployment has failed.", Core::Base::lampLog::warningLevel::ERROR, true, Core::Base::lampLog::LMP_PREDEPLOYFAILED);
        return result;
    });

    Core::Base::LampSequencer::add("BG3 Deployment Queue", [this]() -> lampReturn {
        Lamp::Core::lampControl::getInstance().deploymentStageTitle = "Deploying";
        auto result = deployment();
        if(!result) Core::Base::lampLog::getInstance().log("Deployment has failed.", Core::Base::lampLog::warningLevel::ERROR, true, Core::Base::lampLog::LMP_DEOPLYMENTFAILED);
        return result;
    });


    Core::Base::LampSequencer::run("BG3 Deployment Queue");
    Lamp::Core::lampControl::getInstance().inDeployment = false;
    return false;
}

Lamp::Game::lampReturn Lamp::Game::BG3::preCleanUp() {

    return {1, "PreCleanup Finished."};
}

Lamp::Game::lampReturn Lamp::Game::BG3::preDeployment() {
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,0};

    Core::Base::LampSequencer::run("BG3 Settings Control");
    return {1,"TESTER"};
}

Lamp::Game::lampReturn Lamp::Game::BG3::deployment() {
    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Ident().ReadableName;
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,5};

    return {1, "Deployed"};
}

Lamp::Game::lampReturn Lamp::Game::BG3::postDeploymentTasks() {

}

void Lamp::Game::BG3::listArchives() {
    Lamp::Core::lampControl::lampArchiveDisplayHelper(
            std::list<std::string>{},
            ModList,
            std::vector<std::string>{"Engine Injection", "Standard Mod", "Bin Overwrite","Data Overwrite","No Json Mod", "Select Type"},
            std::list<std::pair<std::string, bool *>>{}
    ).createImguiMenu();
}

std::vector<Lamp::Core::Base::lampMod::Mod *> &Lamp::Game::BG3::getModList() {
    return ModList;
}



