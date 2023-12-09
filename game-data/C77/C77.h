//
// Created by charles on 14/10/23.
//

#ifndef LAMP_C77_H
#define LAMP_C77_H

#include "../gameControl.h"
#include "../../Lamp/Filesystem/lampFS.h"


namespace Lamp::Game {

        class C77 : public gameControl  {

             lampReturn registerArchive(lampString Path, int ArchiveModType = -1) override;

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


            int SeparatorModType(){
                return MOD_SEPARATOR;
            }

            std::vector<std::map<int, std::string> >& getModTypes() override {
                return CModType;
            }

            std::map<int, std::string>& getModTypesMap() override{
                return CModTypeMap;
            }

        private:

            enum ModType{
                C77_MOD,
                MOD_SEPARATOR = 999
            };

            // use a vector to keep things organized, this allows us to output mod types in the order we define
            std::vector<std::map<int, std::string> > CModType{
                {{ C77_MOD, "Mod" }},
                {{ MOD_SEPARATOR, "Separator" }}
            };
            // we will load the mod type vector above into this so we can get display values by the mod type value
            std::map<int, std::string> CModTypeMap = initModTypesMap();

            std::map<std::string,std::string> keyInfo{
                    {"installPath",""},
                    {"ProfileList","Default"},
                    {"CurrentProfile", "Default"}
            };
        };

    } // Lamp

#endif //LAMP_C77_H
