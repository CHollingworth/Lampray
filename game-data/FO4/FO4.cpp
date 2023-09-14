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

            // okay we gotta force a modtype selection before running ANY of this fancy shit.
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
                newArchive->modType = BG3_WITH_FOMOD;
            } else {
                Lamp::Core::lampWarn::getInstance().log("Checking for fomod");
                directoryPath = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(Core::lampConfig::FO4)+"/"+newfolder+"/fomod";
                if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
                    std::cout << "The directory exists." << std::endl;
                    newArchive->modType = BG3_WITH_FOMOD;
                }else{
                    newArchive->modType = BG3_NO_FOMOD;
                }

            }

            ModList.push_back(newArchive);
            Lamp::Core::lampFilesystem::getInstance().saveModList(Core::lampConfig::BG3,ModList);
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

        }
    } // Lamp
} // Game