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

        static void recursiveCopyWithIgnore(const std::filesystem::path &source, const std::filesystem::path &destination, const std::vector<std::string> &ignoreFolders);
        static void copyExtensionWithConfigIgnore(const std::filesystem::path &sourceDir, const std::filesystem::path &destDir, std::string nonIgnoredExtension);
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
}
#endif //LAMP_LAMPFS_H
