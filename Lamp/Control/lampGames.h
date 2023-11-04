//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPGAMES_H
#define LAMP_LAMPGAMES_H

#include "../../game-data/BG3/BG3.h"
#include "../../game-data/C77/C77.h"

namespace Lamp {
    class Games {
    public:
        /**
         * @brief Represents a list of game controls.
         */
        std::vector<Game::gameControl *> gameList{
                new Game::BG3,
                new Game::C77
        };

        /**
         * @brief Represents the name of the current user profile.
         */
        std::string currentProfile = "Default";

        /**
         * @brief Represents the index of the current game in the gameList.
         */
        int currentGameInt = 0;

        /**
         * @brief Represents the current game being played.
         */
        Game::gameControl *currentGame = gameList[currentGameInt];

        /**
         * @brief Provides access to the singleton instance of the Games class.
         * @return The instance of the Games class.
         */
        static Games& getInstance()
        {
            static Games instance;
            return instance;
        }

        /**
         * @brief Deleted copy constructor to prevent copying of the Games instance.
         */
        Games(Games const&) = delete;

        /**
         * @brief Deleted assignment operator to prevent assignment of the Games instance.
         */
        void operator=(Games const&) = delete;

    private:
        /**
         * @brief Private constructor to enforce singleton pattern and launch all gameControls.
         */
        Games(){
            for (Game::gameControl * gameInstance: gameList) {
                gameInstance->launch();
            }
        }
    };
};
#endif //LAMP_LAMPGAMES_H
