//
// Created by charles on 11/09/23.
//

#include <iostream>
#include <filesystem>
#include <regex>
#include <fstream>
#include "lampFilesystem.h"
#include "game-data/BG3/BG3.h"
#include "game-data/gameControl.h"
#include "lampWarn.h"
#include "curl/curl.h"
#include "game-data/FO4/FO4.h"

namespace Lamp {

    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t total_size = size * nmemb;
        std::ofstream *output_stream = (std::ofstream *) userp;
        output_stream->write((char *) contents, total_size);
        return total_size;
    }


    bool Core::lampFilesystem::init() {
        Lamp::Core::lampWarn::getInstance().log("Initializing Lamp FileSystem");
        try {
            std::filesystem::create_directories(saveDataPath);
            std::filesystem::create_directories(archiveDataPath);
            std::filesystem::create_directories(ConfigDataPath);
            std::filesystem::create_directories(DeploymentDataPath);
        } catch (std::exception ex) {
            Lamp::Core::lampWarn::getInstance().log("Could not create base directories", lampWarn::ERROR,
                                                    lampWarn::LMP_NODIRCREATION);
        }

        std::map<Lamp::Core::lampConfig::Game, std::string>::iterator it;
        Lamp::Core::lampWarn::getInstance().log("Creating Game directories");
        for (it = Core::lampConfig::getInstance().GameStringMap.begin();
             it != Core::lampConfig::getInstance().GameStringMap.end(); it++) {
            std::filesystem::create_directories(DeploymentDataPath + it->second);
            Lamp::Core::lampWarn::getInstance().log(it->second);
        }
        Lamp::Core::lampWarn::getInstance().log("Finished creating Game directories");

        std::filesystem::path f{"/usr/libexec/p7zip/7z.so"};
        if (std::filesystem::exists(f)) {
            bit7zLibaryLocation = "/usr/libexec/p7zip/7z.so";
        } else if (exists(std::filesystem::path{"/usr/lib/p7zip/7z.so"})) {
            bit7zLibaryLocation = "/usr/lib/p7zip/7z.so";
        } else if (exists(std::filesystem::path{"/usr/libexec/7z.so"})) {
            bit7zLibaryLocation = "/usr/libexec/7z.so";
        } else {
            Lamp::Core::lampWarn::getInstance().log("Fatal. Cannot locate 7z.so", lampWarn::ERROR, true,
                                                    lampWarn::LMP_NO7ZP);
        }

//        Lamp::Core::lampWarn::getInstance().log("Downloading QuickBMS");
//        const std::string url = "http://aluigi.altervista.org/papers/quickbms_linux.zip";
//        if(!fs::exists("quickbms_4gb_files") && !fs::is_regular_file("quickbms_4gb_files")) {
//            if (downloadFile(url, "quckbms.zip")) {
//                bit7z::Bit7zLibrary lib{bit7zLibaryLocation};
//                bit7z::BitArchiveReader reader{lib, "quckbms.zip", bit7z::BitFormat::Zip};
//                reader.test();
//                reader.extract("");
//
//                const char* command = "chmod +x quickbms_4gb_files";
//                int returnCode = std::system(command);
//
//                if (returnCode == 0) {
//                    std::cout << "Command executed successfully." << std::endl;
//                } else {
//                    std::cerr << "Command failed with return code: " << returnCode << std::endl;
//                }
//            }
//        }




        return true;
    }

    bool Core::lampFilesystem::downloadFile(const std::string &url, const std::string &output_filename) {
        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to initialize libcurl" << std::endl;
            return false;
        }

        std::ofstream output_file(output_filename, std::ios::binary);
        if (!output_file.is_open()) {
            std::cerr << "Failed to open the output file for writing" << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "LAMP");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code == 302) {
                char *new_url;
                curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &new_url);
                if (new_url) {
                    std::cout << "Redirected to: " << new_url << std::endl;
                    curl_easy_cleanup(curl);
                    output_file.close();
                    return downloadFile(new_url, output_filename);
                }
            }
        }

        curl_easy_cleanup(curl);
        output_file.close();

        if (res != CURLE_OK) {
            std::cerr << "libcurl download failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        return true;
    }

    void Core::lampFilesystem::fileDrop(GLFWwindow *window, int count, const char **paths) {
        if (Lamp::Core::lampConfig::getInstance().CurrentGame == Lamp::Core::lampConfig::Game::UNK) {
            return;
        }
        int i;
        for (i = 0; i < count; i++) {
            // Thank you! Roi Danton on stackoverflow for this clean code.
            std::string pth = paths[i];
            Lamp::Core::lampWarn::getInstance().log("File Drop Detected: " + pth);
            std::filesystem::path path(paths[i]);
            std::error_code ec;
            if (std::filesystem::is_regular_file(path, ec)) {
                if (std::regex_match(path.filename().string(), std::regex("^.*\\.(zip|rar|7z)$"))) {
                    std::filesystem::path targetDIR = Core::lampFilesystem::getInstance().archiveDataPath +
                                                      Lamp::Core::lampConfig::getInstance().GameStringMap[Lamp::Core::lampConfig::getInstance().CurrentGame]; // Roi Danton many thanks again!
                    auto target = targetDIR / path.filename();
                    try {
                        std::filesystem::create_directories(targetDIR);
                        std::filesystem::copy_file(path, target, std::filesystem::copy_options::overwrite_existing);
                        switch (Lamp::Core::lampConfig::getInstance().CurrentGame) {
                            case Lamp::Core::lampConfig::Game::BG3:
                                Lamp::Game::BG3::getInstance().registerArchive(target);
                                break;
                            case Lamp::Core::lampConfig::Game::FO4:
                                Lamp::Game::FO4::getInstance().registerArchive(target);
                                break;
                        }


                    }
                    catch (std::exception &e) {
                        std::string ex = e.what();
                        Lamp::Core::lampWarn::getInstance().log("File Drop Failed: " + ex, lampWarn::ERROR, true,
                                                                lampWarn::LMP_NOFILEDROP);
                    }
                }

            }


            if (ec) {
                Lamp::Core::lampWarn::getInstance().log("ec: " + ec.message(), lampWarn::ERROR, true,
                                                        lampWarn::LMP_NOFILEDROP);
            }
        }
    }


    void Core::lampFilesystem::extract(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat &Type,
                                       Lamp::Core::lampMod::Mod *mod,
                                       std::string localExtractionPath) {
        std::thread([&] {
            std::string workingDir = getGameSpecificStoragePath(Game);
            std::filesystem::create_directories(workingDir + localExtractionPath);
            try {
                bit7z::Bit7zLibrary lib{bit7zLibaryLocation};
                bit7z::BitArchiveReader reader{lib, mod->ArchivePath, Type};
                reader.test();
                reader.extract(workingDir + localExtractionPath);
            } catch (const bit7z::BitException &ex) {
                Lamp::Core::lampWarn::getInstance().log("Could not extract file: " + mod->ArchivePath, lampWarn::ERROR,
                                                        true, lampWarn::LMP_EXTRACTIONFALED);

            }
        }).detach();
    }

    std::vector<std::string>
    Core::lampFilesystem::extractSpecificFileType(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat &Type,
                                                  Lamp::Core::lampMod::Mod *mod, std::string extractionPath,
                                                  std::string extension, bool cleanup) {

        std::vector<std::string> xx;

        std::string workingDir = getGameSpecificStoragePath(Game);
        std::filesystem::create_directories(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/");
        Lamp::Core::lampWarn::getInstance().log("Extracting: " + mod->ArchivePath);
        Lamp::Core::lampWarn::getInstance().log("Extension: ." + extension);
        for (const auto &entry: std::filesystem::directory_iterator(
                workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/")) {
            std::filesystem::remove_all(entry.path());
        }

        try {
            bit7z::Bit7zLibrary lib{bit7zLibaryLocation};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, Type};
            reader.test();
            reader.extract(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/");
        } catch (const bit7z::BitException &ex) {
            Lamp::Core::lampWarn::getInstance().log("Could not extract file: " + mod->ArchivePath, lampWarn::ERROR,
                                                    true, lampWarn::LMP_EXTRACTIONFALED);
        }


        for (const auto &entry: std::filesystem::directory_iterator(
                workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/")) {
            if (entry.is_directory()) {
                for (const auto &subentry: std::filesystem::directory_iterator(entry)) {
                    if (std::regex_match(subentry.path().filename().string(),
                                         std::regex("^.*\\.(" + extension + ")$"))) {
                        try {
                            std::filesystem::rename(subentry.path(), workingDir + extractionPath + "/" +
                                                                     subentry.path().filename().string());
                            xx.push_back(workingDir + extractionPath + "/" + subentry.path().filename().string());
                        } catch (std::filesystem::filesystem_error &e) {
                            Lamp::Core::lampWarn::getInstance().log("Could not copy: " + mod->ArchivePath,
                                                                    lampWarn::ERROR, true, lampWarn::LMP_COPYFAILED);
                        }
                    }
                }
            } else if (std::regex_match(entry.path().filename().string(), std::regex("^.*\\.(" + extension + ")$"))) {
                try {
                    std::filesystem::rename(entry.path(),
                                            workingDir + extractionPath + "/" + entry.path().filename().string());
                    xx.push_back(workingDir + extractionPath + "/" + entry.path().filename().string());
                } catch (std::filesystem::filesystem_error &e) {
                    Lamp::Core::lampWarn::getInstance().log("Could not copy: " + mod->ArchivePath, lampWarn::ERROR,
                                                            true, lampWarn::LMP_COPYFAILED);
                }
            }
        }

        if (cleanup) {
            fs::path dirit(workingDir + extractionPath);

            try {
                for (const auto &entry: fs::directory_iterator(dirit)) {
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
                                                    workingDir + extractionPath + "/" + temp.filename().string());
                            std::cout << "File with backslash in name: " << fileName << std::endl;
                        }
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }

        }


        return xx;
    }

    void Core::lampFilesystem::extractSpecificFolder(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat &Type,
                                                     Lamp::Core::lampMod::Mod *mod, std::string localExtractionPath) {
        std::string workingDir = getGameSpecificStoragePath(Game);


        Lamp::Core::lampWarn::getInstance().log("Extracting: " + mod->ArchivePath);
        Lamp::Core::lampWarn::getInstance().log("Folder: ." + localExtractionPath);

        std::filesystem::create_directories(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/");

        for (const auto &entry: std::filesystem::directory_iterator(
                workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/")) {
            std::filesystem::remove_all(entry.path());
        }

        try {
            bit7z::Bit7zLibrary lib{bit7zLibaryLocation};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, Type};
            reader.test();
            reader.extract(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/");
        } catch (const bit7z::BitException &ex) {
            Lamp::Core::lampWarn::getInstance().log("Could not extract file: " + mod->ArchivePath, lampWarn::ERROR,
                                                    true, lampWarn::LMP_EXTRACTIONFALED);
        }

        fs::path dirit(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/");
        try {
            for (const auto &entry: fs::directory_iterator(dirit)) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();
                    if (fileName.find('\\') != std::string::npos) {

                        std::string inputString = fileName;

                        size_t found = inputString.find('\\');
                        while (found != std::string::npos) {
                            inputString.replace(found, 1, "/");
                            found = inputString.find('\\', found + 1);
                        }


                        std::filesystem::path inputPath(
                                workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/" + inputString);
                        std::filesystem::create_directories(inputPath.parent_path());
                        std::filesystem::rename(entry.path(), inputPath);
                        std::cout << "File with backslash in name: " << fileName << std::endl;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        fs::path resultPath;
        if (findFirstMatchingDirectory(workingDir + "/ext/" + fs::path(mod->ArchivePath).stem().string() + "/",
                                       localExtractionPath, resultPath)) {
            std::cout << "Found matching directory: " << resultPath << std::endl;
            fs::copy(resultPath, workingDir + "/" + localExtractionPath, fs::copy_options::recursive);

        } else {
            std::cout << "Directory not found." << std::endl;
        }


    }

    std::list<Lamp::Core::lampMod::Mod *>
    Core::lampFilesystem::loadModList(Lamp::Core::lampConfig::Game Game) {
        std::string xmlPath = saveDataPath + lampConfig::getInstance().GameShorthandMap[Game] + ".mdf";
        std::list<Lamp::Core::lampMod::Mod *> newList;
        pugi::xml_document doc;
        if (doc.load_file(xmlPath.c_str())) {
            pugi::xml_node profileNode = doc.child("root").child("Profile");

            for (pugi::xml_node modNode = profileNode.child("Mod"); modNode; modNode = modNode.next_sibling("Mod")) {
                std::string archivePath = modNode.attribute("ArchivePath").as_string();
                int modType = modNode.attribute("modType").as_int();
                bool enabled = modNode.attribute("enabled").as_bool();

                lampMod::Mod *temp = new lampMod::Mod{archivePath, modType, enabled};

                if (modNode.attribute("timeOfUpdate")) {
                    temp->timeOfUpdate = modNode.attribute("timeOfUpdate").as_string();
                    if (temp->timeOfUpdate == "Unknown") {
                        std::time_t unixTimestamp = std::time(nullptr);
                        std::tm timeInfo;
                        localtime_r(&unixTimestamp, &timeInfo);
                        std::ostringstream oss;
                        oss << std::setfill('0');
                        oss << std::setw(2) << timeInfo.tm_mday << '/' << std::setw(2) << (timeInfo.tm_mon + 1) << '/'
                            << (timeInfo.tm_year + 1900) << ' ';
                        oss << std::setw(2) << timeInfo.tm_hour << ':' << std::setw(2) << timeInfo.tm_min;
                        std::string formattedTime = oss.str();

                        temp->timeOfUpdate = formattedTime;
                    }

                } else {
                    std::time_t unixTimestamp = std::time(nullptr);
                    std::tm timeInfo;
                    localtime_r(&unixTimestamp, &timeInfo);
                    std::ostringstream oss;
                    oss << std::setfill('0');
                    oss << std::setw(2) << timeInfo.tm_mday << '/' << std::setw(2) << (timeInfo.tm_mon + 1) << '/'
                        << (timeInfo.tm_year + 1900) << ' ';
                    oss << std::setw(2) << timeInfo.tm_hour << ':' << std::setw(2) << timeInfo.tm_min;
                    std::string formattedTime = oss.str();

                    temp->timeOfUpdate = formattedTime;
                }


                newList.emplace_back(temp);
            }


            for (const Lamp::Core::lampMod::Mod *mod: newList) {
                Lamp::Core::lampWarn::getInstance().log(
                        "Loaded Mod: " + mod->ArchivePath + "| Mod Type: " + std::to_string(mod->modType) +
                        "| Enabled" + std::to_string(mod->enabled));
            }

            return newList;
        } else {
            Lamp::Core::lampWarn::getInstance().log("Could not load modlist.", lampWarn::ERROR, false,
                                                    lampWarn::LMP_MODLISTSAVEFAILED);
        }


        return std::list<Lamp::Core::lampMod::Mod *>();
    }

    bool Core::lampFilesystem::saveModList(Lamp::Core::lampConfig::Game Game,
                                           std::list<Lamp::Core::lampMod::Mod *> ModList) {
        pugi::xml_document doc;
        pugi::xml_node rootNode = doc.append_child("root");
        pugi::xml_node profileNode = rootNode.append_child("Profile");
        for (const Lamp::Core::lampMod::Mod *mod: ModList) {
            mod->serialize(profileNode);
        }

        std::string xmlPath = saveDataPath + lampConfig::getInstance().GameShorthandMap[Game] + ".mdf";
        doc.save_file(xmlPath.c_str());
        return true;
    }

    bool Core::lampFilesystem::saveKeyData(Lamp::Core::lampConfig::Game Game, std::string key, std::string data) {

        std::string game = Core::lampConfig::getInstance().GameStringMap[Game];
        Lamp::Core::lampWarn::getInstance().log("Saving " + game + ":" + key + ":" + data);
        std::string xmlPath = ConfigDataPath + "conf.mdf";

        pugi::xml_document doc;
        if (doc.load_file(xmlPath.c_str())) {
            pugi::xml_node rootNode = doc.child("root");
            pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", game.c_str());
            if (!gameNode) {
                gameNode = rootNode.append_child("game");
                gameNode.append_attribute("name").set_value(game.c_str());
            }

            pugi::xml_node keyNode = gameNode.child(key.c_str());
            if (keyNode) {
                gameNode.remove_child(key.c_str());
            }

            keyNode = gameNode.append_child(key.c_str());
            keyNode.text().set(data.c_str());

            if (doc.save_file(xmlPath.c_str())) {
                return true;
            }
        } else {
            pugi::xml_node rootNode = doc.append_child("root");
            pugi::xml_node gameNode = rootNode.append_child("game");
            gameNode.append_attribute("name").set_value(game.c_str());

            pugi::xml_node keyNode = gameNode.append_child(key.c_str());
            keyNode.text().set(data.c_str());

            if (doc.save_file(xmlPath.c_str())) {
                return true;
            }
        }
        Lamp::Core::lampWarn::getInstance().log("Failed to save " + game + ":" + key + ":" + ":" + data,
                                                lampWarn::ERROR, true, lampWarn::LMP_KEYSAVEFAILED);
        return false;

    }

    std::string Core::lampFilesystem::loadKeyData(Lamp::Core::lampConfig::Game Game, std::string key) {
        std::string game = Core::lampConfig::getInstance().GameStringMap[Game];
        Lamp::Core::lampWarn::getInstance().log("Loading " + game + ":" + key);
        std::string xmlPath = ConfigDataPath + "conf.mdf";

        pugi::xml_document doc;

        if (doc.load_file(xmlPath.c_str())) {
            pugi::xml_node rootNode = doc.child("root");
            pugi::xml_node gameNode = rootNode.find_child_by_attribute("game", "name", game.c_str());

            if (gameNode) {
                pugi::xml_node keyNode = gameNode.child(key.c_str());

                if (keyNode) {
                    return keyNode.text().as_string();
                }
            }
        }
        Lamp::Core::lampWarn::getInstance().log("Unable to load " + game + ":" + key, lampWarn::ERROR, true,
                                                lampWarn::LMP_KEYLOADFAILED);
        return "";


    }

    bool Core::lampFilesystem::init_config() {
        Lamp::Core::lampWarn::getInstance().log("Loading Lamp Config");
        load_conifg();
        return true;
    }

    void Core::lampFilesystem::load_conifg() {
        std::string SIM = Lamp::Core::lampFilesystem::getInstance().loadKeyData(lampConfig::UNK, "ShowIntroMenu");
        if (SIM != "") {
            lampConfig::getInstance().ShowIntroMenu = (SIM == "1");
        }

        std::string CG = Lamp::Core::lampFilesystem::getInstance().loadKeyData(lampConfig::UNK, "CurrentGame");
        if (CG != "") {
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


    bool Core::lampFilesystem::findFirstMatchingDirectory(const fs::path &currentDir, const std::string &targetName,
                                                          fs::path &resultPath) {
        try {
            for (const auto &entry: fs::directory_iterator(currentDir)) {
                if (fs::is_directory(entry)) {
                    std::string targetNameLower = targetName;
                    std::transform(targetNameLower.begin(), targetNameLower.end(), targetNameLower.begin(), ::tolower);
                    std::string filenameLower = entry.path().filename();
                    std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);

                    if (filenameLower == targetNameLower) {
                        resultPath = entry.path();
                        return true;
                    }
                    if (fs::is_directory(entry.path())) {
                        if (findFirstMatchingDirectory(entry, targetName, resultPath)) {
                            return true;
                        }
                    }
                }
            }
        } catch (const std::exception &ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
        return false;
    }

    void Core::lampFilesystem::recursiveCopyWithIgnore(const fs::path &source, const fs::path &destination,
                                                       const std::vector<std::string> &ignoreFolders) {
        for (const auto &entry: fs::directory_iterator(source)) {
            if (entry.is_directory()) {
                if (std::find(ignoreFolders.begin(), ignoreFolders.end(), entry.path().filename()) ==
                    ignoreFolders.end()) {
                    fs::create_directories(destination / entry.path().filename());
                    recursiveCopyWithIgnore(entry.path(), destination / entry.path().filename(), ignoreFolders);
                }
            } else if (entry.is_regular_file()) {
                try {
                    fs::copy_file(entry.path(), destination / entry.path().filename(),
                                  fs::copy_options::update_existing);
                }
                catch (const fs::filesystem_error &e) {
                    std::cerr << "Error copying file: " << e.what() << std::endl;
                }
            }
        }

    }

    void Core::lampFilesystem::copyDllWithConfigIgnore(const fs::path &sourceDir, const fs::path &destDir) {
        for (const auto &entry: fs::directory_iterator(sourceDir)) {
            const fs::path &sourcePath = entry.path();
            const fs::path destPath = destDir / sourcePath.filename();

            if (fs::is_regular_file(sourcePath)) {
                copyFileWithDLLStrategy(sourcePath, destPath);
            } else if (fs::is_directory(sourcePath)) {
                fs::create_directory(destPath);
                copyDllWithConfigIgnore(sourcePath, destPath);
            }
        }
    }

    void Core::lampFilesystem::copyFileWithDLLStrategy(const fs::path &source, const fs::path &destination) {
        try {
            std::string extension = source.extension().string();
            if (extension == ".dll") {
                fs::copy_file(source, destination, fs::copy_options::update_existing);
            } else {
                fs::copy_file(source, destination, fs::copy_options::skip_existing);
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error copying file: " << e.what() << std::endl;
        }
    }

    std::string Core::lampFilesystem::getFormattedTimeAndDate() {
        std::time_t unixTimestamp = std::time(nullptr);
        std::tm timeInfo;
        localtime_r(&unixTimestamp, &timeInfo);
        std::ostringstream oss;
        oss << std::setfill('0');
        oss << std::setw(2) << timeInfo.tm_mday << '/' << std::setw(2) << (timeInfo.tm_mon + 1) << '/'
            << (timeInfo.tm_year + 1900) << ' ';
        oss << std::setw(2) << timeInfo.tm_hour << ':' << std::setw(2) << timeInfo.tm_min;
        return oss.str();
    }

} // Lamp