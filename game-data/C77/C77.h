//
// Created by charles on 14/10/23.
//

#ifndef LAMP_C77_H
#define LAMP_C77_H

#include "../gameControl.h"
#include "../../Lamp/Filesystem/lampFS.h"


namespace Lamp::Game {

        class C77 : public gameControl  {

             lampReturn registerArchive(lampString Path) override;

             lampReturn ConfigMenu() override;

             lampReturn startDeployment() override;

             lampReturn preCleanUp() override;

             lampReturn preDeployment() override;

             lampReturn deployment() override;

             lampReturn postDeploymentTasks() override;

             void listArchives() override;


            std::vector<Core::Base::lampMod::Mod *> ModList {};

            std::map<std::string,std::string>& KeyInfo() override { return keyInfo; };

            Core::Base::lampTypes::lampIdent Ident() override { return {"Cyberpunk 2077","C77"}; };

            std::vector<Core::Base::lampMod::Mod *>& getModList() override{ return ModList; };



            void launch() override {
                for (const auto& pair : keyInfo) {
                    const std::string& key = pair.first;
                    keyInfo[key] = (std::string) Lamp::Core::FS::lampIO::loadKeyData(key,Ident().ShortHand).returnReason;
                    if(key == "ProfileList"){
                        if(pair.second == "" || pair.second == "Default") {
                            keyInfo[key] = "Default";
                            Lamp::Core::FS::lampIO::saveKeyData(key, keyInfo[key], Ident().ShortHand);
                        }
                    }
                    if(key == "CurrentProfile"){
                        if(pair.second == "" || pair.second == "Default") {
                            keyInfo[key] = "Default";
                            Lamp::Core::FS::lampIO::saveKeyData(key, keyInfo[key], Ident().ShortHand);
                        }
                    }
                }
                ModList = Lamp::Core::FS::lampIO::loadModList(Ident().ShortHand, keyInfo["CurrentProfile"]);
            }

        private:
            enum ModType{
                C77_MOD,
                MOD_SEPARATOR
            };

            std::map<std::string,std::string> keyInfo{
                    {"installPath",""},
                    {"ProfileList","Default"},
                    {"CurrentProfile", "Default"}
            };
        };

    } // Lamp

#endif //LAMP_C77_H
