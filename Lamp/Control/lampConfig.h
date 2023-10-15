//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPCONFIG_H
#define LAMP_LAMPCONFIG_H

#include <list>
#include <filesystem>
#include "../../game-data/gameControl.h"
#include "../Base/lampBase.h"

namespace Lamp::Core {
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;
    class lampConfig {
    public:

        static lampConfig& getInstance()
        {
            static lampConfig instance;
            return instance;
        }

        lampConfig(lampConfig const&) = delete;
        void operator=(lampConfig const&)  = delete;


        const lampString baseDataPath = ((std::string)std::getenv("HOME")) + "/.lamp/Lamp_Data/";
        const lampString saveDataPath = baseDataPath + "Mod_Lists/";
        const lampString archiveDataPath = baseDataPath + "Archives/";
        const lampString ConfigDataPath = baseDataPath + "Config/";
        const lampString DeploymentDataPath = baseDataPath + "Deployment/";
        lampString bit7zLibaryLocation = "";


        int listHighlight = -1;
        char searchBuffer[250]{};

        std::map<lampString, lampString> lampFlags = {
                {"showIntroMenu", "1"},
        };

        ImGuiWindowFlags DefaultWindowFlags();

        bool init();

    private:
        lampConfig(){
        };

    };
};
#endif //LAMP_LAMPCONFIG_H
