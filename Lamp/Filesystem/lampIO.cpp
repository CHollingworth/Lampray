//
// Created by charles on 27/09/23.
//
#include <map>
#include <regex>
#include "lampFS.h"
#include "../Control/lampControl.h"


std::vector<Lamp::Core::Base::lampMod::Mod *> Lamp::Core::FS::lampIO::loadModList(Lamp::Core::FS::lampString game, Lamp::Core::FS::lampString profileNameS) {
    std::string profileName = (std::string)profileNameS;
    if((std::string)profileNameS == ""){
        profileName = "Default";
    }


    std::string xmlPath = Core::lampConfig::getInstance().saveDataPath + game + ".mdf";
    std::vector<Base::lampMod::Mod *> newList;
    pugi::xml_document doc;
    if (doc.load_file(xmlPath.c_str())) {
        pugi::xml_node profileNode = doc.child("root").find_child_by_attribute("Profile", "Name", profileName.c_str());
        std::cout << profileNode.attribute("Name").value() << "" << std::endl;
        if ((std::string)Lamp::Core::FS::lampString(profileNode.attribute("Name").value()) == (std::string)Lamp::Core::FS::lampString(profileName)){
            for (pugi::xml_node modNode = profileNode.child("Mod"); modNode; modNode = modNode.next_sibling(
                    "Mod")) {
                std::string archivePath = modNode.attribute("ArchivePath").as_string();
                int modType = modNode.attribute("modType").as_int();
                bool enabled = modNode.attribute("enabled").as_bool();

                Base::lampMod::Mod *temp = new Base::lampMod::Mod{archivePath, modType, enabled};

                if (modNode.attribute("timeOfUpdate")) {
                    temp->timeOfUpdate = modNode.attribute("timeOfUpdate").as_string();
                    if (temp->timeOfUpdate == "Unknown") {
                        temp->timeOfUpdate = lampControl::getInstance().getFormattedTimeAndDate();
                    }

                } else {
                    temp->timeOfUpdate = lampControl::getInstance().getFormattedTimeAndDate();
                }
                newList.emplace_back(temp);
            }
        }

        for (const Base::lampMod::Mod *mod: newList) {
            Base::lampLog::getInstance().log(
                    "Loaded Mod: " + mod->ArchivePath + "| Mod Type: " + std::to_string(mod->modType) +
                    "| Enabled :" + std::to_string(mod->enabled));
        }

        return newList;
    } else {
        Base::lampLog::getInstance().log("Could not load modlist.", Base::lampLog::ERROR, false,
                                         Base::lampLog::LMP_MODLISTSAVEFAILED);
    }


    return std::vector<Base::lampMod::Mod *>();
}

Lamp::Core::FS::lampReturn
Lamp::Core::FS::lampIO::saveModList(Lamp::Core::FS::lampString game, std::vector<Base::lampMod::Mod *> ModList, Lamp::Core::FS::lampString profileName) {

    if (Lamp::Games::getInstance().currentProfile != "Default") {
        profileName = Lamp::Games::getInstance().currentProfile;
    }
    std::string xmlPath = Core::lampConfig::getInstance().saveDataPath + game + ".mdf";
    pugi::xml_document doc;
    doc.load_file(xmlPath.c_str());
    pugi::xml_node rootNode = doc.child("root");

    if (!rootNode) {
        rootNode = doc.append_child("root");
    }

    pugi::xml_node profileNode = rootNode.find_child_by_attribute("Profile", "Name", profileName);

    if (!profileNode) {
        // If the Profile node doesn't exist, create it.
        pugi::xml_node profileNewNode = rootNode.append_child("Profile");
        profileNewNode.append_attribute("Name").set_value(profileName);

        for (const Base::lampMod::Mod *mod : ModList) {
            mod->serialize(profileNewNode);
        }
    } else {
        // Profile node with the same name exists, you can update it if needed.

        profileNode.remove_children();

        for (const Base::lampMod::Mod *mod : ModList) {
            mod->serialize(profileNode);
        }
    }

    if (doc.save_file(xmlPath.c_str())) {
        return true;
    } else {
        std::cerr << "Failed to save XML file." << std::endl;
        return false;
    }
}

Lamp::Core::FS::lampReturn
Lamp::Core::FS::lampIO::saveKeyData(Lamp::Core::FS::lampString key, Lamp::Core::FS::lampString data, Lamp::Core::FS::lampString game) {
    std::string actual = game;
    if((std::string)game == "") {
        actual = (std::string)Lamp::Games::getInstance().currentGame->Ident().ShortHand;;
    }
    Base::lampLog::getInstance().log("Saving " + actual + ":" + key + ":" + data);
    std::string xmlPath = Core::lampConfig::getInstance().ConfigDataPath + "conf.mdf";

    pugi::xml_document doc;
    if (doc.load_file(xmlPath.c_str())) {
        pugi::xml_node rootNode = doc.child("root");
        pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", actual.c_str());
        if (!gameNode) {
            gameNode = rootNode.append_child("game");
            gameNode.append_attribute("name").set_value(actual.c_str());
        }

        pugi::xml_node keyNode = gameNode.child(key);
        if (keyNode) {
            gameNode.remove_child(key);
        }

        keyNode = gameNode.append_child(key);
        keyNode.text().set(data);

        if (doc.save_file(xmlPath.c_str())) {
            return Lamp::Core::FS::lampReturn(1);
        }
    } else {
        pugi::xml_node rootNode = doc.append_child("root");
        pugi::xml_node gameNode = rootNode.append_child("game");
        gameNode.append_attribute("name").set_value(actual.c_str());

        pugi::xml_node keyNode = gameNode.append_child(key);
        keyNode.text().set(data);

        if (doc.save_file(xmlPath.c_str())) {
            return Lamp::Core::FS::lampReturn(1);
        }
    }
    Base::lampLog::getInstance().log("Failed to save " + game + ":" + key + ":" + ":" + data,
                                     Base::lampLog::ERROR, true, Base::lampLog::LMP_KEYSAVEFAILED);
    return Lamp::Core::FS::lampReturn(0);




}



Lamp::Core::FS::lampReturn Lamp::Core::FS::lampIO::loadKeyData(Lamp::Core::FS::lampString key, Lamp::Core::FS::lampString game) {
    std::string actual = game;
    if((std::string)game == "") {
        actual = (std::string)Lamp::Games::getInstance().currentGame->Ident().ShortHand;;
    }
    Base::lampLog::lampLog::getInstance().log("Loading " + actual + ":" + key);
    std::string xmlPath = Core::lampConfig::getInstance().ConfigDataPath + "conf.mdf";

    pugi::xml_document doc;

    if (doc.load_file(xmlPath.c_str())) {
        pugi::xml_node rootNode = doc.child("root");
        pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", actual.c_str());

        if (gameNode) {
            pugi::xml_node keyNode = gameNode.child(key);

            if (keyNode) {
                return Lamp::Core::FS::lampReturn(1, keyNode.text().as_string());
            }
        }
    }
    Base::lampLog::getInstance().log("Unable to load " + actual + ":" + key, Base::lampLog::WARNING, false,
                                     Base::lampLog::LMP_KEYLOADFAILED);
    return Lamp::Core::FS::lampReturn(0, "");

}


void Lamp::Core::FS::lampIO::fileDrop(const char *inputPath) {
        // Thank you! Roi Danton on stackoverflow for this clean code.
        std::string pth = inputPath;
        Base::lampLog::getInstance().log("File Drop Detected: " + pth);
        std::filesystem::path path(inputPath);
        std::error_code ec;
        if (std::filesystem::is_regular_file(path, ec)) {
            if (std::regex_match(path.filename().string(), std::regex("^.*\\.(zip|rar|7z)$"))) {
                std::filesystem::path targetDIR = Core::lampConfig::getInstance().archiveDataPath +
                        Lamp::Games::getInstance().currentGame->Ident().ReadableName; // Roi Danton many thanks again!
                auto target = targetDIR / path.filename();
                try {
                    std::filesystem::create_directories(targetDIR);
                    if(!std::filesystem::exists(target)){
                        std::filesystem::copy_file(path, target, std::filesystem::copy_options::overwrite_existing);
                    }
                    Lamp::Games::getInstance().currentGame->registerArchive(target.string());

                }
                catch (std::exception &e) {
                    std::string ex = e.what();
                    Base::lampLog::getInstance().log("File Drop Failed: " + ex, Base::lampLog::ERROR, true,
                                                     Base::lampLog::LMP_NOFILEDROP);
                }
            }

        }


        if (ec) {
            Base::lampLog::getInstance().log("ec: " + ec.message(), Base::lampLog::ERROR, true,
                                             Base::lampLog::LMP_NOFILEDROP);
        }
}


Lamp::Core::FS::lampReturn
Lamp::Core::FS::lampIO::downloadFile(Lamp::Core::FS::lampString url, Lamp::Core::FS::lampString output_filename) {
    return Lamp::Core::FS::lampReturn(0, Base::lampTypes::lampString());
}

Lamp::Core::lampReturn Lamp::Core::FS::lampIO::emptyFolder(Lamp::Core::lampString Path, Lamp::Core::lampString SpecificExtension) {
    std::filesystem::create_directories(Path);
    try {
        if (SpecificExtension != "") {
            for (const auto &entry: std::filesystem::directory_iterator(std::filesystem::path(Path))) {
                std::filesystem::remove_all(entry.path());
            }
        }

        for (const auto &entry: std::filesystem::directory_iterator(std::filesystem::path(Path))) {
            if (std::regex_match(entry.path().filename().string(), std::regex("^.*\\.(" + SpecificExtension + ")$"))) {
                std::filesystem::remove_all(entry.path());
            }
        }
        return Lamp::Core::FS::lampReturn(1, Base::lampTypes::lampString());
    }catch(std::exception e){
        return Lamp::Core::FS::lampReturn(0, Base::lampTypes::lampString());
    }

}


