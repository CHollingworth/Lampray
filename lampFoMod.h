//
// Created by charles on 13/09/23.
//

#ifndef LAMP_LAMPFOMOD_H
#define LAMP_LAMPFOMOD_H

#include <list>
#include <string>
#include <vector>
#include <pugixml.hpp>


namespace Lamp::Core{
    class lampFoMod{

    public:

        static lampFoMod& getInstance()
        {
            static lampFoMod instance;
            return instance;
        }

        lampFoMod(lampFoMod const&) = delete;
        void operator=(lampFoMod const&)  = delete;

        struct Config{};


        Config ParseModuleConfig(const std::string& xmlContent) {

        }

        std::string toLowerCase(const std::string& str) {
            std::string result = str;
            for (char& c : result) {
                c = std::tolower(c);
            }
            return result;
        }

        std::string Pth;
        bool running = false;

        bool runInstaller(const std::string& modFolder){
            std::string directoryPath = modFolder;
            std::string targetDirectory = "fomod";
            Pth = modFolder + "/build";
            fs::create_directories(Pth);

            fs::path searchPath(directoryPath);

            if (fs::exists(searchPath) && fs::is_directory(searchPath)) {
                for (const auto& entry : fs::directory_iterator(searchPath)) {
                    if (entry.is_directory() &&  toLowerCase(entry.path().filename()) == targetDirectory) {
                        for(const auto& subentry : fs::directory_iterator(entry.path())){
                            if(fs::is_regular_file(subentry)){
                                std::string entryFilename = subentry.path().filename().string();
                                std::string lowercaseEntryFilename = "moduleconfig.xml";

                                if(toLowerCase(entryFilename) == lowercaseEntryFilename){
                                    //CurrentConfig = ParseModuleConfig(entry.path().string() + "/" + entryFilename);

                                }
                            }
                        }
                        break;
                    }
                }
            } else {
                return false;
            }



        }

        void build(){
            if(!running) return;

        }

        void pickOption(int i){
            if(!running) return;
        }

        void setFlag(){
            if(!running) return;
        }

        void showMenus(){
            if(!running) return;

        }


    private:
        lampFoMod(){}
    };
}


#endif //LAMP_LAMPFOMOD_H
