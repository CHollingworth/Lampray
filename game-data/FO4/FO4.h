//
// Created by charles on 13/09/23.
//

#ifndef LAMP_FO4_H
#define LAMP_FO4_H

#include "../gameControl.h"
#include "../../lampFilesystem.h"
#include "../../lampConfig.h"

namespace Lamp {
    namespace Game {

        class FO4 : public gameControl {
        public:

            std::string installDirPath = "steam/install/path";
            std::string appDataPath = "app/data/path";

            enum ModType{
                F04_NO_FOMOD = 0,
                F04_WITH_FOMOD,
                F04_MAINFOLDER,
                NaN
            };

            std::list<Lamp::Core::lampMod::Mod *> ModList;

            bool checkLock = false;


            static FO4& getInstance()
            {
                static FO4 instance;
                return instance;
            }

            FO4(FO4 const&) = delete;
            void operator=(FO4 const&)  = delete;

            void registerArchive(std::string Path) override;

            bool ConfigMenu() override;
            bool createFileStructure() override;

            bool startDeployment() override;

            bool preCleanUp() override;
            bool preDeployment() override;
            bool deployment() override;
            void postDeploymentTasks() override;

            void lootSortAllPlugins();

            void listArchives() override;
        private:
            FO4 (){


            }
        };

    } // Lamp
} // Game

#endif //LAMP_FO4_H
