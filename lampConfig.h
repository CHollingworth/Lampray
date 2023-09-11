//
// Created by charles on 11/09/23.
//

#ifndef LAMP_LAMPCONFIG_H
#define LAMP_LAMPCONFIG_H

#include <map>
#include <string>

namespace Lamp {
    namespace Core {
        class lampConfig {
        public:

            static lampConfig& getInstance()
            {
                static lampConfig instance;
                return instance;
            }

            lampConfig(lampConfig const&) = delete;
            void operator=(lampConfig const&)  = delete;

            enum Game{
                UNK = -1,
                BG3 = 0
            };

            std::map<Game, std::string> GameStringMap = {
                    {UNK, "LAMP CONFIG"},
                    {BG3, "Baldur's Gate 3"}
            };

            Game CurrentGame = UNK;
            bool ShowIntroMenu = true;

        private:
            lampConfig() {}

        };
    }
} // Lamp

#endif //LAMP_LAMPCONFIG_H
