//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPGAMES_H
#define LAMP_LAMPGAMES_H

#include "../../game-data/BG3/BG3.h"
#include "../../game-data/C77/C77.h"

namespace Lamp {
    class Games{
    public:


        std::vector<Game::gameControl *> gameList{
                new Game::BG3,
                new Game::C77
        };

        std::string currentProfile = "Default";
        int currentGameInt = 0;

        Game::gameControl *currentGame = gameList[currentGameInt];
        static Games& getInstance()
        {
            static Games instance;
            return instance;
        }

        Games(Games const&) = delete;
        void operator=(Games const&)  = delete;
    private:
        Games(){
            for (Game::gameControl * gameInstance: gameList) {
                gameInstance->launch();
            }

        }
    };
};
#endif //LAMP_LAMPGAMES_H
