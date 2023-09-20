//
// Created by charles on 11/09/23.
//

#include <filesystem>
#include <iostream>
#include <regex>
#include <fstream>
#include "BG3.h"
#include "../../json/json.hpp"
#include "../../lampWarn.h"


namespace Lamp {
    void Game::BG3::registerArchive(std::string Path) {
        Lamp::Core::lampMod::Mod  * newArchive = new Lamp::Core::lampMod::Mod{Path,ModType::NaN, false};
        ModList.push_back(newArchive);
        Lamp::Core::lampFilesystem::getInstance().saveModList(Core::lampConfig::BG3,ModList);
    }

    void Game::BG3::listArchives() {

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
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
                    }
                }else{
                    if (ImGui::Button(("Disabled##" + std::to_string(i)).c_str())) {
                        (*it)->enabled = true;
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
                    }
                }
                ImGui::TableNextColumn();
                ImGui::Text(path.filename().c_str());


                ImGui::TableNextColumn();

                std::string Type = "";

                switch ((*it)->modType) {
                    case ModType::BG3_ENGINE_INJECTION:
                        Type = "Engine Injection";
                        break;
                    case ModType::BG3_MOD:
                        Type = "Standard Mod";
                        break;
                    case ModType::BG3_BIN_OVERRIDE:
                        Type = "Bin Overwrite";
                        break;
                    case ModType::BG3_DATA_OVERRIDE:
                        Type = "Data Overwrite";
                        break;
                    case ModType::NaN:
                        Type = "Select Type";
                        break;
                }


                if (ImGui::BeginMenu((Type + "##" + std::to_string(i)).c_str())) {

                    if (ImGui::MenuItem("Engine Injection")) {
                        (*it)->modType = ModType::BG3_ENGINE_INJECTION;
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
                    }
                    if (ImGui::MenuItem("Standard Mod")) {
                        (*it)->modType = ModType::BG3_MOD;
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
                    }
                    if (ImGui::MenuItem("Bin Overwrite")) {
                        (*it)->modType = ModType::BG3_BIN_OVERRIDE;
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
                    }
                    if (ImGui::MenuItem("Data Overwrite")) {
                        (*it)->modType = ModType::BG3_DATA_OVERRIDE;
                        Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
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
                    Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3, ModList);
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

    bool Game::BG3::ConfigMenu() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGuiWindowFlags windowFlags = 0;
        windowFlags += ImGuiWindowFlags_NoMove;
        windowFlags += ImGuiWindowFlags_NoResize;
        windowFlags += ImGuiWindowFlags_NoCollapse;
        windowFlags += ImGuiWindowFlags_NoTitleBar;
        windowFlags += ImGuiWindowFlags_MenuBar;

        ImGui::Begin("BG3 Steam Path Setup", NULL,windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

        ImGui::Text("BG3 Steam Directory");
        if(ImGui::Button((installDirPath+"##installpath").c_str())) {
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_PickFolder(NULL, &outPath);

            if (result == NFD_OKAY) {
                puts(outPath);
                installDirPath = outPath;
                Lamp::Core::lampFilesystem::getInstance().saveKeyData(Core::lampConfig::BG3, "installDirPath",installDirPath);

            } else if (result == NFD_CANCEL) {
                puts("User pressed cancel.");
            } else {
                printf("Error: %s\n", NFD_GetError());
            }
        }

        ImGui::Separator();

        ImGui::Text("BG3 AppData Directory");
        if(ImGui::Button((appDataPath+"##appdatapath").c_str())){
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_PickFolder( NULL, &outPath );

            if ( result == NFD_OKAY ) {
                puts(outPath);
                appDataPath = outPath;
                Lamp::Core::lampFilesystem::getInstance().saveKeyData(Core::lampConfig::BG3, "appDataPath",appDataPath);
            }
            else if ( result == NFD_CANCEL ) {
                puts("User pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError() );
            }
        }
        ImGui::Separator();

        if(checkLock) {
            ImGui::Separator();
            ImGui::Text("WARNING: Incorrect folder may be selected.");
        }


        if (ImGui::Button("Close")){

            std::stringstream ss(installDirPath);
            std::stringstream ssE(appDataPath);
            std::string token;
            std::string folderNameA;
            std::string folderNameB;
            while (std::getline(ss, token, '/')) {
                // Skip empty tokens (e.g., double slashes)
                if (!token.empty()) {
                    folderNameA = token;
                }
            }
            while (std::getline(ssE, token, '/')) {
                // Skip empty tokens (e.g., double slashes)
                if (!token.empty()) {
                    folderNameB = token;
                }
            }

            if(folderNameA == "Baldurs Gate 3" && folderNameB == "Baldur's Gate 3"){
                ImGui::End();
                checkLock = false;
                return true;
            }else{
                checkLock = true;
            }


            ImGui::End();
            return false;
        }

        ImGui::End();
        return false;
    }

    bool Game::BG3::createFileStructure() {
        return false;
    }

    bool Game::BG3::startDeployment() {
        if(appDataPath == "" || appDataPath == " "){
            return false;
        }
        if(installDirPath == "" || installDirPath == " "){
            return false;
        }

        if(preCleanUp()){
            if(preDeployment()){
                // doing it early.
                postDeploymentTasks();
                if(deployment()){
                    Lamp::Core::lampWarn::getInstance().log("Deployment Successful!", Core::lampWarn::LOG, true);
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

    bool Game::BG3::preCleanUp() {
        try {
            std::string workingDir = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(
                    Lamp::Core::lampConfig::BG3);

            std::filesystem::path dir(workingDir);

            for (const auto &entry: std::filesystem::directory_iterator(dir)) {
                std::filesystem::remove_all(entry.path());
            }

            std::filesystem::create_directories(workingDir + "/bin/NativeMods");
            std::filesystem::create_directories(workingDir + "/Data");
            std::filesystem::create_directories(workingDir + "/Mods");
            std::filesystem::create_directories(workingDir + "/PlayerProfiles/Public");
            std::filesystem::create_directories(workingDir + "/ext");

            std::filesystem::copy_file(appDataPath + "/PlayerProfiles/Public/modsettings.lsx",
                                       workingDir + "/PlayerProfiles/Public/modsettings.lsx",
                                       std::filesystem::copy_options::overwrite_existing);


            pugi::xml_document doc;
            if (doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                pugi::xml_node modOrderNode = doc.select_node("//node[@id='ModOrder']").node();
                if (modOrderNode) {
                    pugi::xml_node childrenNode = modOrderNode.child("children");
                    if (childrenNode) {
                        childrenNode.remove_children();
                    }
                } else {
                    std::cerr << "ModOrder section not found in XML." << std::endl;
                }

                pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();
                if (modsNode) {
                    pugi::xml_node childrenNode = modsNode.child("children");
                    if (childrenNode) {
                        childrenNode.remove_children();
                    }
                } else {
                    std::cerr << "Mods section not found in XML." << std::endl;
                }



                /// adding GustavDev

                pugi::xml_node moduleNode = doc.select_node("//node[@id='ModOrder']").node();
                pugi::xml_node childrenNode = moduleNode.child("children");
                pugi::xml_node newNode = childrenNode.append_child("node");
                newNode.append_attribute("id") = "Module";
                pugi::xml_node attributes = newNode.append_child("attribute");
                attributes.append_attribute("id") = "UUID";
                attributes.append_attribute("value") = "28ac9ce2-2aba-8cda-b3b5-6e922f71b6b8";
                attributes.append_attribute("type") = "FixedString";

                pugi::xml_node modsNode2 = doc.select_node("//node[@id='Mods']").node();
                childrenNode = modsNode2.child("children");

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


                if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                    std::cerr << "Failed to save XML file." << std::endl;
                }
            }
            return true;
        }catch (std::exception ex){
            return false;
        }
    }

    bool Game::BG3::preDeployment() {
        std::list<Lamp::Core::lampMod::Mod *>::iterator it;

        for (it = ModList.begin(); it != ModList.end(); ++it) {
            if ((*it)->enabled) {
            if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(zip|rar|7z)$"))) {


                switch ((*it)->modType) {
                    case ModType::BG3_ENGINE_INJECTION:
                        if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(zip)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Zip, (*it),
                                                                              "/bin/NativeMods");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(rar)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Rar, (*it),
                                                                              "/bin/NativeMods");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(7z)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::SevenZip, (*it),
                                                                              "/bin/NativeMods");
                        } else {
                            break;
                        }
                        break;
                    case ModType::BG3_BIN_OVERRIDE:
                        if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(zip)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Zip, (*it), "/");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(rar)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Rar, (*it), "/");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(7z)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::SevenZip, (*it), "/");
                        } else {
                            break;
                        }
                        break;
                    case ModType::BG3_DATA_OVERRIDE:
                        if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(zip)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Zip, (*it), "/Data");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(rar)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::Rar, (*it), "/Data");
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(7z)$"))) {
                            Lamp::Core::lampFilesystem::getInstance().extract(Lamp::Core::lampConfig::BG3,
                                                                              bit7z::BitFormat::SevenZip, (*it),
                                                                              "/Data");
                        } else {
                            break;
                        }
                        break;
                    case ModType::BG3_MOD:
                        std::vector<std::string> xx;
                        if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(zip)$"))) {
                            xx = Lamp::Core::lampFilesystem::getInstance().extractSpecificFileType(
                                    Lamp::Core::lampConfig::BG3, bit7z::BitFormat::Zip, (*it), "/Mods", "pak");
                            if(collectJsonData()){
                                break;
                            }else{
                                findJsonData(xx);
                            }
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(rar)$"))) {
                            xx = Lamp::Core::lampFilesystem::getInstance().extractSpecificFileType(
                                    Lamp::Core::lampConfig::BG3, bit7z::BitFormat::Rar, (*it), "/Mods", "pak");
                            if(collectJsonData()){
                                break;
                            }else{
                                findJsonData(xx);
                            }
                        } else if (std::regex_match((*it)->ArchivePath, std::regex("^.*\\.(7z)$"))) {
                            xx = Lamp::Core::lampFilesystem::getInstance().extractSpecificFileType(
                                    Lamp::Core::lampConfig::BG3, bit7z::BitFormat::SevenZip, (*it), "/Mods", "pak");
                            if(collectJsonData()){
                                break;
                            }else{
                                findJsonData(xx);
                            }
                        } else {
                            break;
                        }
                        break;
                }

            } else {
                break;
            }


        }
        }
        return true;
    }

    bool Game::BG3::findJsonData(std::vector<std::string> xx){

        pugi::xml_document doc;
        std::string uuidout;

        std::string workingDir = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(
                Lamp::Core::lampConfig::BG3);
        if (!doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
            std::cerr << "Failed to load XML file." << std::endl;
            return false;
        }

        for (const std::string& str : xx) {
            std::cout << str << std::endl;
            std::ifstream inputFile(str);

            // Check if the file is open
            if (!inputFile.is_open()) {
                std::cerr << "Failed to open the file!" << std::endl;
                return 1;
            }

            std::regex uuidPattern("\\b[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\\b");

            std::string line;

            while (std::getline(inputFile, line)) {
                std::smatch match;
                if (std::regex_search(line, match, uuidPattern)) {
                    std::cout << "Found UUID in line: " << line << std::endl;
                    uuidout = match[0];

                    break;
                }
            }

            std::filesystem::path assumedFolderName(str.c_str());

            if(uuidout != ""){
            pugi::xml_node moduleNode = doc.select_node("//node[@id='ModOrder']").node();
            pugi::xml_node childrenNode = moduleNode.child("children");
            pugi::xml_node newNode = childrenNode.append_child("node");
            newNode.append_attribute("id") = "Module";
            pugi::xml_node attributes = newNode.append_child("attribute");
            attributes.append_attribute("id") = "UUID";
            attributes.append_attribute("value") = uuidout.c_str();
            attributes.append_attribute("type") = "FixedString";

            pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();
            childrenNode = modsNode.child("children");

            pugi::xml_node newShortDescNode = childrenNode.append_child("node");
            newShortDescNode.append_attribute("id") = "ModuleShortDesc";

            pugi::xml_node folderAttrib = newShortDescNode.append_child("attribute");
            folderAttrib.append_attribute("id") = "Folder";
            folderAttrib.append_attribute("type") = "LSString";
            folderAttrib.append_attribute("value") = assumedFolderName.filename().c_str();

            pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
            md5Attrib.append_attribute("id") = "MD5";
            md5Attrib.append_attribute("type") = "LSString";
            md5Attrib.append_attribute("value") = "";

            pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
            nameAttrib.append_attribute("id") = "Name";
            nameAttrib.append_attribute("type") = "LSString";
            nameAttrib.append_attribute("value") = assumedFolderName.filename().c_str();

            pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
            uuidAttrib.append_attribute("id") = "UUID";
            uuidAttrib.append_attribute("type") = "FixedString";
            uuidAttrib.append_attribute("value") = uuidout.c_str();


            pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
            versionAttrib.append_attribute("id") = "Version64";
            versionAttrib.append_attribute("value") = "36028797018963968";
            versionAttrib.append_attribute("type") = "int64";


            if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                std::cerr << "Failed to save XML file." << std::endl;
            }

            inputFile.close();
        }
        }
        }

    bool Game::BG3::collectJsonData() {
        std::string workingDir = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(
                Lamp::Core::lampConfig::BG3);
        for (const auto& entry : std::filesystem::directory_iterator((workingDir + "/ext").c_str())) {
            if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(json)$") )) {

                std::ifstream jsonFile(entry.path());
                pugi::xml_document doc;

                if (!jsonFile.is_open()) {
                    std::cerr << "Failed to open JSON file." << std::endl;
                    return false;
                }


                if (!doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                    std::cerr << "Failed to load XML file." << std::endl;
                    return false;
                }

                nlohmann::json jsonData;
                jsonFile >> jsonData;
                jsonFile.close();

                if (jsonData.contains("mods") || jsonData.contains("Mods")) {

                    std::string packString = "mods";
                    if(jsonData.contains("Mods")){
                        packString = "Mods";
                    }

                    for (const auto& mod : jsonData[packString]) {
                        std::string modName;
                        if(mod.contains("modName")){
                            modName = mod["modName"];
                        }else if(mod.contains("ModName")){
                            modName = mod["ModName"];
                        }else{
                            modName = mod["Name"];
                        }

                        std::string folderName;
                        if(mod.contains("folderName")){
                            folderName = mod["folderName"];
                        }else if(mod.contains("FolderName")){
                            folderName = mod["FolderName"];
                        }else{
                            folderName = mod["Folder"];
                        }

                        std::string UUID = mod["UUID"];

                        //std::cout << "Mod Name: " << modName << std::endl;
                        //std::cout << "UUID: " << UUID << std::endl;
                        //std::cout << "Folder Name: " << folderName << std::endl;

                        pugi::xml_node moduleNode = doc.select_node("//node[@id='ModOrder']").node();
                        pugi::xml_node childrenNode = moduleNode.child("children");
                        pugi::xml_node newNode = childrenNode.append_child("node");
                        newNode.append_attribute("id") = "Module";
                        pugi::xml_node attributes = newNode.append_child("attribute");
                        attributes.append_attribute("id") = "UUID";
                        attributes.append_attribute("value") = UUID.c_str();
                        attributes.append_attribute("type") = "FixedString";

                        pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();
                        childrenNode = modsNode.child("children");

                        pugi::xml_node newShortDescNode = childrenNode.append_child("node");
                        newShortDescNode.append_attribute("id") = "ModuleShortDesc";

                        pugi::xml_node folderAttrib = newShortDescNode.append_child("attribute");
                        folderAttrib.append_attribute("id") = "Folder";
                        folderAttrib.append_attribute("type") = "LSString";
                        folderAttrib.append_attribute("value") = folderName.c_str();

                        pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
                        md5Attrib.append_attribute("id") = "MD5";
                        md5Attrib.append_attribute("type") = "LSString";
                        md5Attrib.append_attribute("value") = "";

                        pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
                        nameAttrib.append_attribute("id") = "Name";
                        nameAttrib.append_attribute("type") = "LSString";
                        nameAttrib.append_attribute("value") = modName.c_str();

                        pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
                        uuidAttrib.append_attribute("id") = "UUID";
                        uuidAttrib.append_attribute("type") = "FixedString";
                        uuidAttrib.append_attribute("value") = UUID.c_str();


                        pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
                        versionAttrib.append_attribute("id") = "Version64";
                        versionAttrib.append_attribute("value") = "36028797018963968";
                        versionAttrib.append_attribute("type") = "int64";


                        if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                            std::cerr << "Failed to save XML file." << std::endl;
                        }

                        return true;
                    }
                } else {
                    std::cerr << "JSON data does not contain 'mods' key." << std::endl;
                    return false;
                }

            }
        }


    }

    bool Game::BG3::deployment() {
        std::string workingDir = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(
                Lamp::Core::lampConfig::BG3);
        try {
            std::filesystem::path sourceDirectory = workingDir+"/bin/";
            std::filesystem::path destinationDirectory = installDirPath+"/bin/";
            Lamp::Core::lampWarn::getInstance().log("Copying Bin");
            std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);


            sourceDirectory = workingDir+"/Data/";
            destinationDirectory = installDirPath+"/Data/";
            Lamp::Core::lampWarn::getInstance().log("Copying Data");
            std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

            sourceDirectory = workingDir+"/Mods";
            destinationDirectory = appDataPath+"/Mods/";
            Lamp::Core::lampWarn::getInstance().log("Copying Mods");
            std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

            sourceDirectory =  workingDir+"/PlayerProfiles";
            destinationDirectory = appDataPath+"/PlayerProfiles/";
            Lamp::Core::lampWarn::getInstance().log("Copying ModProfile");
            std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }

    void Game::BG3::postDeploymentTasks() {
        std::string workingDir = Lamp::Core::lampFilesystem::getInstance().getGameSpecificStoragePath(
                Lamp::Core::lampConfig::BG3);

        try {
            for (const auto& entry : fs::directory_iterator(workingDir)) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();
                    if (fileName.find('\\') != std::string::npos) {

                        std::string inputString = fileName;

                        // Replace all instances of backslash with forward slash
                        size_t found = inputString.find('\\');
                        while (found != std::string::npos) {
                            inputString.replace(found, 1, "/");
                            found = inputString.find('\\', found + 1);
                        }



                        std::filesystem::rename(entry.path(), workingDir+"/"+inputString);
                        std::cout << "File with backslash in name: " << fileName << std::endl;
                    }
                }
            }
        } catch (const std::exception& e) {
            Lamp::Core::lampWarn::getInstance().log("Json Extraction Failed", Core::lampWarn::ERROR, true);
        }
    }


} // Lamp