//
// Created by charles on 08/10/23.
//

#ifndef LAMP_LAMPCUSTOMISE_H
#define LAMP_LAMPCUSTOMISE_H

#include "../../third-party/imgui/imgui.h"
#include "../Base/lampBase.h"
#include "../Filesystem/lampFS.h"
#include "../../third-party/imgui/imgui_internal.h"

namespace Lamp {
    namespace Core {

        class lampCustomise {
        public:

            static lampCustomise& getInstance()
            {
                static lampCustomise instance;
                return instance;
            }

            lampCustomise(lampCustomise const&) = delete;
            void operator=(lampCustomise const&)  = delete;

            std::string filePathOfLang;

            std::vector<std::string> defaultColours = {
                    "ffffff-ff",
                    "0a0d12-ff",
                    "141414-ff",
                    "260101-ff",
                    "071216-ff",
                    "590202-ff",
                    "a61103-ff",
                    "260101-ff",
                    "4296f9-4f",
                    "4296f9-cc",
                    "590202-ff",
                    "4296f9-ff",
                    "a61103-ff",
                    "1966bf-c6", // Colour_SearchHighlight
                    "1a994d-ff" // green-ish color for ButtonAlt
            };

            float floatMap[15][4] = {
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f },
                    { 1.0f, 0.0f, 0.0f, 0.0f }, // Colour_SearchHighlight
                    { 0.0f, 1.0f, 0.0f, 0.0f } // ButtonAlt
            };

            std::map<ImGuiCol_,std::string> styleMap = {
                    {ImGuiCol_Text,"Colour_Text"},
                    {ImGuiCol_WindowBg,"Colour_WindowBackground"},
                    {ImGuiCol_PopupBg, "Colour_ToolTipAndTypes"},
                    {ImGuiCol_FrameBg,"Colour_InputBG"},
                    {ImGuiCol_MenuBarBg, "Colour_MenuBar"},
                    {ImGuiCol_Button, "Colour_Button"},
                    {ImGuiCol_ButtonHovered,"Colour_Hover"},
                    {ImGuiCol_ButtonActive,"Colour_Pressed"},
                    {ImGuiCol_Header,"Colour_MenuItems"},
                    {ImGuiCol_HeaderHovered,"Colour_HeadHover"},
                    {ImGuiCol_HeaderActive,"Colour_HeadPressed"},
                    {ImGuiCol_Separator,"Colour_Separator"},
                    {ImGuiCol_SeparatorHovered,"Colour_SeparatorHover"}
            };

            const float defaultFontScale = 1.0f;


            void getConfigColours(){
                int x = 0;
                for (const auto& pair : styleMap) {
                    ImGuiCol_ key = pair.first;
                    const std::string& value = pair.second;

                    std::string loaded = Lamp::Core::FS::lampIO::loadKeyData(value, "LAMP CONFIG");
                    if(loaded == ""){
                        Lamp::Core::Base::lampTypes::lampHexAlpha colour = Lamp::Core::Base::lampTypes::lampHexAlpha(ImGui::GetStyle().Colors[key]);
                        Lamp::Core::FS::lampIO::saveKeyData(value, ((std::string)colour), "LAMP CONFIG");
                    }else{
                        ImGui::GetStyle().Colors[key] = Lamp::Core::Base::lampTypes::lampHexAlpha(loaded);
                    }


                    lampCustomise::getInstance().floatMap[x][0] = ImGui::GetStyle().Colors[key].x;
                    lampCustomise::getInstance().floatMap[x][1] = ImGui::GetStyle().Colors[key].y;
                    lampCustomise::getInstance().floatMap[x][2] = ImGui::GetStyle().Colors[key].z;
                    lampCustomise::getInstance().floatMap[x][3] = ImGui::GetStyle().Colors[key].w;


                    x++;
                }

                std::string xloaded = Lamp::Core::FS::lampIO::loadKeyData("Colour_SearchHighlight", "LAMP CONFIG");
                if(xloaded == ""){
                    Lamp::Core::Base::lampTypes::lampHexAlpha colour(lampCustomise::getInstance().defaultColours[x]);
                    Lamp::Core::FS::lampIO::saveKeyData("Colour_SearchHighlight", ((std::string)colour), "LAMP CONFIG");
                }else{
                    Lamp::Core::lampControl::getInstance().Colour_SearchHighlight = Lamp::Core::Base::lampTypes::lampHexAlpha(xloaded);
                }


                lampCustomise::getInstance().floatMap[x][0] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_SearchHighlight).x;
                lampCustomise::getInstance().floatMap[x][1] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_SearchHighlight).y;
                lampCustomise::getInstance().floatMap[x][2] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_SearchHighlight).z;
                lampCustomise::getInstance().floatMap[x][3] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_SearchHighlight).w;


				x++;
				std::string xloadedBtnAlt = Lamp::Core::FS::lampIO::loadKeyData("Colour_ButtonAlt", "LAMP CONFIG");
                if(xloadedBtnAlt == ""){
                    Lamp::Core::Base::lampTypes::lampHexAlpha colour(lampCustomise::getInstance().defaultColours[x]);
                    Lamp::Core::FS::lampIO::saveKeyData("Colour_ButtonAlt", ((std::string)colour), "LAMP CONFIG");
                }else{
                    Lamp::Core::lampControl::getInstance().Colour_ButtonAlt = Lamp::Core::Base::lampTypes::lampHexAlpha(xloadedBtnAlt);
                }

                lampCustomise::getInstance().floatMap[x][0] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_ButtonAlt).x;
                lampCustomise::getInstance().floatMap[x][1] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_ButtonAlt).y;
                lampCustomise::getInstance().floatMap[x][2] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_ButtonAlt).z;
                lampCustomise::getInstance().floatMap[x][3] = ((ImVec4)Lamp::Core::lampControl::getInstance().Colour_ButtonAlt).w;

                std::string loadedCheckUpdates = Lamp::Core::FS::lampIO::loadKeyData("Check_Updates_Startup", "LAMP CONFIG");
                if(loadedCheckUpdates == "0" || loadedCheckUpdates == "false"){
                    lampConfig::getInstance().checkForUpdatesAtStartup = false;
                }
            }

            void setColourTemp(ImGuiCol_ StylePoint, Lamp::Core::Base::lampTypes::lampHexAlpha colour){
                ImGui::GetStyle().Colors[StylePoint] = colour;
            }

            void setColour(ImGuiCol_ StylePoint, Lamp::Core::Base::lampTypes::lampHexAlpha colour){
                ImGui::GetStyle().Colors[StylePoint] = colour;
                Lamp::Core::FS::lampIO::saveKeyData(styleMap[StylePoint], ((std::string)colour), "LAMP CONFIG");
            }

            static bool displayMenu(){
                ImGuiIO &io = ImGui::GetIO();
                if (ImGui::CollapsingHeader(lampLang::LS("LAMPRAY_CUSTOM_COLOUR"))) {
                    int x = 0;
                    for (const auto &pair: lampCustomise::getInstance().styleMap) {
                        ImGuiCol_ key = pair.first;
                        const std::string &value = pair.second;
                        ImGui::ColorEdit4((pair.second + "##" + std::to_string(x)).c_str(),
                                          lampCustomise::getInstance().floatMap[x]);
                        lampCustomise::getInstance().setColourTemp(key,
                                                                   ImVec4(lampCustomise::getInstance().floatMap[x][0],
                                                                          lampCustomise::getInstance().floatMap[x][1],
                                                                          lampCustomise::getInstance().floatMap[x][2],
                                                                          lampCustomise::getInstance().floatMap[x][3]));
                        x++;
                    }
                    ImGui::ColorEdit4(("Colour_SearchHighlight##" + std::to_string(x)).c_str(),
                                      lampCustomise::getInstance().floatMap[x]);
                    Lamp::Core::lampControl::getInstance().Colour_SearchHighlight = ImVec4(
                            lampCustomise::getInstance().floatMap[x][0], lampCustomise::getInstance().floatMap[x][1],
                            lampCustomise::getInstance().floatMap[x][2], lampCustomise::getInstance().floatMap[x][3]);

                    x++;
                    ImGui::ColorEdit4(("Colour_ButtonAlt##" + std::to_string(x)).c_str(),
                                      lampCustomise::getInstance().floatMap[x]);
                    Lamp::Core::lampControl::getInstance().Colour_ButtonAlt = ImVec4(
                            lampCustomise::getInstance().floatMap[x][0], lampCustomise::getInstance().floatMap[x][1],
                            lampCustomise::getInstance().floatMap[x][2], lampCustomise::getInstance().floatMap[x][3]);
                    }

                    if (ImGui::CollapsingHeader(lampLang::LS("LAMPRAY_CUSTOM_FONT"))) {
                        const float MIN_SCALE = 0.3f;
                        const float MAX_SCALE = 2.0f;

                        ImGui::DragFloat(lampLang::LS("LAMPRAY_CUSTOM_FONT"), &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f",
                                         ImGuiSliderFlags_AlwaysClamp);
                    }
                    if (ImGui::CollapsingHeader(lampLang::LS("LAMPRAY_UPDATE_RULES"))) {
                        ImGui::Checkbox(lampLang::LS("LAMPRAY_UPDATE_RULES1")+" (Check_Updates_Startup)",
                                        &lampConfig::getInstance().checkForUpdatesAtStartup);
                    }


                if (ImGui::CollapsingHeader(lampLang::LS("LAMPRAY_CUSTOM_LANG"))) {
                    std::vector<std::string> xmlFiles;
                    std::filesystem::path folderPath = "Lamp_Language"; // Replace with your folder path

                    if (xmlFiles.empty()) {
                        try {
                            for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
                                if (entry.is_regular_file() && entry.path().extension() == ".xml") {
                                    xmlFiles.push_back(entry.path().string());
                                }
                            }


                        } catch (const std::exception& e) {
                        }
                    }

                    // ImGui dropdown
                    if (ImGui::BeginCombo("##SELECTXML", lampCustomise::getInstance().filePathOfLang.c_str())) {
                        for (const auto& xmlFile : xmlFiles) {
                            bool isSelected = (xmlFile == lampCustomise::getInstance().filePathOfLang);
                            if (ImGui::Selectable(xmlFile.c_str(), isSelected)) {
                                lampCustomise::getInstance().filePathOfLang = xmlFile;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }


                if(ImGui::Button(lampLang::LS("LAMPRAY_CUSTOM_SV"))){
                    ImGui::End();

                    int x = 0;
                    for (const auto& pair : lampCustomise::getInstance().styleMap) {
                        ImGuiCol_ key = pair.first;
                        lampCustomise::getInstance().setColour(key, ImVec4(lampCustomise::getInstance().floatMap[x][0], lampCustomise::getInstance().floatMap[x][1], lampCustomise::getInstance().floatMap[x][2], lampCustomise::getInstance().floatMap[x][3]));
                        x++;
                    }

                    Lamp::Core::FS::lampIO::saveKeyData("Colour_SearchHighlight", ((std::string)Lamp::Core::lampControl::getInstance().Colour_SearchHighlight), "LAMP CONFIG");
                    Lamp::Core::FS::lampIO::saveKeyData("Colour_ButtonAlt", ((std::string)Lamp::Core::lampControl::getInstance().Colour_ButtonAlt), "LAMP CONFIG");

                    // I don't feel like doing the work to get the value properly, and this seems to work fine
                    Lamp::Core::FS::lampIO::saveKeyData("Font_Scale", std::to_string(io.FontGlobalScale), "LAMP CONFIG");
                    Lamp::Core::FS::lampIO::saveKeyData("Check_Updates_Startup", std::to_string(lampConfig::getInstance().checkForUpdatesAtStartup), "LAMP CONFIG");
                    Lamp::Core::FS::lampIO::saveKeyData("LanguagePath", lampCustomise::getInstance().filePathOfLang, "LAMP CONFIG");
                    Lamp::Core::lampLang::getInstance().CurrentLanguage = Lamp::Core::lampLang::LanguageContainer();
                    Lamp::Core::Base::lampLog::getInstance().log(Lamp::Core::lampLang::getInstance().CurrentLanguage.build(lampCustomise::getInstance().filePathOfLang).returnReason);
                    return true;
                }
                ImGui::SameLine();
                if(ImGui::Button(lampLang::LS("LAMPRAY_GOBACK"))){
                        ImGui::End();
                        return true;
                }
                ImGui::SameLine();
                if(ImGui::Button(lampLang::LS("LAMPRAY_RESET"))){
                    Lamp::Core::lampControl::getInstance().Colour_SearchHighlight = Lamp::Core::Base::lampTypes::lampHexAlpha(lampCustomise::getInstance().defaultColours[13]);
                    Lamp::Core::lampControl::getInstance().Colour_ButtonAlt = Lamp::Core::Base::lampTypes::lampHexAlpha(lampCustomise::getInstance().defaultColours[14]);

                    for (int i = 0; i < lampCustomise::getInstance().defaultColours.size(); ++i) {
                        ImVec4 color = Lamp::Core::Base::lampTypes::lampHexAlpha(lampCustomise::getInstance().defaultColours[i]);
                        lampCustomise::getInstance().floatMap[i][0] = color.x;
                        lampCustomise::getInstance().floatMap[i][1] = color.y;
                        lampCustomise::getInstance().floatMap[i][2] = color.z;
                        lampCustomise::getInstance().floatMap[i][3] = color.w;
                    }


                    io.FontGlobalScale = lampCustomise::getInstance().defaultFontScale;
                    lampConfig::getInstance().checkForUpdatesAtStartup = lampConfig::getInstance().defaultCheckForUpdateAtStart;
                }

                ImGui::End();
                return false;
            }

        private:
            lampCustomise(){}
        };
    } // Lampray
} // Core

#endif //LAMP_LAMPCUSTOMISE_H
