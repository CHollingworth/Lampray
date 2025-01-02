//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPCONFIG_H
#define LAMP_LAMPCONFIG_H

#include <list>
#include <filesystem>
#include <variant>
#include "../../game-data/gameControl.h"
#include "../Base/lampBase.h"

namespace Lamp::Core {
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;

/**
 * @brief The `lampConfig` class manages configuration settings and data paths for Lampray.
 *
 * The `lampConfig` class provides access to Lampray's configuration settings, data paths, and other
 * configuration-related functionality.
 */
    class lampConfig {
    public:
        /**
         * @brief Gets the singleton instance of the `lampConfig` class.
         *
         * @return The reference to the singleton instance.
         */
        static lampConfig& getInstance()
        {
            static lampConfig instance;
            return instance;
        }

        lampConfig(lampConfig const&) = delete;
        void operator=(lampConfig const&) = delete;
    
        const lampString baseDataPath = getBaseDataPath();
        const lampString saveDataPath = baseDataPath + "Mod_Lists/";
        const lampString archiveDataPath = baseDataPath + "Archives/";
        const lampString ConfigDataPath = baseDataPath + "Config/";
        const lampString DeploymentDataPath = baseDataPath + "Deployment/";
        const lampString workingPaths = baseDataPath + "WorkingDirectories/";

        lampString bit7zLibraryLocation = "";

        const bool defaultCheckForUpdateAtStart = true;
        bool checkForUpdatesAtStartup = true;

        int listHighlight = -1;
        char searchBuffer[250]{};

        std::map<lampString, lampString> lampFlags = {
                {"showIntroMenu", "1"},
        };

        /**
         * @brief Gets the default ImGui window flags for Lampray windows.
         *
         * @return The ImGui window flags.
         */
        ImGuiWindowFlags DefaultWindowFlags();

        /**
         * @brief Initializes the `lampConfig` instance.
         *
         * @return `true` if initialization was successful; otherwise, `false`.
         */
        bool init();

    private:
        /**
         * @brief Private constructor for the `lampConfig` class.
         *
         * The constructor is private to ensure that only one instance of `lampConfig` can exist.
         */
        lampConfig(){};

        static lampString getBaseDataPath() {
          std::string ret = "";
#ifdef USE_XDG
          char *xdg_data_home = std::getenv("XDG_DATA_HOME");

          if (!xdg_data_home) {
            ret += std::getenv("HOME"); 
            ret += "/.local/share";
          }
          
          ret += "/lampray/";
#else 
          ret = "Lamp_Data/";
#endif
          return lampString(ret);
        }
    };
};
#endif //LAMP_LAMPCONFIG_H
