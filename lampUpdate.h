//
// Created by charles on 21/09/23.
//

#ifndef LAMP_LAMPUPDATE_H
#define LAMP_LAMPUPDATE_H

#include <curl/curl.h>
#include <string>
#include <iostream>
#include "imgui/imgui.h"

namespace Lamp::Core{
    class lampUpdate{
    public:
        static lampUpdate& getInstance()
        {
            static lampUpdate instance;
            return instance;
        }

        lampUpdate(lampUpdate const&) = delete;
        void operator=(lampUpdate const&)  = delete;

        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
            size_t total_size = size * nmemb;
            output->append(static_cast<char*>(contents), total_size);
            return total_size;
        }

        std::string versionNumber = "1.0.6";
        void checkForUpdates(){
            CURL* curl = curl_easy_init();
            if (curl) {
                std::string url = "https://raw.githubusercontent.com/CHollingworth/Lamp/master/VERSION";
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                std::string response_data;
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
                CURLcode res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                } else {
                    size_t pos = response_data.find('\n');
                    if (pos != std::string::npos) {
                        std::string first_line = response_data.substr(0, pos);
                        if(versionNumber != first_line){
                            versionNumber = "Update Available!";
                        }
                    }
                }
                curl_easy_cleanup(curl);
            }
        }


        void getButton(){
            auto style = ImGui::GetStyle();
            float buttonWidth1 = ImGui::CalcTextSize(("v"+versionNumber).c_str()).x + style.FramePadding.x * 2.f;
            float widthNeeded = buttonWidth1;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);


            if(versionNumber == "Update Available!") {
                if (ImGui::Button(versionNumber.c_str())) {
                    const char* url = "https://github.com/CHollingworth/Lamp/releases/latest";
                    std::string openCommand = "xdg-open " + std::string(url);
                    system(openCommand.c_str());
                }
            }else{
                ImGui::Text(("v"+versionNumber).c_str());
            }
        }

    private:
        lampUpdate(){}
    };
}
#endif //LAMP_LAMPUPDATE_H
