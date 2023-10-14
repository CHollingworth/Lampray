//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPFS_H
#define LAMP_LAMPFS_H

#include <string>
#include <GLFW/glfw3.h>
#include <filesystem>
#include "../Base/lampBase.h"
#include "../Control/lampConfig.h"

namespace Lamp::Core::FS{
    typedef Base::lampTypes::lampString lampString;
    typedef Base::lampTypes::lampReturn lampReturn;

    class lampExtract{
    public:
        static lampReturn extract(const Base::lampMod::Mod * mod);
        static lampReturn moveModSpecificFileType(const Base::lampMod::Mod * mod, Lamp::Core::FS::lampString extension, lampString localExtractionPath);
        static lampReturn moveModSpecificFolder(const Base::lampMod::Mod * mod, Lamp::Core::FS::lampString extension, lampString localExtractionPath);

    private:
        static lampReturn copyFile(const std::filesystem::path& source, const std::filesystem::path& destination);
        static lampReturn copyDirectoryRecursively(const std::filesystem::path& source, const std::filesystem::path& destination);
        static lampReturn caseInsensitiveFolderCopyRecursive(const std::filesystem::path& sourceDirectory, const std::filesystem::path& destinationDirectory, const std::string& targetDirectoryName);
        static lampReturn copyFilesWithExtension(const std::filesystem::path& sourceDirectory, const std::filesystem::path& destinationDirectory, const std::string& extension) ;
        static bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2);

    };

    class lampIO{
    public:
        static std::vector<Base::lampMod::Mod *> loadModList(Lamp::Core::FS::lampString game, lampString profileName = "Default");
        static lampReturn saveModList(Lamp::Core::FS::lampString game, std::vector<Base::lampMod::Mod *> ModList, lampString profileName = "Default");
        static lampReturn saveKeyData(lampString key, lampString data, Lamp::Core::FS::lampString game);
        static lampReturn loadKeyData(lampString key, Lamp::Core::FS::lampString game);

        static lampReturn emptyFolder(lampString Path, lampString SpecificExtension = "");

         static void fileDrop(GLFWwindow* window, int count, const char** paths);

        static lampReturn downloadFile(lampString url, lampString output_filename);

    };

    class lampShare{
    public:
        static void exportProfile(std::string profileName);
        static void importProfile();


    private:

        static void InsertXMLintoFile(const std::string& filename, const pugi::xml_node& metadata);
        static void ExtractXMLfromFile(const std::string& filename, std::string& content);

        static bool compressFile(const std::string& inputFile, const std::string& outputFile);
        static bool decompressFile(const std::string& inputFile, const std::string& outputFile, std::uintmax_t volumeSize);

    };

    class lampUpdate{
    public:

        static lampUpdate& getInstance()
        {
            static lampUpdate instance;
            return instance;
        }

        lampUpdate(lampUpdate const&) = delete;
        void operator=(lampUpdate const&)  = delete;


        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
            size_t total_size = size * nmemb;
            output->append(static_cast<char*>(contents), total_size);
            return total_size;
        }

        std::string versionNumber = "1.0.8";

        void checkForUpdates();
        void getExpression();
    private:
        lampUpdate(){}
    };

    class lampTrack{
    private:

        enum trackedFileType{
            GameFile = 0,
            ModFile
        };

        struct trackedFile{
        public:
            std::string game;
            std::string gameLocation; // relative path from games root.
            std::string fileHash;
            trackedFileType type; // 0 - GameFile, 1 - ModFile
        };




        constexpr static unsigned int crc32(const char* data, size_t length) {
            unsigned int crc = 0xFFFFFFFF;
            for (size_t i = 0; i < length; ++i) {
                crc ^= static_cast<unsigned int>(data[i]);
                for (int j = 0; j < 8; ++j) {
                    crc = (crc >> 1) ^ (0xEDB88320 & (-int(crc & 1)));
                }
            }
            return ~crc;
        }

        static bool doesHashExist(const std::string& game, const std::string& hash);

        static bool doesFilenameExist(const std::string& game, const std::string& filename);

        static std::string getHash(std::filesystem::path FilePath);

        static void loadTracker(const std::string& filename);
        static void saveTracker(const std::string& filename, const std::vector<trackedFile>& files);

        static void recursiveCopyWithIgnore(const std::filesystem::path &source, const std::filesystem::path &destination, const std::vector<std::string> &ignoreFolders,std::filesystem::copy_options options, std::string game);
        static void copyExtensionWithFileTypeIgnore(const std::filesystem::path &sourceDir, const std::filesystem::path &destDir, std::string nonIgnoredExtension,std::filesystem::copy_options options, std::string game);
        static void copyAndOperate(const std::filesystem::path& source, const std::filesystem::path& destination, std::string game);
        static lampReturn trackOperation(const std::string& file, const std::string& destination, const std::string &game);

        static std::vector<trackedFile>& getTrackedFiles() {
            static std::vector<trackedFile> trackedFiles;
            return trackedFiles;
        }

        static void replaceGameFile(const std::filesystem::path& gameFilePath, const std::filesystem::path& gameFilesFolder);
        static void deleteModFile(const std::filesystem::path& modFilePath);

        public:




        struct handleFileDescriptor{
        public:
            enum operation{
                copyFile, // directly copy filePath to target.
                copyFolder, // copy a folder and its content recursively.
                copyFilesIgnoreExt, // copy a folder and its content recursively ignoring a folder with the name of extName.
                copyOnlyExt, // copy a folder and its content recursively with the extension of extName.
            };

            enum mode{
                direct,
                skipExisting,
                updateExisting
            };

            operation handlerOperation;
            mode handlerMode;
            std::filesystem::path filePath;
            std::filesystem::path target;
            std::filesystem::copy_options copyOperationOptions = std::filesystem::copy_options::recursive;
            std::string gameFullName;
            std::string extName;

            handleFileDescriptor(operation handlerOperation, mode handlerMode, std::filesystem::path filePath,
                                 std::filesystem::path target, std::string extName, std::string gameFullName)
                    : handlerOperation(handlerOperation), filePath(filePath), extName(extName), target(target),
                      handlerMode(handlerMode), gameFullName(gameFullName) {}
        };
        static Lamp::Core::lampReturn handleFile(handleFileDescriptor descriptor);
        //static void forcefulTrack();

        static void reset(std::string gameFullReadableName);

        };
}
#endif //LAMP_LAMPFS_H
