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

            bool createFileStructure(Lamp::Core::lampConfig::Game Game, std::list<std::string> structure);

            void extract(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat & Type, Lamp::Core::lampMod::Mod * mod, std::string localExtractionPath);
            void extractSpecificFileType(Lamp::Core::lampConfig::Game Game, const bit7z::BitInFormat & Type, Lamp::Core::lampMod::Mod * mod, std::string localExtractionPath, std::string extension);

            std::string getGameSpecificStoragePath(Lamp::Core::lampConfig::Game Game);

            std::list<Lamp::Core::lampMod::Mod *> loadModList(Lamp::Core::lampConfig::Game Game);
            bool saveModList(Lamp::Core::lampConfig::Game Game, std::list<Lamp::Core::lampMod::Mod *> ModList);

            bool saveKeyData(Lamp::Core::lampConfig::Game Game, std::string key, std::string data);
            std::string loadKeyData(Lamp::Core::lampConfig::Game Game, std::string key);

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
