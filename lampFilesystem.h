//
// Created by charles on 11/09/23.
//

#ifndef LAMP_LAMPFILESYSTEM_H
#define LAMP_LAMPFILESYSTEM_H

#include <string>
#include <list>
#include <GLFW/glfw3.h>
#include "bitformat.hpp"
#include "lampMod.h"
#include "lampConfig.h"
#include "bit7zlibrary.hpp"
#include "bitarchivereader.hpp"
#include "bitexception.hpp"
namespace Lamp {
    namespace Core {
        class lampFilesystem {
        public:
            static lampFilesystem& getInstance()
            {
                static lampFilesystem instance;
                return instance;
            }

            lampFilesystem(lampFilesystem const&)               = delete;
            void operator=(lampFilesystem const&)  = delete;

            bool init();
            bool init_config();
            void load_conifg();
            void save_config();

            /**
             * @brief Extracts a game mod from a compressed format and stores it locally.
             *
             * This function extracts a game mod specified by the provided parameters from a
             * compressed format (e.g., 7z) and stores it locally in the specified extraction path.
             *
             * @param Game The game for which the mod is intended.
             * @param Type The format of the compressed mod file (e.g., 7z).
             * @param mod Pointer to the Lamp Mod object representing the mod to extract.
             * @param localExtractionPath The local directory path where the mod will be extracted, this is game local.
             *
             * @note The function assumes that the specified mod exists and is compatible with the provided game.
             * @note The extracted mod will be stored in the local directory specified by 'localExtractionPath'.
             */
            void extract(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat & Type, Lamp::Core::lampMod::Mod * mod, std::string localExtractionPath);


            /**
             * @brief Extracts a specific file type from a game mod in a compressed format.
             *
             * This function extracts a specific file type from a game mod specified by the provided parameters
             * from a compressed format (e.g., 7z) and stores it locally in the specified extraction path.
             *
             * @param Game The game for which the mod is intended.
             * @param Type The format of the compressed mod file (e.g., 7z).
             * @param mod Pointer to the Lamp Mod object representing the mod to extract.
             * @param localExtractionPath The local directory path where the extracted file will be stored.
             * @param extension The file extension (excluding the dot) of the specific file to extract (e.g., "txt").
             *
             * @see getGameSpecificStoragePath(Lamp::Core::lampConfig::Game Game);
             *
             * @note The function assumes that the specified mod exists and is compatible with the provided game.
             * @note The extracted file will be stored in the local directory specified by 'localExtractionPath'.
             * @note If multiple files match the specified extension, all of them will be extracted.
             */
            void extractSpecificFileType(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat & Type, Lamp::Core::lampMod::Mod * mod, std::string localExtractionPath, std::string extension);


            /**
             * @brief Retrieves the specific storage path for a game.
             *
             * This function returns the storage path for a specific game based on the provided game configuration.
             *
             * @param Game The game for which the storage path is requested.
             *
             * @return A string representing the game's specific storage path.
             *
             * @note The function assumes that the provided game configuration is valid and recognized.
             * @note The returned storage path is a relative directory path.
             */
            std::string getGameSpecificStoragePath(Lamp::Core::lampConfig::Game Game);


            /**
             * @brief Loads a list of game mods for the specified game.
             *
             * This function loads a list of game mods associated with the provided game configuration.
             *
             * @param Game The game for which to load the mod list.
             *
             * @return A std::list of pointers to Lamp Mod objects representing the loaded mods.
             *
             * @note The function assumes that the provided game configuration is valid and recognized.
             */
            std::list<Lamp::Core::lampMod::Mod *> loadModList(Lamp::Core::lampConfig::Game Game);


            /**
             * @brief Saves a list of game mods for the specified game.
             *
             * This function saves the provided list of game mods to a persistent storage for the specified game.
             *
             * @param Game The game for which to save the mod list.
             * @param ModList A std::list of pointers to Lamp Mod objects representing the mods to be saved.
             *
             * @return True if the mod list was successfully saved, false otherwise.
             *
             * @note The function assumes that the provided game configuration is valid and recognized.
             * @note The caller is responsible for ensuring that the Mod objects in ModList are valid and properly managed.
             */
            bool saveModList(Lamp::Core::lampConfig::Game Game, std::list<Lamp::Core::lampMod::Mod *> ModList);


            /**
             * @brief Saves key-value data associated with a specific game.
             *
             * This function stores key-value data in a persistent storage for the specified game.
             *
             * @param Game The game for which to save the data.
             * @param key The key under which the data will be stored.
             * @param data The data to be saved as a string.
             *
             * @return True if the data was successfully saved, false otherwise.
             *
             * @note The function assumes that the provided game configuration is valid and recognized.
             */
            bool saveKeyData(Lamp::Core::lampConfig::Game Game, std::string key, std::string data);


            /**
             * @brief Loads key-value data associated with a specific game.
             *
             * This function retrieves key-value data from a persistent storage for the specified game.
             *
             * @param Game The game for which to load the data.
             * @param key The key under which the data is stored.
             *
             * @return A string containing the loaded data, or an empty string if the data is not found.
             *
             * @note The function assumes that the provided game configuration is valid and recognized.
             */
            std::string loadKeyData(Lamp::Core::lampConfig::Game Game, std::string key);


            /**
             * @brief Callback function for handling file drop events in a GLFW window.
             *
             * This static function is a callback used to handle file drop events in a GLFW window.
             * When files are dropped onto the window, this function is called with information about the dropped files.
             *
             * @param window The GLFW window receiving the file drop event.
             * @param count The number of files being dropped.
             * @param paths An array of C-style strings containing the file paths of the dropped files.
             *
             * @note This function is intended to be registered as a callback with GLFW for handling file drop events.
             * @note The 'window' parameter specifies the window in which the files were dropped.
             * @note 'count' indicates the number of files being dropped, and 'paths' contains their file paths.
             */
            static void fileDrop(GLFWwindow* window, int count, const char** paths);
        private:
            lampFilesystem() {}

            const std::string baseDataPath = "Lamp_Data/";
            const std::string saveDataPath = baseDataPath+"Mod_Lists/";
            const std::string archiveDataPath = baseDataPath+"Archives/";
            const std::string ConfigDataPath = baseDataPath+"Config/";
            const std::string DeploymentDataPath = baseDataPath+"Deployment/";

        };


    }
} // Lamp

#endif //LAMP_LAMPFILESYSTEM_H
