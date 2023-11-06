//
// Created by charles on 27/09/23.
//

#include <regex>
#include "BG3.h"
#include "../../Lamp/Control/lampControl.h"
#include "../../third-party/json/json.hpp"

Lamp::Game::lampReturn Lamp::Game::BG3::registerArchive(Lamp::Game::lampString Path) {

    for (Core::Base::lampMod::Mod* it : ModList) {

        std::filesystem::path NewFilePath = Path;
        std::filesystem::path TestingAgainstPath = it->ArchivePath;


        std::string NewFilePathCut = NewFilePath.filename();
        size_t posA = NewFilePathCut.find('-');
        if (posA != std::string::npos) {
            NewFilePathCut.erase(posA);
        }

        std::string TestingAgainstPathCut = TestingAgainstPath.filename();
        size_t posB = TestingAgainstPathCut.find('-');
        if (posB != std::string::npos) {
            TestingAgainstPathCut.erase(posB);
        }


        if(NewFilePathCut == TestingAgainstPathCut){

            it->timeOfUpdate = Lamp::Core::lampControl::getFormattedTimeAndDate();
            it->ArchivePath = Path;
            return Lamp::Core::FS::lampIO::saveModList(Ident().ShortHand,ModList,Games::getInstance().currentProfile);
        }


    }

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

    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Ident().ReadableName;
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,6};

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
    // Phantom Step.
    Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 2;
    Core::Base::lampLog::getInstance().log("Creating Working Directories", Core::Base::lampLog::warningLevel::LOG);
    try {
        std::filesystem::create_directories(workingDir + "/bin/NativeMods");
        std::filesystem::create_directories(workingDir + "/Data");
        std::filesystem::create_directories(workingDir + "/Mods");
        std::filesystem::create_directories(workingDir + "/PlayerProfiles/Public");
        std::filesystem::create_directories(workingDir + "/ext");
    }catch(std::exception e){
        return {-1, "Unable to create working directories."};
    }
    Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 3;
    Core::Base::lampLog::getInstance().log("Creating modsettings.lsx", Core::Base::lampLog::warningLevel::LOG);
    pugi::xml_document doc;



    pugi::xml_node save = doc.append_child("save");

    // Create the version node
    pugi::xml_node version = save.append_child("version");
    version.append_attribute("major").set_value(4);
    version.append_attribute("minor").set_value(2);
    version.append_attribute("revision").set_value(0);
    version.append_attribute("build").set_value(100);

    // Create the region node
    pugi::xml_node region = save.append_child("region");
    region.append_attribute("id").set_value("ModuleSettings");

    // Create the node node
    pugi::xml_node node = region.append_child("node");
    node.append_attribute("id").set_value("root");

    // Create the children node
    pugi::xml_node children = node.append_child("children");

    // Create the ModOrder node
    pugi::xml_node modOrder = children.append_child("node");
    modOrder.append_attribute("id").set_value("ModOrder");

    // Create the children node for ModOrder (empty in your example)
    pugi::xml_node modOrderChildren = modOrder.append_child("children");

    // Create the Mods node
    pugi::xml_node mods = children.append_child("node");
    mods.append_attribute("id").set_value("Mods");

    // Create the children node for Mods (empty in your example)
    pugi::xml_node modsChildren = mods.append_child("children");





    Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 4;
    Core::Base::lampLog::getInstance().log("Cleaning modsettings.lsx", Core::Base::lampLog::warningLevel::LOG);
    pugi::xml_node modOrderNode = doc.select_node("//node[@id='ModOrder']").node();
    pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();

    if (modOrderNode) {
        pugi::xml_node childrenNode = modOrderNode.child("children");
        if (childrenNode) {
            childrenNode.remove_children();
        }else{
            modOrderNode.append_child("children");
        }
    } else {
        return {-1, "ModOrder section not found."};
    }
    if (modsNode) {
        pugi::xml_node childrenNode = modsNode.child("children");
        if (childrenNode) {
            childrenNode.remove_children();
        }else{
            modsNode.append_child("children");
        }
    } else {
        return {-1, "Mods section not found."};
    }
    Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 5;
    Core::Base::lampLog::getInstance().log("Injecting GustavDev into modsettings.lsx", Core::Base::lampLog::warningLevel::LOG);
    pugi::xml_node modsNode2 = doc.select_node("//node[@id='Mods']").node();
    pugi::xml_node childrenNode = modsNode2.child("children");

    pugi::xml_node newShortDescNode = childrenNode.append_child("node");
    newShortDescNode.append_attribute("id") = "ModuleShortDesc";

    pugi::xml_node folderAttrib = newShortDescNode.append_child("attribute");
    folderAttrib.append_attribute("id") = "Folder";
    folderAttrib.append_attribute("type") = "LSString";
    folderAttrib.append_attribute("value") = "GustavDev";

    pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
    md5Attrib.append_attribute("id") = "MD5";
    md5Attrib.append_attribute("type") = "LSString";
    md5Attrib.append_attribute("value") = "";

    pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
    nameAttrib.append_attribute("id") = "Name";
    nameAttrib.append_attribute("type") = "LSString";
    nameAttrib.append_attribute("value") = "GustavDev";

    pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
    uuidAttrib.append_attribute("id") = "UUID";
    uuidAttrib.append_attribute("type") = "FixedString";
    uuidAttrib.append_attribute("value") = "28ac9ce2-2aba-8cda-b3b5-6e922f71b6b8";


    pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
    versionAttrib.append_attribute("id") = "Version64";
    versionAttrib.append_attribute("value") = "36028797018963968";
    versionAttrib.append_attribute("type") = "int64";

    Core::Base::lampLog::getInstance().log("Saving modsettings.lsx", Core::Base::lampLog::warningLevel::LOG);
    if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
        return {-1, "Failed to save modsettings.lsx"};
    }

    Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 6;
    return {1, "PreCleanup Finished."};
}

Lamp::Game::lampReturn Lamp::Game::BG3::preDeployment() {
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,0};
    Core::Base::lampLog::getInstance().log("Extracting Archives", Core::Base::lampLog::warningLevel::LOG);
    auto lambdaFunction = [](const Core::Base::lampMod::Mod* item) {
        if(item->enabled) {
            Lamp::Core::lampControl::getInstance().deplopmentTracker.second++;
        if(Lamp::Core::FS::lampExtract::extract(item)) {

            std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Lamp::Games::getInstance().currentGame->Ident().ReadableName;
            std::filesystem::path dirit(workingDir + "/ext/" + std::filesystem::path(item->ArchivePath).filename().stem().string());

            try {
                for (const auto &entry: std::filesystem::directory_iterator(dirit)) {
                    if (entry.is_regular_file()) {
                        std::string fileName = entry.path().filename().string();
                        if (fileName.find('\\') != std::string::npos) {
                            std::string inputString = fileName;
                            size_t found = inputString.find('\\');
                            while (found != std::string::npos) {
                                inputString.replace(found, 1, "/");
                                found = inputString.find('\\', found + 1);
                            }
                            std::filesystem::path temp(inputString);
                            std::filesystem::rename(entry.path(),
                                                    workingDir +  "/ext/" + std::filesystem::path(item->ArchivePath).filename().stem().string() + "/" + temp.filename().string());
                            std::cout << "File with backslash in name: " << fileName << std::endl;
                        }
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }




            std::string x = item->ArchivePath;
            switch (item->modType) {
                case BG3_ENGINE_INJECTION:
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item,"dll","bin/NativeMods");
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item,"toml","bin/NativeMods");
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item,"ini","bin/NativeMods");
                    break;
                case BG3_MOD:
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item,"pak","Mods");

                    Core::Base::LampSequencer::add("BG3 Settings Control", [x]() -> lampReturn {
                        if(!Lamp::Core::Parse::lampBG3PakParser::collectJsonData(x)){
                            return Lamp::Core::Parse::lampBG3PakParser::extractJsonData(x);
                        }else{
                            return true;
                        }
                    });
                    break;
                case BG3_BIN_OVERRIDE:
                    Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"bin","bin");
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item, "dll", "bin");
                    break;
                case BG3_DATA_OVERRIDE:
                    Lamp::Core::FS::lampExtract::moveModSpecificFolder(item,"Data","Data");
                    break;
                case BG3_MOD_FIXER:
                    Lamp::Core::FS::lampExtract::moveModSpecificFileType(item,"pak","Mods");
                    break;
                default:
                    break;
            }
            Lamp::Core::lampControl::getInstance().deplopmentTracker.first++;
        }
        }
    };
    lampExecutor executor(lambdaFunction);
    executor.execute(ModList);
    Core::Base::LampSequencer::run("BG3 Settings Control");
    return {1,"TESTER"};
}

Lamp::Game::lampReturn Lamp::Game::BG3::deployment() {
    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Ident().ReadableName;
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,5};

    try {
        Lamp::Core::FS::lampTrack::handleFileDescriptor A{
            Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyFilesIgnoreExt,
            Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
            workingDir+"/bin/",
            keyInfo["installDirPath"]+"/bin/",
            "NativeMods",
            Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(A);
        Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 1;



        Core::Base::lampLog::getInstance().log("Copying NativeMods");


        Lamp::Core::FS::lampTrack::handleFileDescriptor B{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyOnlyExt,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
                workingDir+"/bin/NativeMods",
                keyInfo["installDirPath"]+"/bin/NativeMods",
                ".dll",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(B);

        Lamp::Core::FS::lampTrack::handleFileDescriptor B2{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyOnlyExt,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::skipExisting,
                workingDir+"/bin/NativeMods",
                keyInfo["installDirPath"]+"/bin/NativeMods",
                ".toml",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(B2);

        Lamp::Core::FS::lampTrack::handleFileDescriptor B3{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyOnlyExt,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::skipExisting,
                workingDir+"/bin/NativeMods",
                keyInfo["installDirPath"]+"/bin/NativeMods",
                ".ini",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(B3);

        Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 2;

        Core::Base::lampLog::getInstance().log("Copying Data");
        Lamp::Core::FS::lampTrack::handleFileDescriptor C{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyFolder,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
                workingDir+"/Data/",
                keyInfo["installDirPath"]+"/Data/",
                "",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(C);


        Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 3;

        Core::Base::lampLog::getInstance().log("Copying Mods");

        Lamp::Core::FS::lampTrack::handleFileDescriptor D{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyFolder,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
                workingDir+"/Mods",
                KeyInfo()["appDataPath"]+"/Mods/",
                "",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(D);




        Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 4;

        Core::Base::lampLog::getInstance().log("Copying ModProfile");

        Lamp::Core::FS::lampTrack::handleFileDescriptor E{
                Lamp::Core::FS::lampTrack::handleFileDescriptor::operation::copyFolder,
                Lamp::Core::FS::lampTrack::handleFileDescriptor::mode::updateExisting,
                workingDir+"/PlayerProfiles",
                KeyInfo()["appDataPath"]+"/PlayerProfiles/",
                "",
                Ident().ReadableName
        };
        Lamp::Core::FS::lampTrack::handleFile(E);

        Lamp::Core::lampControl::getInstance().deplopmentTracker.first = 5;

        return {1, "Deployed"};
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return {0, "Deployment Failed."};
    }
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



