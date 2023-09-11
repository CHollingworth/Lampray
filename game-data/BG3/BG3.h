//
// Created by charles on 11/09/23.
//

#ifndef LAMP_BG3_H
#define LAMP_BG3_H

#include "../gameControl.h"
    namespace Lamp::Game {
        class BG3: public gameControl {
        public:

            enum ModType{
                BG3_ENGINE_INJECTION = 0,
                BG3_MOD,
                BG3_BIN_OVERRIDE,
                BG3_DATA_OVERRIDE,
                NaN
            };

            std::list<Lamp::Core::lampMod::Mod *> ModList;

            static BG3& getInstance()
            {
                static BG3 instance;
                return instance;
            }

            BG3(BG3 const&) = delete;
            void operator=(BG3 const&)  = delete;

            bool init() override{};

            bool close(std::list<Lamp::Core::lampMod> ModList) override{};

            int internalTypeToInt(Lamp::Core::lampMod Mod) override{};
            std::list<std::string> internalTypeListToStrings() override{};

            void registerArchive(std::string Path) override;

            void ConfigMenu() override{};
            bool createFileStructure() override{};

            bool preCleanUp() override{};
            bool preDeployment() override{};
            bool deployment() override{};
            void postDeploymentTasks() override{};

            void listArchives() override;
        private:
            BG3(){};
        };
    }
// Lamp

#endif //LAMP_BG3_H
