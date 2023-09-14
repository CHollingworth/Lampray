//
// Created by charles on 13/09/23.
//

#ifndef LAMP_FO4_H
#define LAMP_FO4_H

#include "../gameControl.h"

namespace Lamp {
    namespace Game {

        class FO4 : public gameControl {
        public:

            std::string installDirPath = "steam/install/path";
            std::string appDataPath = "app/data/path";

            enum ModType{
                BG3_NO_FOMOD = 0,
                BG3_WITH_FOMOD,
                ENB,
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
            FO4 (){}
        };

    } // Lamp
} // Game

#endif //LAMP_FO4_H
