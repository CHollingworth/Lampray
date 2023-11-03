//
// Created by charles on 11/09/23.
//

#ifndef LAMP_GAMECONTROL_H
#define LAMP_GAMECONTROL_H

#include <string>
#include <list>
#include <map>
#include "../Lamp/Base/lampBase.h"


namespace Lamp::Game {
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;
    typedef Core::Base::lampExecutor lampExecutor;
        class gameControl {
        public:

            virtual lampReturn registerArchive(lampString Path) = 0;

            virtual lampReturn ConfigMenu() = 0;

            virtual lampReturn startDeployment() = 0;

            virtual lampReturn preCleanUp() = 0;

            virtual lampReturn preDeployment() = 0;

            virtual lampReturn deployment() = 0;

            virtual lampReturn postDeploymentTasks() = 0;

            virtual void listArchives() = 0;

            virtual std::map<std::string,std::string>& KeyInfo() = 0;

            virtual Core::Base::lampTypes::lampIdent Ident() = 0;

            virtual std::vector<Core::Base::lampMod::Mod *>& getModList() = 0;


            virtual void launch() = 0;
        protected:
            gameControl(){}
        };
} // Lamp

#endif //LAMP_GAMECONTROL_H
