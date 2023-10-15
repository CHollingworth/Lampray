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
    /**
     * @brief The lampExtract class provides utility functions for extracting and moving mod files.
     */
    class lampExtract {
    public:
        /**
         * @brief Extracts the specified mod archive.
         *
         * This function extracts the contents of a mod archive to a specified location.
         *
         * @param mod The mod to extract.
         * @return A lampReturn object indicating the extraction result.
         */
        static lampReturn extract(const Base::lampMod::Mod* mod);

        /**
         * @brief Moves mod files with a specific file extension.
         *
         * This function moves mod files with a specific file extension from the extraction directory
         * to a local extraction path.
         *
         * @param mod The mod containing files to move.
         * @param extension The file extension to filter files by.
         * @param localExtractionPath The destination path for the moved files.
         * @return A lampReturn object indicating the move operation result.
         */
        static lampReturn moveModSpecificFileType(const Base::lampMod::Mod* mod, Lamp::Core::FS::lampString extension, lampString localExtractionPath);

        /**
         * @brief Moves mod files within a specific folder.
         *
         * This function moves mod files within a specific folder from the extraction directory
         * to a local extraction path.
         *
         * @param mod The mod containing files to move.
         * @param extension The folder name to filter files by.
         * @param localExtractionPath The destination path for the moved files.
         * @return A lampReturn object indicating the move operation result.
         */
        static lampReturn moveModSpecificFolder(const Base::lampMod::Mod* mod, Lamp::Core::FS::lampString extension, lampString localExtractionPath);

    private:
        /**
         * @brief Copies a file from the source path to the destination path.
         *
         * @param source The source file path.
         * @param destination The destination file path.
         * @return A lampReturn object indicating the copy operation result.
         */
        static lampReturn copyFile(const std::filesystem::path& source, const std::filesystem::path& destination);

        /**
         * @brief Recursively copies a directory and its contents from the source path to the destination path.
         *
         * @param source The source directory path.
         * @param destination The destination directory path.
         * @return A lampReturn object indicating the copy operation result.
         */
        static lampReturn copyDirectoryRecursively(const std::filesystem::path& source, const std::filesystem::path& destination);

        /**
         * @brief Recursively copies a folder with a case-insensitive name match from source to destination.
         *
         * @param sourceDirectory The source directory path.
         * @param destinationDirectory The destination directory path.
         * @param targetDirectoryName The target folder name to match case-insensitively.
         * @return A lampReturn object indicating the copy operation result.
         */
        static lampReturn caseInsensitiveFolderCopyRecursive(const std::filesystem::path& sourceDirectory, const std::filesystem::path& destinationDirectory, const std::string& targetDirectoryName);

        /**
         * @brief Copies files with a specific file extension from the source directory to the destination directory.
         *
         * @param sourceDirectory The source directory path.
         * @param destinationDirectory The destination directory path.
         * @param extension The file extension to filter files by.
         * @return A lampReturn object indicating the copy operation result.
         */
        static lampReturn copyFilesWithExtension(const std::filesystem::path& sourceDirectory, const std::filesystem::path& destinationDirectory, const std::string& extension);

        /**
         * @brief Performs a case-insensitive string comparison between two strings.
         *
         * @param str1 The first string to compare.
         * @param str2 The second string to compare.
         * @return True if the strings are equal (case-insensitive), false otherwise.
         */
        static bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2);
    };

    /**
     * @brief The lampIO class provides input/output utility functions for loading and saving mod-related data.
     */
    class lampIO {
    public:
        /**
         * @brief Loads the list of mods for a specified game and profile.
         *
         * This function loads the list of mods for a specific game and profile from an XML file.
         *
         * @param game The name of the game.
         * @param profileName The name of the profile (default is "Default").
         * @return A vector of lampMod::Mod pointers representing the loaded mods.
         */
        static std::vector<Base::lampMod::Mod*> loadModList(Lamp::Core::FS::lampString game, lampString profileName = "Default");

        /**
         * @brief Saves the list of mods to an XML file.
         *
         * This function saves the list of mods to an XML file for a specified game and profile.
         *
         * @param game The name of the game.
         * @param ModList The vector of lampMod::Mod pointers to be saved.
         * @param profileName The name of the profile (default is "Default").
         * @return A lampReturn object indicating the save operation result.
         */
        static lampReturn saveModList(Lamp::Core::FS::lampString game, std::vector<Base::lampMod::Mod*> ModList, lampString profileName = "Default");

        /**
         * @brief Saves key-value data to a configuration file.
         *
         * This function saves key-value data to a configuration file for a specified game.
         *
         * @param key The key to save.
         * @param data The data associated with the key.
         * @param game The name of the game.
         * @return A lampReturn object indicating the save operation result.
         */
        static lampReturn saveKeyData(lampString key, lampString data, Lamp::Core::FS::lampString game);

        /**
         * @brief Loads key-value data from a configuration file.
         *
         * This function loads key-value data from a configuration file for a specified game.
         *
         * @param key The key to load.
         * @param game The name of the game.
         * @return A lampReturn object containing the loaded data.
         */
        static lampReturn loadKeyData(lampString key, Lamp::Core::FS::lampString game);

        /**
         * @brief Empties a folder with optional filtering by file extension.
         *
         * This function empties a folder by deleting its contents, optionally filtering by a specific file extension.
         *
         * @param Path The path of the folder to empty.
         * @param SpecificExtension The file extension to filter files by (default is empty, which means all files will be deleted).
         * @return A lampReturn object indicating the operation result.
         */
        static lampReturn emptyFolder(lampString Path, lampString SpecificExtension = "");

        /**
         * @brief Recursively copies files and folders from source to destination with optional ignore list.
         *
         * This function recursively copies files and folders from the source to the destination, ignoring specified folders.
         *
         * @param source The source directory.
         * @param destination The destination directory.
         * @param ignoreFolders A vector of folder names to ignore during the copy operation.
         */
        static void recursiveCopyWithIgnore(const std::filesystem::path& source, const std::filesystem::path& destination, const std::vector<std::string>& ignoreFolders);

        /**
         * @brief Copies files with a specific extension while ignoring others.
         *
         * This function copies files with a specific file extension from the source directory to the destination directory,
         * ignoring files with different extensions.
         *
         * @param sourceDir The source directory.
         * @param destDir The destination directory.
         * @param nonIgnoredExtension The file extension to copy while ignoring others.
         */
        static void copyExtensionWithConfigIgnore(const std::filesystem::path& sourceDir, const std::filesystem::path& destDir, std::string nonIgnoredExtension);

        /**
         * @brief Handles file drop events.
         *
         * This function handles file drop events, copying supported files to the appropriate destination.
         *
         * @param window The GLFW window.
         * @param count The number of dropped files.
         * @param paths An array of dropped file paths.
         */
        static void fileDrop(GLFWwindow* window, int count, const char** paths);

        /**
         * @brief Downloads a file from a URL and saves it with a specified filename.
         *
         * This function downloads a file from a given URL and saves it with the specified output filename.
         *
         * @param url The URL of the file to download.
         * @param output_filename The filename to save the downloaded file as.
         * @return A lampReturn object indicating the download operation result.
         */
        static lampReturn downloadFile(lampString url, lampString output_filename);
    };


/**
 * @class lampShare
 * @brief A class for handling profile export and import operations.
 */
    class lampShare {
    public:
        /**
         * @brief Export a user profile to a compressed file.
         *
         * This function exports a user profile to a compressed .lampProfile file.
         *
         * @param profileName The name of the profile to be exported.
         */
        static void exportProfile(std::string profileName);

        /**
         * @brief Import a user profile from a compressed file.
         *
         * This function allows users to import a previously exported profile.
         */
        static void importProfile();

    private:
        /**
         * @brief Insert XML data into an existing file.
         *
         * This private function takes a filename and an XML node and inserts the XML data at the start of the file.
         *
         * @param filename The name of the file to insert XML data into.
         * @param metadata The XML node containing the metadata to be inserted.
         */
        static void InsertXMLintoFile(const std::string& filename, const pugi::xml_node& metadata);

        /**
         * @brief Extract XML data from a file and return it as a string.
         *
         * This private function reads a file line by line and extracts the XML content between "<metadata>" and "</metadata>" tags.
         *
         * @param filename The name of the file from which to extract XML data.
         * @param content[out] A string containing the extracted XML data.
         */
        static void ExtractXMLfromFile(const std::string& filename, std::string& content);

        /**
         * @brief Compress a file using LZ4HC.
         *
         * This private function compresses an input file using the LZ4HC compression algorithm and saves the result in an output file.
         *
         * @param inputFile The path to the input file to be compressed.
         * @param outputFile The path to the output file where the compressed data will be saved.
         * @return True if compression is successful, false otherwise.
         */
        static bool compressFile(const std::string& inputFile, const std::string& outputFile);

        /**
         * @brief Decompress a file using LZ4HC.
         *
         * This private function decompresses an input file that was compressed with LZ4HC and saves the result in an output file.
         *
         * @param inputFile The path to the input compressed file.
         * @param outputFile The path to the output file where the decompressed data will be saved.
         * @param volumeSize The expected volume size of the decompressed data.
         * @return True if decompression is successful, false otherwise.
         */
        static bool decompressFile(const std::string& inputFile, const std::string& outputFile, std::uintmax_t volumeSize);
    };

    /**
     * @class lampUpdate
     * @brief A singleton class for checking and retrieving updates.
     *
     * The `lampUpdate` class provides functionality to check for updates and retrieve expression data.
     */
    class lampUpdate {
    public:
        /**
         * @brief Get the instance of the `lampUpdate` class.
         * @return A reference to the `lampUpdate` instance.
         *
         * This static function allows access to the singleton instance of the `lampUpdate` class.
         */
        static lampUpdate& getInstance()
        {
            static lampUpdate instance;
            return instance;
        }

        /**
         * @brief Deleted copy constructor to prevent copying of the singleton instance.
         */
        lampUpdate(lampUpdate const&) = delete;

        /**
         * @brief Deleted assignment operator to prevent copying of the singleton instance.
         */
        void operator=(lampUpdate const&)  = delete;

        /**
         * @brief Callback function for writing received data.
         *
         * This static function is used as a callback to write received data into a string.
         *
         * @param contents A pointer to the received data.
         * @param size The size of each data element.
         * @param nmemb The number of data elements.
         * @param output A pointer to the output string where data will be appended.
         * @return The total size of data written.
         */
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
            size_t total_size = size * nmemb;
            output->append(static_cast<char*>(contents), total_size);
            return total_size;
        }

        /**
         * @brief The version number of the software.
         */
        std::string versionNumber = "1.0.8"; // x-release-please-version

        /**
         * @brief Check for updates.
         *
         * This function checks for updates to the software.
         */
        void checkForUpdates();

        void getExpression();

    private:
        /**
         * @brief Private constructor to enforce singleton pattern.
         */
        lampUpdate(){}
    };
}
#endif //LAMP_LAMPFS_H
