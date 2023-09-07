//
// Created by charles on 02/09/23.
//

#ifndef LAMP_LAMPFS_H
#define LAMP_LAMPFS_H


#include <string>
#include <GLFW/glfw3.h>
#include <list>
#include "imgui/imgui.h"
#include "ostream"
#include "bit7zlibrary.hpp"
#include "bitarchivereader.hpp"
#include "bitexception.hpp"
#include <iostream>
#include <filesystem>
#include <regex>
#include <fstream>
#include "json/json.hpp"
#include "pugixml.hpp"

class LampFS {
public:
    enum ModType{
        BG3_ENGINE_INJECTION = 0,
        BG3_MOD,
        BG3_BIN_OVERRIDE,
        BG3_DATA_OVERRIDE,
        NaN
    };

private:

    LampFS() {}

    struct Archive{
    public:
        ModType type = ModType::NaN;
        std::string holdingPath = "";
        bool enabled = false;
        // use file compaison dumbass, LIES FORCEFULL RESET
        std::string serialize(){
            return std::to_string((static_cast<int>(type))) +","+ holdingPath +","+ std::to_string(enabled) + ",";
        }
    };

    std::list<Archive*> Archives;

public:

    static LampFS& getInstance()
    {
        static LampFS instance;
        return instance;
    }

    enum deployment_stages{
        BG3_IDLE,
        BG3_PRE_DEPLOYMENT,
        BG3_PRE_DEPLOYMENT_CLEAN_UP,
        BG3_DEPLOYMENT,
        BG3_CLEANUP
    };

    int removeArchive(std::filesystem::path path, LampFS::Archive * arch);
    void listArchives();

    void deploy();
    deployment_stages deploymentStage = deployment_stages::BG3_IDLE;;
    int deployDisplayMax = 0;
    int deployDisplayCount = 0;

    int loadArchives();
    void saveArchives();


    void checkForOrphanedFiles();
    bool extractPackFileOnly(const bit7z::BitInFormat & Type, std::list<Archive *>::iterator it, std::string extractionPath);

    bool insertIntoModSettings(std::string infopath);
    void extract(const bit7z::BitInFormat & Type, std::list<Archive *>::iterator it, std::string extractionPath);

    void saveConfig();
    void loadConfig();

    std::string installDirPath = "steam/install/path";
    std::string appDataPath = "app/data/path";

    static void fileDrop(GLFWwindow* window, int count, const char** paths);

    LampFS(LampFS const&)               = delete;
    void operator=(LampFS const&)  = delete;
};


#endif //LAMP_LAMPFS_H
