//
// Created by charles on 11/09/23.
//

#ifndef LAMP_BG3_H
#define LAMP_BG3_H

#include "../gameControl.h"
#include <filesystem>
#include <iostream>
#include <regex>
#include <fstream>
#include <thread>
#include <future>
#include "../../json/json.hpp"
#include "../../lampWarn.h"
#include "../../lampArchiveDisplayHelper.h"
#include "../../imgui/imgui.h"
#include "../../lampFilesystem.h"
#include "../../nfd/include/nfd.h"

    namespace Lamp::Game {
        class BG3: public gameControl {
        public:

            std::string installDirPath = "steam/install/path";
            std::string appDataPath = "app/data/path";

            enum ModType{
                BG3_ENGINE_INJECTION = 0,
                BG3_MOD,
                BG3_BIN_OVERRIDE,
                BG3_DATA_OVERRIDE,
                BG3_MOD_FIXER,
                NaN
            };


            std::list<Lamp::Core::lampMod::Mod *> ModList;

            bool checkLock = false;


            static BG3& getInstance()
            {
                static BG3 instance;
                return instance;
            }

            BG3(BG3 const&) = delete;
            void operator=(BG3 const&)  = delete;

            void registerArchive(std::string Path) override;

            bool ConfigMenu() override;

            bool startDeployment() override;

            bool preCleanUp() override;
            bool preDeployment() override;
            bool deployment() override;
            void postDeploymentTasks() override;


            bool collectJsonData(Lamp::Core::lampMod::Mod * mod);

            void listArchives() override;
        private:
            BG3(){
                installDirPath = Lamp::Core::lampFilesystem::getInstance().loadKeyData(Core::lampConfig::BG3, "installDirPath");
                appDataPath = Lamp::Core::lampFilesystem::getInstance().loadKeyData(Core::lampConfig::BG3, "appDataPath");
            };

            std::vector<std::thread> modEditQueue;

            bool findJsonData(std::vector<std::string> xx);
        };
    }
// Lamp

#endif //LAMP_BG3_H
