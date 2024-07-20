//
// Created by charles on 10/10/23.
//
#include <algorithm>
#include <filesystem>
#include "lampFS.h"
std::string Lamp::Core::FS::lampTrack::getHash(std::filesystem::path filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    unsigned int hash = 0xFFFFFFFF; // Initialize CRC32 with all 1s

    while (file.good()) {
        file.read(buffer, bufferSize);
        const size_t bytesRead = static_cast<size_t>(file.gcount());
        hash = crc32(buffer, bytesRead);
    }

    // Convert the numeric hash to a hexadecimal string
    std::stringstream stream;
    stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << hash;
    return stream.str();
}


bool Lamp::Core::FS::lampTrack::doesFilenameExist(const std::string &game, const std::string &filename) {
    for (const auto& file : getTrackedFiles()) {
        if (file.game == game && file.gameLocation == filename) {
            return true; // Found a match
        }
    }
    return false; // No match found
}

bool Lamp::Core::FS::lampTrack::doesHashExist(const std::string &game, const std::string &hash) {
    for (const auto& file : getTrackedFiles()) {
        if (file.game == game && file.fileHash == hash) {
            return true; // Found a match
        }
    }
    return false; // No match found
}


 void Lamp::Core::FS::lampTrack::saveTracker(const std::string& filename, const std::vector<trackedFile>& files) {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("Tracker");

    for (const auto& file : files) {
        pugi::xml_node fileNode = root.append_child("TrackedFile");
        fileNode.append_child("Game").text() = file.game.c_str();
        fileNode.append_child("GameLocation").text() = file.gameLocation.c_str();
        fileNode.append_child("FileHash").text() = file.fileHash.c_str();
        fileNode.append_child("Type").text() = static_cast<int>(file.type);
    }

    if (doc.save_file(filename.c_str())) {
       Lamp::Core::Base::lampLog::getInstance().log("Tracker data saved to " + filename, Base::lampLog::LOG);
    } else {
        Lamp::Core::Base::lampLog::getInstance().log("Error saving tracker data to " + filename, Lamp::Core::Base::lampLog::ERROR);
    }
}

void Lamp::Core::FS::lampTrack::loadTracker(const std::string& filename) {
    getTrackedFiles().clear();
    pugi::xml_document doc;
    if (doc.load_file(filename.c_str())) {
        pugi::xml_node root = doc.child("Tracker");
        getTrackedFiles().clear(); // Clear existing data

        for (pugi::xml_node fileNode = root.child("TrackedFile"); fileNode; fileNode = fileNode.next_sibling("TrackedFile")) {
            trackedFile file;
            file.game = fileNode.child_value("Game");
            file.gameLocation = fileNode.child_value("GameLocation");
            file.fileHash = fileNode.child_value("FileHash");
            file.type = static_cast<trackedFileType>(fileNode.child("Type").text().as_int());
            getTrackedFiles().push_back(file);
        }

        Lamp::Core::Base::lampLog::getInstance().log("Tracker data loaded from " + filename, Base::lampLog::LOG);
    } else {
        Lamp::Core::Base::lampLog::getInstance().log("Error loading tracker data from " + filename, Lamp::Core::Base::lampLog::ERROR);
    }
}

Lamp::Core::lampReturn
Lamp::Core::FS::lampTrack::handleFile(Lamp::Core::FS::lampTrack::handleFileDescriptor descriptor) {
    if(getTrackedFiles().empty()){
        loadTracker(lampConfig::getInstance().ConfigDataPath+"lampTrack.mdf");
    }

    switch (descriptor.handlerMode) {
        case handleFileDescriptor::direct:
            break;
        case handleFileDescriptor::skipExisting:
            descriptor.copyOperationOptions = descriptor.copyOperationOptions | std::filesystem::copy_options::skip_existing;
            break;
        case handleFileDescriptor::updateExisting:
            descriptor.copyOperationOptions = descriptor.copyOperationOptions | std::filesystem::copy_options::update_existing;
            break;
    }


    switch (descriptor.handlerOperation) {
        case handleFileDescriptor::copyFile:
            if(trackOperation(descriptor.filePath,descriptor.target,descriptor.gameFullName)) {
                std::filesystem::copy_file(descriptor.filePath,descriptor.target, descriptor.copyOperationOptions);
            }
            break;
        case handleFileDescriptor::copyFolder:
            copyAndOperate(descriptor.filePath, descriptor.target,descriptor.gameFullName);
            break;
        case handleFileDescriptor::copyFilesIgnoreExt:
            recursiveCopyWithIgnore(descriptor.filePath, descriptor.target,std::vector<std::string>{descriptor.extName},descriptor.copyOperationOptions,descriptor.gameFullName);
            break;
        case handleFileDescriptor::copyOnlyExt:
            std::filesystem::create_directories(descriptor.target);
            copyExtensionWithFileTypeIgnore(descriptor.filePath, descriptor.target, descriptor.extName, descriptor.copyOperationOptions,descriptor.gameFullName);
            break;
    }


    return {0,""};

}

void Lamp::Core::FS::lampTrack::recursiveCopyWithIgnore(const std::filesystem::path &source,
                                                        const std::filesystem::path &destination,
                                                        const std::vector<std::string> &ignoreFolders, std::filesystem::copy_options options, std::string game) {
    for (const auto &entry: std::filesystem::directory_iterator(source)) {
        if (entry.is_directory()) {
            if (std::find(ignoreFolders.begin(), ignoreFolders.end(), entry.path().filename()) ==
                ignoreFolders.end()) {
                std::filesystem::create_directories(destination / entry.path().filename());
                recursiveCopyWithIgnore(entry.path(), destination / entry.path().filename(), ignoreFolders, options, game);
            }
        } else if (entry.is_regular_file()) {
            try {



                if(trackOperation(entry.path(),destination / entry.path().filename(),game)) {
                    std::filesystem::copy_file(entry.path(), destination / entry.path().filename(),
                                               options);
                }
            }
            catch (const std::filesystem::filesystem_error &e) {
                Lamp::Core::Base::lampLog::getInstance().log("Error copying file: " + (std::string)e.what(), Lamp::Core::Base::lampLog::ERROR);
            }
        }
    }

}

void Lamp::Core::FS::lampTrack::copyExtensionWithFileTypeIgnore(const std::filesystem::path &sourceDir,
                                                                const std::filesystem::path &destDir,
                                                                std::string nonIgnoredExtension, std::filesystem::copy_options options, std::string game) {
    for (const auto &entry: std::filesystem::directory_iterator(sourceDir)) {
        const std::filesystem::path &sourcePath = entry.path();
        const std::filesystem::path destPath = destDir / sourcePath.filename();
        if (std::filesystem::is_regular_file(sourcePath)) {
            try {
                std::string extension = sourcePath.extension().string();
                if ((extension == nonIgnoredExtension) && trackOperation(sourcePath,destPath,game)) {
                    std::filesystem::copy_file(sourcePath, destPath, options);
                }
            } catch (const std::filesystem::filesystem_error &e) {
            }


        } else if (std::filesystem::is_directory(sourcePath)) {
            std::filesystem::create_directory(destPath);
            copyExtensionWithFileTypeIgnore(sourcePath, destPath, nonIgnoredExtension, options, game);
        }
    }
}

Lamp::Core::lampReturn
Lamp::Core::FS::lampTrack::trackOperation(const std::string &file, const std::string &destination, const std::string &game) {
    const std::filesystem::path &sourcePath = file;
    const std::filesystem::path destPath = destination;


    std::string fileHash = getHash(sourcePath);

    if (std::filesystem::exists(destPath)) {

        if(doesHashExist(game, fileHash) && !doesFilenameExist(game,destPath)){
            return Base::lampLog::getInstance().pLog({0, sourcePath.filename().string() + " hash exists but the file does not. Skipping."}, Base::lampLog::ERROR,true);
        }

        if(doesHashExist(game, fileHash) && doesFilenameExist(game,destPath)){
            return Base::lampLog::getInstance().pLog({0, sourcePath.filename().string() + " is up to date."});
        }

        if(!doesHashExist(game, fileHash) && doesFilenameExist(game,destPath)){
            for (auto& file : getTrackedFiles()) {
                if (file.game == game && file.gameLocation == destPath) {
                    if(file.type == trackedFileType::GameFile)
                    file.fileHash = fileHash;

                    return Base::lampLog::getInstance().pLog({1, sourcePath.filename().string() + " Was outdated updating hash and updating file."});
                }
            }
        }

        trackedFile newTrackedFile;
        newTrackedFile.game = game;
        newTrackedFile.fileHash = fileHash;
        newTrackedFile.gameLocation = destPath;
        newTrackedFile.type = trackedFileType::GameFile;

        getTrackedFiles().push_back(newTrackedFile);
        saveTracker(lampConfig::getInstance().ConfigDataPath+"lampTrack.mdf", getTrackedFiles());
        if(!std::filesystem::exists(lampConfig::getInstance().archiveDataPath+game+"/GameFiles/"+destPath.filename().string())) {
            std::filesystem::copy(destPath, lampConfig::getInstance().archiveDataPath + game + "/GameFiles/");
        }
        return Base::lampLog::getInstance().pLog({1, sourcePath.filename().string() + " tracking created."});

    } else {
        trackedFile newTrackedFile;
        newTrackedFile.game = game;
        newTrackedFile.fileHash = fileHash;
        newTrackedFile.gameLocation = destPath;
        newTrackedFile.type = trackedFileType::ModFile;
        getTrackedFiles().push_back(newTrackedFile);
        saveTracker(lampConfig::getInstance().ConfigDataPath+"lampTrack.mdf", getTrackedFiles());
        return Base::lampLog::getInstance().pLog({1, sourcePath.filename().string() + " ModFile created, game did not contain file."});
    }

}

void Lamp::Core::FS::lampTrack::copyAndOperate(const std::filesystem::path &source,
                                               const std::filesystem::path &destination, std::string game) {
    if (!std::filesystem::exists(source)) {
        Lamp::Core::Base::lampLog::getInstance().log("Source does not exist: " + (std::string)source, Lamp::Core::Base::lampLog::ERROR);
        return;
    }
    if (std::filesystem::is_directory(source)) {
        if (!std::filesystem::exists(destination)) {
            std::filesystem::create_directories(destination);
        }

        for (const auto& entry : std::filesystem::directory_iterator(source)) {
            const std::filesystem::path& sourceEntry = entry.path();
            const std::filesystem::path destinationEntry = destination / sourceEntry.filename();

            copyAndOperate(sourceEntry, destinationEntry, game);
        }
    } else if (std::filesystem::is_regular_file(source)) {
        if (trackOperation(source, destination, game)) {
            std::filesystem::copy_file(source, destination,
                                       std::filesystem::copy_options::update_existing);
        }
    }
}

void Lamp::Core::FS::lampTrack::reset(std::string gameFullReadableName) {
    auto temp = getTrackedFiles();
    for (const auto& file : temp) {
        if (file.game == gameFullReadableName) {
            if (file.type == trackedFileType::ModFile) {
                // Delete ModFile
                deleteModFile(file.gameLocation);
            } else if (file.type == trackedFileType::GameFile) {
                // Replace GameFile with counterpart from GameFiles folder
                replaceGameFile(file.gameLocation,
                                lampConfig::getInstance().archiveDataPath + gameFullReadableName + "/GameFiles/");
            }
        }
    }
    saveTracker(lampConfig::getInstance().ConfigDataPath+"lampTrack.mdf", getTrackedFiles());
}

void Lamp::Core::FS::lampTrack::replaceGameFile(const std::filesystem::path &gameFilePath,
                                                const std::filesystem::path &gameFilesFolder) {
    if (std::filesystem::exists(gameFilePath) && std::filesystem::is_regular_file(gameFilePath)) {
        std::filesystem::path gameFileName = gameFilePath.filename();
        std::filesystem::path gameFilesPath = gameFilesFolder / gameFileName;

        if (std::filesystem::exists(gameFilesPath) && std::filesystem::is_regular_file(gameFilesPath)) {
            //std::filesystem::rename(gameFilesPath, gameFilePath);
            std::filesystem::copy_file(gameFilesPath,gameFilePath, std::filesystem::copy_options::overwrite_existing);

            Lamp::Core::Base::lampLog::getInstance().log("GameFile replaced: " + (std::string)gameFilePath, Lamp::Core::Base::lampLog::LOG);
            // Remove the processed file from the trackedFiles vector
            getTrackedFiles().erase(
                    std::remove_if(getTrackedFiles().begin(), getTrackedFiles().end(),
                                   [&](const trackedFile& file) {
                                       return file.gameLocation == gameFilePath;
                                   }),
                    getTrackedFiles().end());


        } else {
            Lamp::Core::Base::lampLog::getInstance().log("GameFile counterpart not found: " + (std::string)gameFilesPath, Lamp::Core::Base::lampLog::ERROR);
        }
    } else {
        Lamp::Core::Base::lampLog::getInstance().log("GameFile not found or is not a regular file: " + (std::string)gameFilePath, Lamp::Core::Base::lampLog::ERROR);
    }
}

void Lamp::Core::FS::lampTrack::deleteModFile(const std::filesystem::path &modFilePath) {
    if (std::filesystem::exists(modFilePath) && std::filesystem::is_regular_file(modFilePath)) {
        std::filesystem::remove(modFilePath);

        getTrackedFiles().erase(
                std::remove_if(getTrackedFiles().begin(), getTrackedFiles().end(),
                               [&](const trackedFile& file) {
                                   return file.gameLocation == modFilePath;
                               }),
                getTrackedFiles().end());

        Lamp::Core::Base::lampLog::getInstance().log("ModFile deleted: " + (std::string)modFilePath, Lamp::Core::Base::lampLog::LOG);
    } else {
        Lamp::Core::Base::lampLog::getInstance().log("ModFile not found or is not a regular file: " + (std::string)modFilePath, Lamp::Core::Base::lampLog::LOG);
    }
}
