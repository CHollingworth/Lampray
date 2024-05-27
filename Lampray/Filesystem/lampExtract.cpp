//
// Created by charles on 27/09/23.
//

#include <regex>
#include "lampFS.h"
#include "bit7zlibrary.hpp"
#include "../Control/lampGames.h"
#include "bitarchivereader.hpp"
#include "bitexception.hpp"

Lamp::Core::FS::lampReturn Lamp::Core::FS::lampExtract::extract(const Base::lampMod::Mod *mod) {
    if(!mod->enabled) return false;
    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Lamp::Games::getInstance().currentGame->Ident().ReadableName;
    std::filesystem::create_directories(workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string());

    Base::lampLog::getInstance().log("Extracting File: " + mod->ArchivePath, Base::lampLog::LOG);
    if (std::regex_match((std::string)mod->ArchivePath, std::regex("^.*\\.(zip)$"))) {
        try {
            bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, bit7z::BitFormat::Zip};
            reader.test();
            reader.extract(workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string());
            return Base::lampLog::getInstance().pLog({1, "Extraction Successful. : "+ mod->ArchivePath},  Base::lampLog::LOG);
        } catch (const bit7z::BitException &ex) {
            return Base::lampLog::getInstance().pLog({0, "Could not extract file : "+ mod->ArchivePath},  Base::lampLog::ERROR,
                                                     true, Base::lampLog::LMP_EXTRACTIONFALED);
        }
    } else if (std::regex_match((std::string)mod->ArchivePath, std::regex("^.*\\.(rar)$"))) {

        auto rar_base_out = workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string();

        if(!system("which 7z > /dev/null 2>&1")) {
            auto sevenz_cmd_string = "7z e \"" + (std::string)mod->ArchivePath + "\" -o\""+rar_base_out+"\"  > /dev/null 2>&1";
            int result = system(sevenz_cmd_string.c_str());

            if(result == 0){
                return Base::lampLog::getInstance().pLog({1, "Extraction Successful with 7z : " + mod->ArchivePath},  Base::lampLog::LOG);
            } else{
                // do not return so we can attempt the next method as well
                Base::lampLog::getInstance().pLog({0, "Could not extract file with 7z : " + mod->ArchivePath},
                                                     Base::lampLog::ERROR, false, Base::lampLog::LMP_EXTRACTIONFALED);
            }
        }

        if(!system("which unrar > /dev/null 2>&1")){
            auto unrar_cmd_string = "unrar e \"" + (std::string)mod->ArchivePath + "\" \""+rar_base_out+"\"  > /dev/null 2>&1";
            int result = system(unrar_cmd_string.c_str());
            if(result == 0){
                return Base::lampLog::getInstance().pLog({1, "Extraction Successful with unrar : " + mod->ArchivePath},  Base::lampLog::LOG);
            } else{
                // do not return so we can attempt the next method as well
                Base::lampLog::getInstance().pLog({0, "Could not extract file with unrar : " + mod->ArchivePath},
                                                     Base::lampLog::ERROR, false, Base::lampLog::LMP_EXTRACTIONFALED);
            }
        }

        try {
            bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, bit7z::BitFormat::Rar5};
            reader.test();
            reader.extract(rar_base_out);
            return Base::lampLog::getInstance().pLog({1, "Extraction Successful. : "+ mod->ArchivePath},  Base::lampLog::LOG);
        } catch (const bit7z::BitException &ex) {
            // try using an alternate Rar format
            try {
                bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation};
                bit7z::BitArchiveReader reader{lib, mod->ArchivePath, bit7z::BitFormat::Rar};
                reader.test();
                reader.extract(rar_base_out);
                return Base::lampLog::getInstance().pLog({1, "Extraction Successful. : "+ mod->ArchivePath},  Base::lampLog::LOG);
            } catch (const bit7z::BitException &ex2) {
                return Base::lampLog::getInstance().pLog({0, "Could not extract file : "+ mod->ArchivePath + "\nMessages:\n" + ex.what() + "\n" + ex2.what()},
                                                        Base::lampLog::ERROR, true, Base::lampLog::LMP_EXTRACTIONFALED);
            }
        }
    } else if (std::regex_match((std::string)mod->ArchivePath, std::regex("^.*\\.(7z)$"))) {
        try {
            bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation};
            bit7z::BitArchiveReader reader{lib, mod->ArchivePath, bit7z::BitFormat::SevenZip};
            reader.test();
            reader.extract(workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string());
            return Base::lampLog::getInstance().pLog({1, "Extraction Successful. : "+ mod->ArchivePath},  Base::lampLog::LOG);
        } catch (const bit7z::BitException &ex) {
            return Base::lampLog::getInstance().pLog({0, "Could not extract file : "+ mod->ArchivePath},  Base::lampLog::ERROR,
                                                     true, Base::lampLog::LMP_EXTRACTIONFALED);
        }
    }
    else{
        return false;
    }
    return false;
}

Lamp::Core::lampReturn Lamp::Core::FS::lampExtract::moveModSpecificFileType(const Lamp::Core::Base::lampMod::Mod *mod,
                                                                            Lamp::Core::FS::lampString extension,
                                                                            Lamp::Core::lampString localExtractionPath) {
    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath +
                             Lamp::Games::getInstance().currentGame->Ident().ReadableName;
    for (const auto& entry : fs::recursive_directory_iterator(workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string())) {
        copyFilesWithExtension(
                workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string(),
                workingDir + "/" + localExtractionPath, extension);
    }

    return {1, ""};
}

Lamp::Core::lampReturn Lamp::Core::FS::lampExtract::moveModSpecificFolder(const Lamp::Core::Base::lampMod::Mod *mod,
                                                                          Lamp::Core::FS::lampString extension,
                                                                          Lamp::Core::lampString localExtractionPath) {
    std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + Lamp::Games::getInstance().currentGame->Ident().ReadableName;
    return caseInsensitiveFolderCopyRecursive(workingDir + "/ext/" + std::filesystem::path(mod->ArchivePath).filename().stem().string(), workingDir+"/"+localExtractionPath, extension);
}



Lamp::Core::lampReturn
Lamp::Core::FS::lampExtract::copyFile(const std::filesystem::path &source, const std::filesystem::path &destination) {
    std::ifstream sourceFile(source, std::ios::binary);
    std::ofstream destinationFile(destination, std::ios::binary);

    if (!sourceFile.is_open() || !destinationFile.is_open()) {
        return {false, "Failed to open source or destination file."};
    }

    destinationFile << sourceFile.rdbuf();
    return {true, "File copied successfully."};
}

Lamp::Core::lampReturn Lamp::Core::FS::lampExtract::copyDirectoryRecursively(const std::filesystem::path &source,
                                                           const std::filesystem::path &destination) {
    for (const auto& entry : fs::directory_iterator(source)) {
        fs::path destinationEntry = destination / entry.path().filename();

        if (fs::is_directory(entry)) {
            fs::create_directories(destinationEntry);
            lampReturn result = copyDirectoryRecursively(entry, destinationEntry);
            if (!result) {
                return result; // Propagate the error if copying failed
            }
        } else if (fs::is_regular_file(entry)) {
            lampReturn result = copyFile(entry.path(), destinationEntry);
            if (!result) {
                return result; // Propagate the error if copying failed
            }
            std::cout << "File copied to: " << destinationEntry << std::endl;
        }
    }

    return {true, "Directory copied successfully."};
}

Lamp::Core::lampReturn Lamp::Core::FS::lampExtract::caseInsensitiveFolderCopyRecursive(const std::filesystem::path &sourceDirectory,
                                                                     const std::filesystem::path &destinationDirectory,
                                                                     const std::string &targetDirectoryName) {
    for (const auto& entry : fs::recursive_directory_iterator(sourceDirectory)) {
        if (fs::is_directory(entry) &&
            caseInsensitiveStringCompare(entry.path().filename().string(), targetDirectoryName)) {
            lampReturn result = copyDirectoryRecursively(entry, destinationDirectory);
            if (!result) {
                return result; // Propagate the error if copying failed
            }
        }
    }

    return {true, "Folder copied successfully."};
}


Lamp::Core::lampReturn Lamp::Core::FS::lampExtract::copyFilesWithExtension(const std::filesystem::path &sourceDirectory,
                                                         const std::filesystem::path &destinationDirectory,
                                                         const std::string &extension) {
    for (const auto& entry : fs::recursive_directory_iterator(sourceDirectory)) {
        if (fs::is_regular_file(entry)) {
            std::string fileExtension = entry.path().extension().string();
            if (!fileExtension.empty() && caseInsensitiveStringCompare(fileExtension.substr(1), extension)) {
                fs::path destinationFile = destinationDirectory / entry.path().filename();
                lampReturn result = copyFile(entry.path(), destinationFile);
                if (!result) {
                    return result; // Propagate the error if copying failed
                }
                std::cout << "File copied to: " << destinationFile << std::endl;
            }
        }
    }

    return {true, "Files copied successfully."};
}


bool Lamp::Core::FS::lampExtract::caseInsensitiveStringCompare(const std::string &str1, const std::string &str2) {
    return std::equal(str1.begin(), str1.end(), str2.begin(), str2.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}



