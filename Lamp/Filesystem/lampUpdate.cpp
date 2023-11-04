//
// Created by charles on 27/09/23.
//
#include "lampFS.h"

#include <curl/curl.h>
#include <string>
#include <iostream>
#include "../../third-party/imgui/imgui.h"

void Lamp::Core::FS::lampUpdate::checkForUpdates() {
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
                // Find the position of the second newline character
                size_t second_pos = response_data.find('\n', pos + 1);
                if (second_pos != std::string::npos) {
                    // Extract the second line of the response
                    std::string second_line = response_data.substr(pos + 1, second_pos - pos - 1);
                    if (versionNumber != second_line) {
                        versionNumber = "Update Available!";
                    }
                }
            }
        }
        curl_easy_cleanup(curl);
    }
}

void Lamp::Core::FS::lampUpdate::getExpression() {
    auto style = ImGui::GetStyle();
    float buttonWidth1 = ImGui::CalcTextSize(("v"+versionNumber).c_str()).x + style.FramePadding.x * 2.f;
    float widthNeeded = buttonWidth1;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);


    if(versionNumber == "Update Available!") {
        if (ImGui::Button((versionNumber).c_str())) {
            const char* url = "https://github.com/CHollingworth/Lamp/releases/latest";
            std::string openCommand = "xdg-open " + std::string(url);
            system(openCommand.c_str());
        }
    }else{
        ImGui::Text(("v"+versionNumber).c_str());
    }
}
