//
// Created by charles on 14/10/23.
//

#include "C77.h"
#include "../../Lamp/Control/lampControl.h"
namespace Lamp {
    namespace Game {
        lampReturn C77::registerArchive(lampString Path, int ArchiveModType) {

            for (Core::Base::lampMod::Mod* it : ModList) {

                std::filesystem::path NewFilePath = Path;
                std::filesystem::path TestingAgainstPath = it->ArchivePath;


                std::string NewFilePathCut = NewFilePath.filename();
                /*
                size_t posA = NewFilePathCut.find('-');
                if (posA != std::string::npos) {
                    NewFilePathCut.erase(posA);
                }
                */

                std::string TestingAgainstPathCut = TestingAgainstPath.filename();
                size_t posB = TestingAgainstPathCut.find('/');
                if (posB != std::string::npos) {
                    TestingAgainstPathCut.erase(posB);
                }


                if(NewFilePathCut == TestingAgainstPathCut){

                    it->timeOfUpdate = Lamp::Core::lampControl::getFormattedTimeAndDate();
                    it->ArchivePath = Path;
                    return Lamp::Core::FS::lampIO::saveModList(Ident().ShortHand,ModList,Games::getInstance().currentProfile);
                }


            }

            Core::Base::lampMod::Mod  * newArchive = new Core::Base::lampMod::Mod{Path,ModType::C77_MOD, false};
            newArchive->timeOfUpdate = Lamp::Core::lampControl::getFormattedTimeAndDate();
            ModList.push_back(newArchive);
            return Lamp::Core::FS::lampIO::saveModList(Ident().ShortHand,ModList,Games::getInstance().currentProfile);
        }

        lampReturn C77::ConfigMenu() {
            ImGui::Separator();
            Lamp::Core::lampControl::lampGameSettingsDisplayHelper("Cyberpunk 2077 Steam Directory", keyInfo["installPath"],
                                                                   "This is usually (steampath)/steamapps/common/Cyberpunk 2077/",
                                                                   "installPath").createImGuiMenu();
            return false;
        }

        lampReturn C77::startDeployment() {
            Lamp::Core::lampControl::getInstance().inDeployment = true;
            if(KeyInfo()["installPath"] == "" ) {
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

        lampReturn C77::preCleanUp() {
            std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Ident().ReadableName;
            Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,2};

            std::filesystem::path dir(workingDir);
            Core::Base::lampLog::getInstance().log("Cleaning Working Directory : "+workingDir, Core::Base::lampLog::warningLevel::LOG);
            try {
                for (const auto &entry: std::filesystem::directory_iterator(dir)) {
                    std::filesystem::remove_all(entry.path());
                }
            }catch(std::exception e){
                return {-1, "Cannot clean working directories"};
            }
            Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 1;


            Core::Base::lampLog::getInstance().log("Creating Working Directories", Core::Base::lampLog::warningLevel::LOG);
            try {
                std::filesystem::create_directories(workingDir + "/Cyberpunk 2777/archive/pc/mod/");
            }catch(std::exception e){
                return {-1, "Unable to create working directories."};
            }
            Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 2;
            return {1 , ""};
        }

        lampReturn C77::preDeployment() {
            Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,0};
            Core::Base::lampLog::getInstance().log("Extracting Archives", Core::Base::lampLog::warningLevel::LOG);
            auto lambdaFunction = [](const Core::Base::lampMod::Mod* item) {
                if(item->enabled && item->modType != MOD_SEPARATOR) {
                    Lamp::Core::lampControl::getInstance().deplopmentTracker.second++;
                    if(Lamp::Core::FS::lampExtract::extract(item)) {

                        std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Lamp::Games::getInstance().currentGame->Ident().ReadableName;
                        std::filesystem::path dirit(workingDir + "/ext/" + std::filesystem::path(item->ArchivePath).filename().stem().string());

                        std::string x = item->ArchivePath;
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"archive","Cyberpunk 2777/archive");
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"red4ext","Cyberpunk 2777/red4ext");
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"r6","Cyberpunk 2777/r6");
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"bin","Cyberpunk 2777/bin");
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"engine","Cyberpunk 2777/engine");
                        Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"REDMod","Cyberpunk 2777/REDMod");

                        Lamp::Core::lampControl::getInstance().deplopmentTracker.first++;
                    }
                }
            };
            lampExecutor executor(lambdaFunction);
            executor.execute(ModList, false);
            return {1,"TESTER"};
        }

        lampReturn C77::deployment() {
            std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Ident().ReadableName;
            Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,1};

                Lamp::Core::FS::lampTrack::handleFileDescriptor A{
                        Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyFolder,
                        Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
                        workingDir+"/Cyberpunk 2777",
                        keyInfo["installPath"],
                        "",
                        Ident().ReadableName
                };
                Lamp::Core::FS::lampTrack::handleFile(A);
                Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 1;
            return Lamp::Game::lampReturn();

        }

        lampReturn C77::postDeploymentTasks() {
            return Lamp::Game::lampReturn();
        }

        void C77::listArchives() {
            Lamp::Core::lampControl::lampArchiveDisplayHelper(
                    std::list<std::string>{},
                    ModList,
                    std::list<std::pair<std::string, bool *>>{}
            ).createImguiMenu();
        }


    } // Lamp
} // Game