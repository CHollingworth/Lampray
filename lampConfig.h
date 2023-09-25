//
// Created by charles on 11/09/23.
//

#ifndef LAMP_LAMPCONFIG_H
#define LAMP_LAMPCONFIG_H

#include <map>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Lamp {
    namespace Core {
        class lampConfig {
        public:

            static lampConfig& getInstance()
            {
                static lampConfig instance;
                return instance;
            }

            lampConfig(lampConfig const&) = delete;
            void operator=(lampConfig const&)  = delete;

            /**
             * @brief Enumerated type representing different games.
             *
             * This enum defines constants for different games. It includes an unknown (UNK) value
             * and a specific game (BG3) value.
             */
            enum Game{
                UNK = -1, ///< Unknown game.
                BG3 = 0,   ///< Baldur's Gate 3.
                FO4 = 1 ///< I dont want to set the world on fire.
            };

            /**
             * @brief Map that associates Game enum values with their corresponding string representations.
             *
             * This map is used to map Game enum values to human-readable string names.
             * It provides a way to display user-friendly names for the games.
             */
            std::map<Game, std::string> GameStringMap = {
                    {UNK, "LAMP CONFIG"},  ///< Unknown game string.
                    {BG3, "Baldur's Gate 3"}, ///< Baldur's Gate 3 game string.
                    {FO4, "Fallout 4"} ///< IIIII JUST WANT TO START A FLAME IN YOUR HEARRRRRRRRT
            };

            std::map<Game, std::string> GameShorthandMap = {
                    {UNK, "UNK"},  ///< Unknown game string.
                    {BG3, "BG3"}, ///< Baldur's Gate 3 game string.
                    {FO4, "FO4"} ///< IIIII JUST WANT TO START A FLAME IN YOUR HEARRRRRRRRT
            };

            /**
             * @brief The currently selected game.
             *
             * This variable represents the currently selected game from the Game enum.
             * It is initialized to UNK (unknown) by default.
             */
            Game CurrentGame = UNK;

            /**
             * @brief Flag indicating whether to show an introductory menu.
             *
             * This boolean flag determines whether to display an introductory menu.
             * It is initially set to true, indicating that the introductory menu should be shown.
             */
            bool ShowIntroMenu = true;

            int listHighlight = -1;
        private:
            lampConfig() {}
            struct colourSet{
            public:

                std::string Colour_WindowBackground;
                std::string Colour_Text;
                std::string Colour_Button;
                std::string Colour_Hover;
                std::string Colour_Pressed;
                std::string Colour_InputBG;
                std::string Colour_Separator;
                std::string Colour_MenuBar;
                std::string Colour_ToolTipAndTypes;
                std::string Colour_SearchHighlight;


                void setColourScheme(){
                    ImGuiStyle& style = ImGui::GetStyle();
                    style.Colors[ImGuiCol_WindowBg] = HexStringToImVec4(Colour_WindowBackground);
                    style.Colors[ImGuiCol_Text] = HexStringToImVec4(Colour_Text);
                    style.Colors[ImGuiCol_Button] = HexStringToImVec4(Colour_Button);
                    style.Colors[ImGuiCol_ButtonHovered] = HexStringToImVec4(Colour_Hover);
                    style.Colors[ImGuiCol_ButtonActive] = HexStringToImVec4(Colour_Pressed);
                    style.Colors[ImGuiCol_FrameBg] = HexStringToImVec4(Colour_InputBG);
                    style.Colors[ImGuiCol_Separator] = HexStringToImVec4(Colour_Separator);
                    style.Colors[ImGuiCol_MenuBarBg] = HexStringToImVec4(Colour_MenuBar);
                    style.Colors[ImGuiCol_PopupBg] = HexStringToImVec4(Colour_ToolTipAndTypes);
                    style.Colors[ImGuiCol_HeaderHovered] = HexStringToImVec4(Colour_Hover);
                    style.Colors[ImGuiCol_HeaderActive] = HexStringToImVec4(Colour_Pressed);
                }

                void getColourScheme(){
                    ImGuiStyle& style = ImGui::GetStyle();
                    Colour_WindowBackground = ImVec4ToHexString(style.Colors[ImGuiCol_WindowBg]);
                    Colour_Text = ImVec4ToHexString(style.Colors[ImGuiCol_Text]);
                    Colour_Button = ImVec4ToHexString(style.Colors[ImGuiCol_Button]);
                    Colour_Hover = ImVec4ToHexString(style.Colors[ImGuiCol_ButtonHovered]);
                    Colour_Pressed = ImVec4ToHexString(style.Colors[ImGuiCol_ButtonActive]);
                    Colour_InputBG = ImVec4ToHexString(style.Colors[ImGuiCol_FrameBg]);
                    Colour_Separator = ImVec4ToHexString(style.Colors[ImGuiCol_Separator]);
                    Colour_MenuBar = ImVec4ToHexString(style.Colors[ImGuiCol_MenuBarBg]);
                    Colour_ToolTipAndTypes = ImVec4ToHexString(style.Colors[ImGuiCol_PopupBg]);
                }


                std::string ImVec4ToHexString(const ImVec4& color) {
                    int r = static_cast<int>(color.x * 255.0f);
                    int g = static_cast<int>(color.y * 255.0f);
                    int b = static_cast<int>(color.z * 255.0f);
                    int a = static_cast<int>(color.w * 255.0f);

                    std::stringstream stream;
                    stream << std::hex << std::setw(2) << std::setfill('0') << r
                           << std::setw(2) << std::setfill('0') << g
                           << std::setw(2) << std::setfill('0') << b
                           << '-' << std::setw(2) << std::setfill('0') << a;

                    return stream.str();
                }

                ImVec4 HexStringToImVec4(const std::string& hexColor) {
                    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                    if (hexColor.size() >= 8) {
                        // Split the hex string at the '-' delimiter
                        std::string rgbHex = hexColor.substr(0, 6);
                        std::string alphaHex = hexColor.substr(7, 2);

                        // Convert RGB components from hex
                        int r, g, b, a;
                        std::istringstream(rgbHex.substr(0, 2)) >> std::hex >> r;
                        std::istringstream(rgbHex.substr(2, 2)) >> std::hex >> g;
                        std::istringstream(rgbHex.substr(4, 2)) >> std::hex >> b;
                        std::istringstream(alphaHex) >> std::hex >> a;

                        // Convert to ImVec4
                        color.x = static_cast<float>(r) / 255.0f;
                        color.y = static_cast<float>(g) / 255.0f;
                        color.z = static_cast<float>(b) / 255.0f;
                        color.w = static_cast<float>(a) / 255.0f;
                    }
                   return color;
                }
            };
        public:
            colourSet styleSet;
        };
    }
} // Lamp

#endif //LAMP_LAMPCONFIG_H
