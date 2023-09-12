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

            /**
             * @brief Enumerated type representing different games.
             *
             * This enum defines constants for different games. It includes an unknown (UNK) value
             * and a specific game (BG3) value.
             */
            enum Game{
                UNK = -1, ///< Unknown game.
                BG3 = 0   ///< Baldur's Gate 3.
            };

            /**
             * @brief Map that associates Game enum values with their corresponding string representations.
             *
             * This map is used to map Game enum values to human-readable string names.
             * It provides a way to display user-friendly names for the games.
             */
            std::map<Game, std::string> GameStringMap = {
                    {UNK, "LAMP CONFIG"},  ///< Unknown game string.
                    {BG3, "Baldur's Gate 3"} ///< Baldur's Gate 3 game string.
            };

            /**
             * @brief The currently selected game.
             *
             * This variable represents the currently selected game from the Game enum.
             * It is initialized to UNK (unknown) by default.
             */
            Game CurrentGame = UNK;

            /**
             * @brief Flag indicating whether to show an introductory menu.
             *
             * This boolean flag determines whether to display an introductory menu.
             * It is initially set to true, indicating that the introductory menu should be shown.
             */
            bool ShowIntroMenu = true;

        private:
            lampConfig() {}

        };
    }
} // Lamp

#endif //LAMP_LAMPCONFIG_H
