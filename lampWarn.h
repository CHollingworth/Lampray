//
// Created by charles on 13/09/23.
//

#ifndef LAMP_LAMPWARN_H
#define LAMP_LAMPWARN_H

#include <string>
#include <list>
#include <iostream>
#include "imgui/imgui.h"
#include "lampConfig.h"

namespace Lamp::Core{
    class lampWarn{
    public:
        static lampWarn& getInstance()
        {
            static lampWarn instance;
            return instance;
        }

        lampWarn(lampWarn const&) = delete;
        void operator=(lampWarn const&)  = delete;
        
        
        enum warningLevel{
            LOG,
            WARNING,
            ERROR
        };

        enum errorCode{
            LMP_UNK = 0,
            LMP_NODIRCREATION,
            LMP_NO7ZP,
            LMP_NOFILEDROP,
            LMP_CLEANUPFAILED,
            LMP_PREDEPLOYFAILED,
            LMP_DEOPLYMENTFAILED,
            LMP_POSTTASKFAILED,
            LMP_CONFIGSAVEFAILED,
            LMP_CONFIGLOADFAILED,
            LMP_MODLISTSAVEFAILED,
            LMP_MODLISTLOADFAILED,
            LMP_KEYSAVEFAILED,
            LMP_KEYLOADFAILED,
            LMP_EXTRACTIONFALED,
            LMP_COPYFAILED

        };

        std::list<std::string> poplist;
        
        void log(std::string data,warningLevel warningLevel = warningLevel::LOG,bool pop = false, errorCode errorCode = errorCode::LMP_UNK){
            std::string ping;
            switch (warningLevel) {
                case LOG:
                    ping = "[LAMP:LOG:"+ std::to_string(errorCode) +"] "+data;
                    break;
                case WARNING:
                    ping = "[LAMP:WARNING:"+ std::to_string(errorCode) +"] "+data;
                    break;
                case ERROR:
                    ping = "[LAMP:ERROR:"+ std::to_string(errorCode) +"] "+data;
                    break;
            }
            std::cout << ping << std::endl;
            if(pop){
                if(!lampConfig::getInstance().ShowIntroMenu) {
                    poplist.push_front(ping);
                }
            }
        }



        bool showWarns(){

            for (auto it = poplist.begin(); it != poplist.end();) {
                if (!poplist.empty()) {
                    std::string x = *it;
                    ImGuiIO &io = ImGui::GetIO();
                    ImGui::SetNextWindowSize(io.DisplaySize, 0);
                    ImGui::SetNextWindowPos(ImVec2(0, 0));

                    ImGuiWindowFlags windowFlags = 0;
                    windowFlags += ImGuiWindowFlags_NoMove;
                    windowFlags += ImGuiWindowFlags_NoResize;
                    windowFlags += ImGuiWindowFlags_NoCollapse;
                    windowFlags += ImGuiWindowFlags_NoTitleBar;
                    windowFlags += ImGuiWindowFlags_MenuBar;

                    ImGui::Begin(x.c_str(), NULL, windowFlags);

                    ImGui::Text(x.c_str());
                    ImGui::Separator();

                    ImGui::Text("If an error persists please create an issue on GitHub.");
                    if (ImGui::Button("Okay")) {
                        poplist.erase(it);
                    } else {
                        ++it; // Move to the next element
                    }

                    ImGui::End();
                }
                break;
            }
        }
        
    private:
        lampWarn(){}
    };
}
#endif //LAMP_LAMPWARN_H
