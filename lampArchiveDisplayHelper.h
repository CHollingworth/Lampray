//
// Created by charles on 21/09/23.
//

#ifndef LAMP_LAMPARCHIVEDISPLAYHELPER_H
#define LAMP_LAMPARCHIVEDISPLAYHELPER_H

#include <list>
#include <functional>
#include <filesystem>
#include <iostream>
#include "lampMod.h"
#include "lampConfig.h"
#include "imgui/imgui.h"
#include "lampFilesystem.h"

namespace Lamp::Core{
    class lampArchiveDisplayHelper{
    public:


        struct lampArchiveListBuilder{
        private:
            std::list<std::string> columnNames{"Enabled","Mod Name", "Mod Type", "Load Order","Last Updated" ,"Remove Mod"};
            std::list<Lamp::Core::lampMod::Mod *>& ModList;
            std::vector<std::string> typeNames;
            lampConfig::Game gameRefrence;
            std::list<std::pair<std::string,bool *>> ExtraOptions;
            std::string temp{"0"};
            char searchBuffer[250]{};


            int calculateLevenshteinDistance(const std::string& s1, const std::string& s2) {
                int len1 = s1.length();
                int len2 = s2.length();

                std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

                for (int i = 1; i <= len1; ++i) {
                    dp[i][0] = i;
                }

                for (int j = 1; j <= len2; ++j) {
                    dp[0][j] = j;
                }

                for (int i = 1; i <= len1; ++i) {
                    for (int j = 1; j <= len2; ++j) {
                        int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                        dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
                    }
                }

                return dp[len1][len2];
            }


            int findClosestMatchPosition() {

                if(std::strlen(searchBuffer) == 0){
                    return -1;
                }

                int closestMatchPosition = -1;
                int minDistance = std::numeric_limits<int>::max();

                for (auto it = ModList.begin(); it != ModList.end(); ++it) {
                    std::filesystem::path path((*it)->ArchivePath);
                    std::string cutname = path.filename().c_str();

                    if (gameRefrence == Lamp::Core::lampConfig::BG3) {
                        size_t pos = cutname.find('-');
                        if (pos != std::string::npos) {
                            cutname.erase(pos);
                        }
                    }

                    int distance = calculateLevenshteinDistance(cutname, searchBuffer);

                    if (distance < minDistance) {
                        minDistance = distance;
                        closestMatchPosition = std::distance(ModList.begin(), it);
                    }
                }

                return closestMatchPosition;
            }


            void moveUp(std::list<Lamp::Core::lampMod::Mod *>::iterator& it) {
                if (ModList.size() <= 1) {
                    return; // Nothing to move
                }

                if (it == ModList.begin()) {
                    // If the iterator points to the first element, wrap around to the end
                    std::list<Lamp::Core::lampMod::Mod *>::iterator last = std::prev(ModList.end());
                    std::swap(*it, *last);
                    it = last;
                } else {
                    // Swap the element with the one before it
                    std::list<Lamp::Core::lampMod::Mod *>::iterator prev = std::prev(it);
                    std::swap(*it, *prev);
                    it = prev;
                }
            }

            void moveDown(std::list<Lamp::Core::lampMod::Mod *>::iterator& it) {
                if (ModList.size() <= 1) {
                    return; // Nothing to move
                }

                std::list<Lamp::Core::lampMod::Mod *>::iterator next = std::next(it);

                if (next == ModList.end()) {
                    // If the iterator points to the end, wrap around to the beginning
                    std::swap(*it, *ModList.begin());
                    it = ModList.begin();
                } else {
                    // Swap the element with the one after it
                    std::swap(*it, *next);
                    it = next;
                }
            }

        public:



            lampArchiveListBuilder(std::list<std::string> ExtraColumnNames, std::list<Lamp::Core::lampMod::Mod *> &modList,
                                  std::vector<std::string> typeNames, lampConfig::Game gameRefrence,
                                   std::list<std::pair<std::string, bool * >> extraOptions)
                    : ModList(modList), typeNames(typeNames), gameRefrence(gameRefrence),
                      ExtraOptions(extraOptions) {
                columnNames.insert(columnNames.end(), ExtraColumnNames.begin(), ExtraColumnNames.end());

                // add assert for length here
            }

            void createImguiMenu(){
                ImGui::Text("Search for Name: ");
                ImGui::SameLine();
                if(ImGui::InputText("##searcher", searchBuffer, 250)){
                    lampConfig::getInstance().listHighlight = findClosestMatchPosition();
                }
                if(ImGui::BeginTable(lampConfig::getInstance().GameShorthandMap[gameRefrence].c_str(), columnNames.size()+1,  ImGuiTableFlags_SizingStretchProp)) {
                    for (auto it = columnNames.begin(); it != columnNames.end(); ++it) {
                        ImGui::TableNextColumn();

                        ImGui::Text((*it).c_str());
                    }
                    ImGui::TableNextRow();
                    int i = 0;
                    for (auto it = ModList.begin(); it != ModList.end(); ++it) {

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }


                        if((*it)->enabled) {
                            if (ImGui::Button(("Enabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = false;
                                Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence, ModList);
                            }
                        }else{
                            if (ImGui::Button(("Disabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = true;
                                Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence, ModList);
                            }
                        }
                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }

                        std::filesystem::path path((*it)->ArchivePath);
                        std::string cutname = path.filename().c_str();
                        if(gameRefrence == Lamp::Core::lampConfig::BG3) {
                            size_t pos = cutname.find('-');
                            if (pos != std::string::npos) {
                                cutname.erase(pos);
                            }
                        }

                            ImGui::Text(cutname.c_str());
                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                ImGui::SetTooltip((*it)->ArchivePath.c_str());
                            }



                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }

                        if (ImGui::BeginMenu((typeNames[(*it)->modType] + "##" + std::to_string(i)).c_str())) {
                            int y = 0;
                            ImGui::MenuItem(cutname.c_str());
                            ImGui::MenuItem("------------");

                            for (auto itt = typeNames.begin(); itt != typeNames.end(); ++itt) {
                                if (ImGui::MenuItem((*itt).c_str())) {
                                    (*it)->modType = y;
                                    Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence, ModList);
                                }
                                y++;
                            }
                            ImGui::EndMenu();
                        }


                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }


                        if(ImGui::Button(("Up##"+std::to_string(i)).c_str())){
                            moveUp(it);
                            Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence, ModList);
                        }
                        ImGui::SameLine();
                        ImGui::Text((std::to_string(i)).c_str());
                        ImGui::SameLine();
                        if(ImGui::Button(("Down##"+std::to_string(i)).c_str())){
                            moveDown(it);
                            Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence, ModList);
                        }

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }


                        ImGui::Text((*it)->timeOfUpdate.c_str());

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.f, 0.3f, 1)));
                        }


                        if (ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())) {
                            //std::remove(absolute(path).c_str());
                            std::cout << absolute(path).c_str() << std::endl;
                            ModList.remove((*it));
                            Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence,ModList);
                            break;
                        }


                        for (auto ittt = ExtraOptions.begin(); ittt != ExtraOptions.end(); ++ittt) {
                            ImGui::TableNextColumn();
                            if (ImGui::Button(ittt->first.c_str()))
                                ittt->second = reinterpret_cast<bool *>(!ittt->second);
                        }

                        ImGui::TableNextRow();



                        i++;
                    }

                    ImGui::EndTable();
                }
            }

        };
    };
}
#endif //LAMP_LAMPARCHIVEDISPLAYHELPER_H
