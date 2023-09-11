//
// Created by charles on 11/09/23.
//

#ifndef LAMP_BG3_H
#define LAMP_BG3_H

#include "../gameControl.h"
    namespace Lamp::Game {
        class BG3: public gameControl {
        public:
            bool init() override;

            bool close(std::list<Lamp::Core::lampMod> ModList) override;

            int internalTypeToInt(Lamp::Core::lampMod Mod) override;
            std::list<std::string> internalTypeListToStrings() override;

            void ConfigMenu() override;
            bool createFileStructure() override;

            bool preCleanUp() override;
            bool preDeployment() override;
            bool deployment() override;
            void postDeploymentTasks() override;
        };
    }
// Lamp

#endif //LAMP_BG3_H
