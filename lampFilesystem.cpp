//
// Created by charles on 11/09/23.
//

#include <iostream>
#include <filesystem>
#include <regex>
#include "lampFilesystem.h"
#include "game-data/BG3/BG3.h"
#include "game-data/gameControl.h"
#include "lampWarn.h"

namespace Lamp {
    bool Core::lampFilesystem::init() {
        Lamp::Core::lampWarn::getInstance().log("Initializing Lamp FileSystem");
        try {
            std::filesystem::create_directories(saveDataPath);
            std::filesystem::create_directories(archiveDataPath);
            std::filesystem::create_directories(ConfigDataPath);
            std::filesystem::create_directories(DeploymentDataPath);
        }catch(std::exception ex){
            Lamp::Core::lampWarn::getInstance().log("Could not create base directories", lampWarn::ERROR);
        }

        std::map<Lamp::Core::lampConfig::Game, std::string>::iterator it;
        Lamp::Core::lampWarn::getInstance().log("Creating Game directories");
        for (it = Core::lampConfig::getInstance().GameStringMap.begin(); it != Core::lampConfig::getInstance().GameStringMap.end(); it++)
        {
            std::filesystem::create_directories(DeploymentDataPath + it->second);
            Lamp::Core::lampWarn::getInstance().log(it->second);
        }
        Lamp::Core::lampWarn::getInstance().log("Finished creating Game directories");
        return true;
    }

    void Core::lampFilesystem::fileDrop(GLFWwindow *window, int count, const char **paths) {
        if(Lamp::Core::lampConfig::getInstance().CurrentGame == Lamp::Core::lampConfig::Game::UNK){
            return;
        }
        int i;
        for (i = 0; i < count; i++){
            // Thank you! Roi Danton on stackoverflow for this clean code.
            std::string pth = paths[i];
            Lamp::Core::lampWarn::getInstance().log("File Drop Detected: " +pth);
            std::filesystem::path path(paths[i]);
            std::error_code ec;
            if (std::filesystem::is_regular_file(path, ec))
            {
                if (std::regex_match (path.filename().string(), std::regex("^.*\\.(zip|rar|7z)$") )) {
                    std::filesystem::path targetDIR = Core::lampFilesystem::getInstance().archiveDataPath + Lamp::Core::lampConfig::getInstance().GameStringMap[Lamp::Core::lampConfig::getInstance().CurrentGame]; // Roi Danton many thanks again!
                    auto target = targetDIR / path.filename();
                    try
                    {
                        std::filesystem::create_directories(targetDIR);
                        std::filesystem::copy_file(path, target, std::filesystem::copy_options::overwrite_existing);
                        switch (Lamp::Core::lampConfig::getInstance().CurrentGame) {
                            case Lamp::Core::lampConfig::Game::BG3:
                                Lamp::Game::BG3::getInstance().registerArchive(target);
                                break;
                        }


                    }
                    catch (std::exception& e)
                    {
                        std::string ex  = e.what();
                        Lamp::Core::lampWarn::getInstance().log("File Drop Failed: " +ex, lampWarn::ERROR, true);
                    }
                }

            }


            if (ec)
            {
                Lamp::Core::lampWarn::getInstance().log("ec: " +ec.message(), lampWarn::ERROR, true);
            }
        }
    }


    void Core::lampFilesystem::extract(Lamp::Core::lampConfig::Game Game,const bit7z::BitInFormat & Type, Lamp::Core::lampMod::Mod * mod,
                                       std::string localExtractionPath) {
        std::string workingDir = getGameSpecificStoragePath(Game);
        try {
            bit7z::Bit7zLibrary lib{};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, Type};
            reader.test();
            reader.extract(workingDir+localExtractionPath);
        } catch (const bit7z::BitException &ex) {
            Lamp::Core::lampWarn::getInstance().log("Could not extract file: " +mod->ArchivePath, lampWarn::ERROR, true);

        }



    }

    void
    Core::lampFilesystem::extractSpecificFileType(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat &Type,
                                                  Lamp::Core::lampMod::Mod * mod, std::string extractionPath,
                                                  std::string extension) {
        std::string workingDir = getGameSpecificStoragePath(Game);
        Lamp::Core::lampWarn::getInstance().log("Extracting: " +mod->ArchivePath );
        Lamp::Core::lampWarn::getInstance().log("Extension: ." +extension );
        for (const auto& entry : std::filesystem::directory_iterator(workingDir+"/ext")) {
            std::filesystem::remove_all(entry.path());
        }

        try {
            bit7z::Bit7zLibrary lib{};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, Type};
            reader.test();
            reader.extract(workingDir+"/ext");
        } catch (const bit7z::BitException &ex) {
            Lamp::Core::lampWarn::getInstance().log("Could not extract file: " +mod->ArchivePath, lampWarn::ERROR, true);
        }


        for (const auto& entry : std::filesystem::directory_iterator(workingDir+"/ext")) {
            if(entry.is_directory()){
                for (const auto& subentry : std::filesystem::directory_iterator(entry)){
                    if(std::regex_match (subentry.path().filename().string(), std::regex("^.*\\.("+extension+")$") )){
                        try {
                            std::filesystem::rename(subentry.path(),workingDir+extractionPath + "/" + subentry.path().filename().string());
                        } catch (std::filesystem::filesystem_error& e) {
                            Lamp::Core::lampWarn::getInstance().log("Could not copy: " +mod->ArchivePath, lampWarn::ERROR, true);
                        }
                    }
                }
            }else if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.("+extension+")$") )){
                try {
                        std::filesystem::rename(entry.path(), workingDir+extractionPath + "/" + entry.path().filename().string());
                } catch (std::filesystem::filesystem_error& e) {
                    Lamp::Core::lampWarn::getInstance().log("Could not copy: " +mod->ArchivePath, lampWarn::ERROR, true);
                }
            }
        }
    }

    std::list<Lamp::Core::lampMod::Mod *>
    Core::lampFilesystem::loadModList(Lamp::Core::lampConfig::Game Game) {
        switch (Game) {
            case lampConfig::BG3:

                std::string xmlPath =saveDataPath+"BG3.mdf";
                std::list<Lamp::Core::lampMod::Mod *> newList;
                pugi::xml_document doc;
                if (doc.load_file(xmlPath.c_str())) {
                    // Find the Profile node
                    pugi::xml_node profileNode = doc.child("root").child("Profile");

                    // Iterate through the Mod nodes and deserialize them
                    for (pugi::xml_node modNode = profileNode.child("Mod"); modNode; modNode = modNode.next_sibling("Mod")) {
                        std::string archivePath = modNode.attribute("ArchivePath").as_string();
                        int modType = modNode.attribute("modType").as_int();
                        bool enabled = modNode.attribute("enabled").as_bool();

                        lampMod::Mod * temp = new lampMod::Mod{archivePath, modType, enabled};

                        newList.emplace_back(temp);
                    }

                    // Print the loaded Mod objects
                    for (const Lamp::Core::lampMod::Mod * mod : newList) {
                        Lamp::Core::lampWarn::getInstance().log("Loaded Mod: " +mod->ArchivePath+ "| Mod Type: " + std::to_string(mod->modType) +"| Enabled" + std::to_string(mod->enabled));
                    }

                    return newList;
                } else {
                    Lamp::Core::lampWarn::getInstance().log("Could not load modlist.", lampWarn::ERROR, true);
                }

            break;
        }


        return std::list<Lamp::Core::lampMod::Mod *>();
    }

    bool Core::lampFilesystem::saveModList(Lamp::Core::lampConfig::Game Game, std::list<Lamp::Core::lampMod::Mod *> ModList) {

        switch (Game) {
            case lampConfig::BG3:
                // Create an XML document
                pugi::xml_document doc;
                pugi::xml_node rootNode = doc.append_child("root");

                // Create a profile node to hold the Mod objects
                pugi::xml_node profileNode = rootNode.append_child("Profile");

                // Serialize the list of Mod objects
                for (const Lamp::Core::lampMod::Mod * mod : ModList) {
                    mod->serialize(profileNode);
                }

                // Save the XML to a file
                std::string xmlPath =saveDataPath+"BG3.mdf";
                doc.save_file(xmlPath.c_str());
                return true;
        }

        return false;
    }
//
    bool Core::lampFilesystem::saveKeyData(Lamp::Core::lampConfig::Game Game, std::string key, std::string data) {

        std::string game = Core::lampConfig::getInstance().GameStringMap[Game];
        Lamp::Core::lampWarn::getInstance().log("Saving "+game+":"+key+":"+data);
        std::string xmlPath =ConfigDataPath+"conf.mdf";

        // Load the existing XML file or create a new one if it doesn't exist
        pugi::xml_document doc;
        if (doc.load_file(xmlPath.c_str())) {
            // Check if the game node already exists, create it if it doesn't
            pugi::xml_node rootNode = doc.child("root");
            pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", game.c_str());
            if (!gameNode) {
                gameNode = rootNode.append_child("game");
                gameNode.append_attribute("name").set_value(game.c_str());
            }

            // Add a new key node with data as its child value
            pugi::xml_node keyNode = gameNode.append_child(key.c_str());
            keyNode.text().set(data.c_str());

            // Save the modified XML back to the file
            if (doc.save_file(xmlPath.c_str())) {
                return true; // Successfully saved the data
            }
        } else {
            // Create a new XML document if the file doesn't exist
            pugi::xml_node rootNode = doc.append_child("root");
            pugi::xml_node gameNode = rootNode.append_child("game");
            gameNode.append_attribute("name").set_value(game.c_str());

            // Add a new key node with data as its child value
            pugi::xml_node keyNode = gameNode.append_child(key.c_str());
            keyNode.text().set(data.c_str());

            // Save the new XML document to the file
            if (doc.save_file(xmlPath.c_str())) {
                return true; // Successfully saved the data
            }
        }
        Lamp::Core::lampWarn::getInstance().log("Failed to save "+game+":"+key+":"+":"+data, lampWarn::ERROR, true);
        return false; // Failed to save the data

    }

    std::string Core::lampFilesystem::loadKeyData(Lamp::Core::lampConfig::Game Game, std::string key) {
        std::string game = Core::lampConfig::getInstance().GameStringMap[Game];
        Lamp::Core::lampWarn::getInstance().log("Loading "+game+":"+key);
        std::string xmlPath =ConfigDataPath+"conf.mdf";

        pugi::xml_document doc;

        // Load the XML file
        if (doc.load_file(xmlPath.c_str())) {
            // Find the game node with the specified name
            pugi::xml_node rootNode = doc.child("root");
            pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", game.c_str());

            if (gameNode) {
                // Find the child node with the specified key
                pugi::xml_node keyNode = gameNode.child(key.c_str());

                if (keyNode) {
                    // Return the text value of the key node
                    return keyNode.text().as_string();
                }
            }
        }
        Lamp::Core::lampWarn::getInstance().log("Unable to load "+game+":"+key, lampWarn::ERROR, true);
        return ""; // Return an empty string if data is not found or there's an error


    }

    bool Core::lampFilesystem::init_config() {
        Lamp::Core::lampWarn::getInstance().log("Loading Lamp Config");
        load_conifg();
        return true;
    }

    void Core::lampFilesystem::load_conifg() {
        std::string SIM = Lamp::Core::lampFilesystem::getInstance().loadKeyData(lampConfig::UNK, "ShowIntroMenu");
        if(SIM != ""){
            lampConfig::getInstance().ShowIntroMenu = (SIM == "1");
        }

        std::string CG = Lamp::Core::lampFilesystem::getInstance().loadKeyData(lampConfig::UNK, "CurrentGame");
        if(CG != ""){
            lampConfig::getInstance().CurrentGame = (lampConfig::Game) std::stoi(CG);
        }
    }

    void Core::lampFilesystem::save_config() {
        saveKeyData(lampConfig::UNK, "ShowIntroMenu", std::to_string(lampConfig::getInstance().ShowIntroMenu));
        saveKeyData(lampConfig::UNK, "CurrentGame", std::to_string(lampConfig::getInstance().CurrentGame));
    }

    std::string Core::lampFilesystem::getGameSpecificStoragePath(Lamp::Core::lampConfig::Game Game) {
        return DeploymentDataPath + Core::lampConfig::getInstance().GameStringMap[Game];
    }
} // Lamp