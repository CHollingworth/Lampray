//
// Created by charles on 13/09/23.
//

#include <regex>
#include "FO4.h"
#include "../../lampWarn.h"
#include "../../lampConfig.h"
#include "../../lampFilesystem.h"

namespace Lamp {
    namespace Game {
        void FO4::registerArchive(std::string Path) {

            Lamp::Core::lampMod::Mod  * newArchive = new Lamp::Core::lampMod::Mod{Path,ModType::NaN, false};

            std::filesystem::path file_path(Path);
            std::string newfolder = file_path.filename();
            size_t dotPos = newfolder.find_last_of('.');
            if (dotPos != std::string::npos) {
                newfolder.erase(dotPos);
            }
            Lamp::Core::lampWarn::getInstance().log("Extracting Archive " + Path);
            if (std::regex_match(newArchive->ArchivePath, std::regex("^.*\\.(zip)$"))) {
                Lamp::Core::lampFilesystem::getInstance().extract(Core::lampConfig::FO4, bit7z::BitFormat::Zip, newArchive,"/"+newfolder);
            } else if (std::regex_match(newArchive->ArchivePath, std::regex("^.*\\.(rar)$"))) {
                Lamp::Core::lampFilesystem::getInstance().extract(Core::lampConfig::FO4, bit7z::BitFormat::Rar, newArchive,"/"+newfolder);
            } else if (std::regex_match(newArchive->ArchivePath, std::regex("^.*\\.(7z)$"))) {
                Lamp::Core::lampFilesystem::getInstance().extract(Core::lampConfig::FO4, bit7z::BitFormat::SevenZip, newArchive,"/"+newfolder);
            }

            Lamp::Core::lampWarn::getInstance().log("Checking for subfolder packing...");
            bool containsOnlyOneSubdirectory = false;
            fs::directory_entry subdirectoryPath;
            for (const auto& entry : fs::directory_iterator(Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(Core::lampConfig::FO4)+"/"+newfolder)) {
                if (fs::is_directory(entry)) {
                    if (!containsOnlyOneSubdirectory) {
                        containsOnlyOneSubdirectory = true;
                        subdirectoryPath = entry;
                    } else {
                        containsOnlyOneSubdirectory = false;
                        break;
                    }
                }
            }

            if (containsOnlyOneSubdirectory) {

                try {
                    for (const auto& entry : fs::directory_iterator(subdirectoryPath)) {
                        std::string newFilePath = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(Core::lampConfig::FO4)+"/"+newfolder + "/" + entry.path().filename().string();
                        fs::rename(entry.path(), newFilePath);
                    }
                    fs::remove(subdirectoryPath);
                    Lamp::Core::lampWarn::getInstance().log("Subfolder packing found & unpacked.");
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }


            }
            Lamp::Core::lampWarn::getInstance().log("Checking for FOMod");
            std::string directoryPath = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(Core::lampConfig::FO4)+"/"+newfolder+"/FOMod";

            if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
                std::cout << "The directory exists." << std::endl;
                newArchive->modType = F04_WITH_FOMOD;
            } else {
                Lamp::Core::lampWarn::getInstance().log("Checking for fomod");
                directoryPath = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(Core::lampConfig::FO4)+"/"+newfolder+"/fomod";
                if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
                    std::cout << "The directory exists." << std::endl;
                    newArchive->modType = F04_WITH_FOMOD;
                }else{
                    newArchive->modType = F04_NO_FOMOD;
                }

            }

            ModList.push_back(newArchive);
            Lamp::Core::lampFilesystem::getInstance().saveModList(Core::lampConfig::FO4,ModList);
        }

        bool FO4::ConfigMenu() {
            return false;
        }

        bool FO4::createFileStructure() {
            return false;
        }

        bool FO4::startDeployment() {
            if(appDataPath == "" || appDataPath == " "){
                return false;
            }
            if(installDirPath == "" || installDirPath == " "){
                return false;
            }

            if(preCleanUp()){
                if(preDeployment()){
                    if(deployment()){
                        Lamp::Core::lampWarn::getInstance().log("Deployment Successful!", Core::lampWarn::LOG, true);
                        postDeploymentTasks();
                        return true;
                    }
                    Lamp::Core::lampWarn::getInstance().log("Deployment failed! Aborting Deployment", Core::lampWarn::ERROR, true, Core::lampWarn::LMP_DEOPLYMENTFAILED);
                    return false;
                }
                Lamp::Core::lampWarn::getInstance().log("PreDeployment failed! Aborting Deployment", Core::lampWarn::ERROR, true, Core::lampWarn::LMP_PREDEPLOYFAILED);
                return false;
            }
            Lamp::Core::lampWarn::getInstance().log("Cleanup failed! Aborting Deployment", Core::lampWarn::ERROR, true, Core::lampWarn::LMP_CLEANUPFAILED);
            return false;
        }

        bool FO4::preCleanUp() {
            return false;
        }

        bool FO4::preDeployment() {
            return false;
        }

        bool FO4::deployment() {
            return false;
        }

        void FO4::postDeploymentTasks() {

        }

        void FO4::listArchives() {

            if(ImGui::BeginTable("Split", 6, ImGuiTableFlags_SizingFixedFit)) {

                ImGui::TableNextColumn();
                ImGui::Text("Enabled");
                ImGui::TableNextColumn();
                ImGui::Text("Mod Name");
                ImGui::TableNextColumn();
                ImGui::Text("Mod Type");
                ImGui::TableNextColumn();
                ImGui::Text("Mod Order");
                ImGui::TableNextColumn();
                ImGui::Text("Change Order");
                ImGui::TableNextColumn();
                ImGui::Text("Remove Mod");
                ImGui::TableNextRow();
                std::list<Lamp::Core::lampMod::Mod *>::iterator it;
                int i = 0;
                for (it = ModList.begin(); it != ModList.end(); ++it) {

                    std::filesystem::path path((*it)->ArchivePath);
                    ImGui::TableNextColumn();

                    if((*it)->enabled) {
                        if (ImGui::Button(("Enabled##" + std::to_string(i)).c_str())) {
                            (*it)->enabled = false;
                            Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                        }
                    }else{
                        if (ImGui::Button(("Disabled##" + std::to_string(i)).c_str())) {
                            (*it)->enabled = true;
                            Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                        }
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(path.filename().c_str());


                    ImGui::TableNextColumn();

                    std::string Type = "";

                    switch ((*it)->modType) {
                        case ModType::F04_NO_FOMOD:
                            Type = "Standard Mod";
                            break;
                        case ModType::F04_WITH_FOMOD:
                            Type = "FOMOD Installer";
                            break;
                        case ModType::F04_MAINFOLDER:
                            Type = "Main Folder Overwrite";
                            break;
                        case ModType::NaN:
                            Type = "Select Type";
                            break;
                    }


                    if (ImGui::BeginMenu((Type + "##" + std::to_string(i)).c_str())) {

                        if (ImGui::MenuItem("Standard Mod")) {
                            (*it)->modType = ModType::F04_NO_FOMOD;
                            Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                        }
                        if (ImGui::MenuItem("FOMOD Installer")) {
                            (*it)->modType = ModType::F04_WITH_FOMOD;
                            Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                        }
                        if (ImGui::MenuItem("Main Folder Overwrite")) {
                            (*it)->modType = ModType::F04_MAINFOLDER;
                            Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text((std::to_string(i)).c_str());
                    ImGui::TableNextColumn();
                    if(ImGui::Button(("Move Up##" + std::to_string(i)).c_str())){
                        auto prev = std::prev(it);
                        if (it != ModList.begin()) {
                            ModList.splice(prev, ModList, it);
                        }else{
                            auto last = std::prev(ModList.end());
                            ModList.splice(last, ModList, it);
                            it = last;
                        }
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::FO4, ModList);
                    }

                    ImGui::TableNextColumn();
                    if (ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())) {
                        std::remove(absolute(path).c_str());
                        std::cout << absolute(path).c_str() << std::endl;
                        ModList.remove((*it));
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Core::lampConfig::BG3,ModList);
                        break;
                    }

                    ImGui::TableNextRow();
                    i++;
                }
                ImGui::EndTable();
            }
        }
    } // Lamp
} // Game