//
// Created by charles on 11/09/23.
//

#ifndef LAMP_GAMECONTROL_H
#define LAMP_GAMECONTROL_H

#include <string>
#include <list>
#include "../lampMod.h"

namespace Lamp {
    namespace Game {
        class gameControl {
        public:
            virtual bool init(){

            }

            virtual bool close(std::list<Lamp::Core::lampMod> ModList){

            }

            virtual int internalTypeToInt(Lamp::Core::lampMod Mod){
            }
            virtual std::list<std::string> internalTypeListToStrings(){
            }

            virtual void ConfigMenu(){}

            /**
             * @brief Create the file structure for the game.
             * @return If the creation succeeded.
             */
            virtual bool createFileStructure(){

            }

            /**
             * @brief Clean and remove all files within the created file structure and in the game.
             * @return If the cleanup succeeded.
             */
            virtual bool preCleanUp(){

            }

            /**
             * @brief Reextract archives into the file structure in mod order.
             * @return If the extraction succeeded.
             */
            virtual bool preDeployment(){

            }

            /**
             * @brief  Copy or create symlinks to the game's data folders.
             * @return If the creation succeeded.
             */
            virtual bool deployment(){

            }

            /**
             * This is where you would run tools such as Bodyslide or Fnis.
             */
            virtual void postDeploymentTasks(){

            }
        };
    }
} // Lamp

#endif //LAMP_GAMECONTROL_H
