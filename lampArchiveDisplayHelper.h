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
            std::list<std::string> columnNames{"Enabled","Mod Name", "Mod Type", "Load Order", "Remove Mod"};
            std::list<Lamp::Core::lampMod::Mod *>& ModList;
            std::vector<std::string> typeNames;
            lampConfig::Game gameRefrence;
            std::list<std::pair<std::string,std::function<void>&>> ExtraOptions;
            std::string temp{"0"};


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
                                   std::list<std::pair<std::string, std::function<void> &>> extraOptions)
                    : ModList(modList), typeNames(typeNames), gameRefrence(gameRefrence),
                      ExtraOptions(extraOptions) {
                columnNames.insert(columnNames.end(), ExtraColumnNames.begin(), ExtraColumnNames.end());

                // add assert for length here
            }

            void createImguiMenu(){
                if(ImGui::BeginTable(lampConfig::getInstance().GameShorthandMap[gameRefrence].c_str(), columnNames.size()+1,  ImGuiTableFlags_SizingStretchProp)) {
                    for (auto it = columnNames.begin(); it != columnNames.end(); ++it) {
                        ImGui::TableNextColumn();

                        ImGui::Text((*it).c_str());
                    }
                    ImGui::TableNextRow();
                    int i = 0;
                    for (auto it = ModList.begin(); it != ModList.end(); ++it) {

                        ImGui::TableNextColumn();



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

                        if (ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())) {
                            //std::remove(absolute(path).c_str());
                            std::cout << absolute(path).c_str() << std::endl;
                            ModList.remove((*it));
                            Lamp::Core::lampFilesystem::getInstance().saveModList(gameRefrence,ModList);
                            break;
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
