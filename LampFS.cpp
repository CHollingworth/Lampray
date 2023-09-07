//
// Created by charles on 02/09/23.
//


#include "LampFS.h"


void LampFS::fileDrop(GLFWwindow *window, int count, const char **paths) {
    int i;
    for (i = 0; i < count; i++){
        // Thank you! Roi Danton on stackoverflow for this clean code.
        std::cout << paths[i] << std::endl;
        std::filesystem::path path(paths[i]);
        std::error_code ec;
        if (std::filesystem::is_regular_file(path, ec))
        {
            if (std::regex_match (path.filename().string(), std::regex("^.*\\.(zip|rar|7z)$") )) {
                std::filesystem::path targetDIR = "Holding/Archives/Backup"; // Roi Danton many thanks again!
                auto target = targetDIR / path.filename();
                try
                {
                    fs::create_directories(targetDIR);
                    fs::copy_file(path, target, fs::copy_options::overwrite_existing);
                    Archive * newArchive = new Archive;
                    newArchive->enabled = false;
                    newArchive->holdingPath = target;
                    newArchive->type = ModType::NaN;
                    LampFS::getInstance().Archives.push_back(newArchive);
                    LampFS::getInstance().saveArchives();
                }
                catch (std::exception& e)
                {
                    std::cout << e.what();
                }
            }

        }


        if (ec)
        {
            std::cerr << "Error" << ec.message();
        }
        std::cout << std::endl<< std::endl;
    }
}

int LampFS::loadArchives() {
    std::string text;
    std::ifstream file;
    file.open("Holding/Archives/archives.csv");
    getline(file, text);
    if(text == "") return 0;
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    int count = 0;
    Archive * arv = new Archive;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        std::cout << token;
            switch (count) {
                case 0:
                    arv->type = static_cast<ModType>(std::stoi(token));
                    count++;
                    break;
                case 1:
                    arv->holdingPath = token;
                    count++;
                    break;
                case 2:
                    arv->enabled = static_cast<bool>(std::stoi(token));
                    std::cout << std::endl;
                    count = 0;
                    break;
            }
        text.erase(0, pos + delimiter.length());
        if(count == 0){
            Archives.push_back(arv);
            arv = new Archive;
        }
    }
    saveArchives();

    return 1;
}

void LampFS::saveArchives() {
    std::list<Archive *>::iterator it;
    std::ofstream file("Holding/Archives/archives.csv", std::ios::trunc);
    for (it = Archives.begin(); it != Archives.end(); ++it){
        file << (*it)->serialize();
    }
    file.close();
}

void LampFS::listArchives() {
    std::list<Archive *>::iterator it;
    int i = 0;
    for (it = Archives.begin(); it != Archives.end(); ++it){

        std::filesystem::path path((*it)->holdingPath);
        ImGui::TableNextColumn();
        ImGui::Text(path.filename().c_str());
        i++;

        ImGui::TableNextColumn();

        std::string Type = "";

        switch ((*it)->type) {
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


        if (ImGui::BeginMenu((Type + "##" +std::to_string(i)).c_str())){

            if(ImGui::MenuItem("Engine Injection")){
                (*it)->type = ModType::BG3_ENGINE_INJECTION;
                saveArchives();
            }
            if(ImGui::MenuItem("Standard Mod")){
                (*it)->type = ModType::BG3_MOD;
                saveArchives();
            }
            if(ImGui::MenuItem("Bin Overwrite")){
                (*it)->type = ModType::BG3_BIN_OVERRIDE;
                saveArchives();
            }
            if(ImGui::MenuItem("Data Overwrite")){
                (*it)->type = ModType::BG3_DATA_OVERRIDE;
                saveArchives();
            }
            ImGui::EndMenu();
        }

        ImGui::TableNextColumn();
        if(ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())){
            removeArchive(path, (*it));
            break;
        }

        ImGui::TableNextRow();
    }

}


int LampFS::removeArchive(std::filesystem::path path, LampFS::Archive * arch) {
    std::remove(absolute(path).c_str());
    std::cout<<absolute(path).c_str() << std::endl;
    Archives.remove(arch);
    saveArchives();
}

void LampFS::saveConfig() {
    std::list<Archive *>::iterator it;
    std::ofstream file("config.csv", std::ios::trunc);
    file << installDirPath+","+appDataPath+","; // yes this is lazy.
    file.close();
}

void LampFS::loadConfig() {
    std::string text;
    std::ifstream file;
    file.open("config.csv");
    getline(file, text);

    if(text == "") return;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        std::cout << token;
        if(installDirPath == "steam/install/path"){
            installDirPath = token;
        }else{
            appDataPath = token;
        }
        text.erase(0, pos + delimiter.length());
    }
}

void LampFS::deploy() {
    /// {PRE CLEAN}
    deploymentStage = deployment_stages::BG3_PRE_DEPLOYMENT_CLEAN_UP;
    deployDisplayMax = 1;
    deployDisplayCount = 0;


    std::filesystem::path dir("Holding/PreDeployment/");

    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        std::filesystem::remove_all(entry.path());
    }

    std::filesystem::create_directories("Holding/PreDeployment/bin");
    std::filesystem::create_directories("Holding/PreDeployment/bin/NativeMods");
    std::filesystem::create_directories("Holding/PreDeployment/data");
    std::filesystem::create_directories("Holding/PreDeployment/Mods");
    std::filesystem::create_directories("Holding/PreDeployment/PlayerProfiles/Public");

    fs::copy_file(appDataPath+"/PlayerProfiles/Public/modsettings.lsx", "Holding/PreDeployment/PlayerProfiles/Public/modsettings.lsx", fs::copy_options::overwrite_existing);


    pugi::xml_document doc;
    if (doc.load_file("Holding/PreDeployment/PlayerProfiles/Public/modsettings.lsx")) {
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

        if (!doc.save_file("Holding/PreDeployment/PlayerProfiles/Public/modsettings.lsx")) {
            std::cerr << "Failed to save XML file." << std::endl;
        }

    }
    deployDisplayCount = 1;

    /// {PRE DEPLOY}
    deploymentStage = deployment_stages::BG3_PRE_DEPLOYMENT;
    deployDisplayMax = Archives.size() + 1;
    deployDisplayCount = 0;
    std::list<Archive *>::iterator it;
    for (it = Archives.begin(); it != Archives.end(); ++it) {
        deployDisplayCount++;

        if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(zip|rar|7z)$") )){


        switch ((*it)->type) {
            case ModType::BG3_ENGINE_INJECTION:
                if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(zip)$") )){
                    extract(bit7z::BitFormat::Zip, it, "Holding/PreDeployment/bin/NativeMods");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(rar)$") )){
                    extract(bit7z::BitFormat::Rar, it, "Holding/PreDeployment/bin/NativeMods");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(7z)$") )){
                    extract(bit7z::BitFormat::SevenZip, it, "Holding/PreDeployment/bin/NativeMods");
                }else{
                    break;
                }
                break;
            case ModType::BG3_BIN_OVERRIDE:
                if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(zip)$") )){
                    extract(bit7z::BitFormat::Zip, it, "Holding/PreDeployment/");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(rar)$") )){
                    extract(bit7z::BitFormat::Rar, it, "Holding/PreDeployment/");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(7z)$") )){
                    extract(bit7z::BitFormat::SevenZip, it, "Holding/PreDeployment/");
                }else{
                    break;
                }
                //checkForAndFixPathIncludedFileNames(); // Looking at you BG3AchievementEnabler >.>
                break;
            case ModType::BG3_DATA_OVERRIDE:
                if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(zip)$") )){
                    extract(bit7z::BitFormat::Zip, it, "Holding/PreDeployment/data");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(rar)$") )){
                    extract(bit7z::BitFormat::Rar, it, "Holding/PreDeployment/data");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(7z)$") )){
                    extract(bit7z::BitFormat::SevenZip, it, "Holding/PreDeployment/data");
                }else{
                    break;
                }
                break;
            case ModType::BG3_MOD:
                if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(zip)$") )){
                    extractPackFileOnly(bit7z::BitFormat::Zip, it, "Holding/PreDeployment/Mods");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(rar)$") )){
                    extractPackFileOnly(bit7z::BitFormat::Rar, it, "Holding/PreDeployment/Mods");
                }else if(std::regex_match ((*it)->holdingPath, std::regex("^.*\\.(7z)$") )){
                    extractPackFileOnly(bit7z::BitFormat::SevenZip, it, "Holding/PreDeployment/Mods");
                }else{
                    break;
                }
                break;
        }

        }else{
            break;
        }



    }
    checkForOrphanedFiles();
    deploymentStage = deployment_stages::BG3_DEPLOYMENT;
    try {
        std::filesystem::path sourceDirectory = "Holding/PreDeployment/bin/";
        std::filesystem::path destinationDirectory = installDirPath+"/bin/";
        std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

        sourceDirectory = "Holding/PreDeployment/data/";
        destinationDirectory = installDirPath+"/data/";
        std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

        sourceDirectory = "Holding/PreDeployment/Mods";
        destinationDirectory = appDataPath+"/Mods/";
        std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

        sourceDirectory = "Holding/PreDeployment/PlayerProfiles";
        destinationDirectory = appDataPath+"/PlayerProfiles/";
        std::filesystem::copy(sourceDirectory, destinationDirectory, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    deploymentStage = deployment_stages::BG3_IDLE;

    return;
}

void LampFS::extract(const bit7z::BitInFormat & Type, std::list<Archive *>::iterator it, std::string extractionPath) {
    try {
        bit7z::Bit7zLibrary lib{};
        bit7z::BitArchiveReader reader{lib, (*it)->holdingPath, Type};
        reader.test();
        reader.extract(extractionPath);
    } catch (const bit7z::BitException &ex) { std::cout << ex.what() << std::endl << ex.code() << std::endl; }

}

// This is expensive and needs refactoring.
bool LampFS::extractPackFileOnly(const bit7z::BitInFormat &Type, std::list<Archive *>::iterator it,
                                 std::string extractionPath) {
    for (const auto& entry : std::filesystem::directory_iterator("Holding/ext")) {
        std::filesystem::remove_all(entry.path());
    }

    try {
        bit7z::Bit7zLibrary lib{};
        bit7z::BitArchiveReader reader{lib, (*it)->holdingPath, Type};
        reader.test();
        reader.extract("Holding/ext");
    } catch (const bit7z::BitException &ex) { std::cout << ex.what() << std::endl << ex.code() << std::endl; }


    for (const auto& entry : std::filesystem::directory_iterator("Holding/ext")) {
        if(entry.is_directory()){
            std::cout << entry  << '\n';
            for (const auto& subentry : std::filesystem::directory_iterator(entry)){
                if(std::regex_match (subentry.path().filename().string(), std::regex("^.*\\.(pak)$") )){
                    try {
                        if(insertIntoModSettings(entry.path())) {
                            std::filesystem::rename(subentry.path(),extractionPath + "/" + subentry.path().filename().string());
                        }
                    } catch (std::filesystem::filesystem_error& e) {
                        std::cout << e.what() << '\n';
                    }
                }
            }
        }else if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(pak)$") )){
            try {
                if(insertIntoModSettings("Holding/ext")) {
                    std::filesystem::rename(entry.path(), extractionPath + "/" + entry.path().filename().string());
                }
            } catch (std::filesystem::filesystem_error& e) {
                std::cout << e.what() << '\n';
            }
        }
    }


    return false;
}



bool LampFS::insertIntoModSettings(std::string infopath) {
    for (const auto& entry : std::filesystem::directory_iterator(infopath)) {
        if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(json)$") )) {

            std::ifstream jsonFile(entry.path());
            pugi::xml_document doc;

            if (!jsonFile.is_open()) {
                std::cerr << "Failed to open JSON file." << std::endl;
                return 1;
            }


            if (!doc.load_file("Holding/PreDeployment/PlayerProfiles/Public/modsettings.lsx")) {
                std::cerr << "Failed to load XML file." << std::endl;
                return 1;
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

                    std::cout << "Mod Name: " << modName << std::endl;
                    std::cout << "UUID: " << UUID << std::endl;
                    std::cout << "Folder Name: " << folderName << std::endl;

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


                    if (!doc.save_file("Holding/PreDeployment/PlayerProfiles/Public/modsettings.lsx")) {
                        std::cerr << "Failed to save XML file." << std::endl;
                    }
                }
            } else {
                std::cerr << "JSON data does not contain 'mods' key." << std::endl;
                return 1;
            }

            }
        }


    return true;
}

void LampFS::checkForOrphanedFiles() {
    std::string folderPath = "Holding/PreDeployment"; // Replace with the path to your folder

    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
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



                    std::filesystem::rename(entry.path(), folderPath+"/"+inputString);
                    std::cout << "File with backslash in name: " << fileName << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}




//



//
//
//     bit7z::BitArchiveReader reader{lib, (*it)->holdingPath, bit7z::BitFormat::Rar};
//             bit7z::BitArchiveReader reader{lib, (*it)->holdingPath, bit7z::BitFormat::SevenZip};
//
//
//
//
//




