//
// Created by charles on 11/09/23.
//

#include <filesystem>
#include <iostream>
#include "BG3.h"
#include "../../imgui/imgui.h"
#include "../../lampFilesystem.h"

namespace Lamp {
    void Game::BG3::registerArchive(std::string Path) {
        Lamp::Core::lampMod::Mod  * newArchive = new Lamp::Core::lampMod::Mod{Path,ModType::NaN, false};
        ModList.push_back(newArchive);
        Lamp::Core::lampFilesystem::getInstance().saveModList(Core::lampConfig::BG3,ModList);
    }

    void Game::BG3::listArchives() {
        std::list<Lamp::Core::lampMod::Mod *>::iterator it;
        int i = 0;
        for (it = ModList.begin(); it != ModList.end(); ++it){

            std::filesystem::path path((*it)->ArchivePath);
            ImGui::TableNextColumn();
            ImGui::Text(path.filename().c_str());
            i++;

            ImGui::TableNextColumn();

            std::string Type = "";

            switch ((*it)->modType) {
                case ModType::BG3_ENGINE_INJECTION:
                    Type = "Engine Injection";
                    break;
                case ModType::BG3_MOD:
                    Type = "Standard Mod";
                    break;
                case ModType::BG3_BIN_OVERRIDE:
                    Type = "Bin Overwrite";
                    break;
                case ModType::BG3_DATA_OVERRIDE:
                    Type = "Data Overwrite";
                    break;
                case ModType::NaN:
                    Type = "Select Type";
                    break;
            }


            if (ImGui::BeginMenu((Type + "##" +std::to_string(i)).c_str())){

                if(ImGui::MenuItem("Engine Injection")){
                    (*it)->modType = ModType::BG3_ENGINE_INJECTION;
                    Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3,ModList);
                }
                if(ImGui::MenuItem("Standard Mod")){
                    (*it)->modType = ModType::BG3_MOD;
                    Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3,ModList);
                }
                if(ImGui::MenuItem("Bin Overwrite")){
                    (*it)->modType = ModType::BG3_BIN_OVERRIDE;
                    Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3,ModList);
                }
                if(ImGui::MenuItem("Data Overwrite")){
                    (*it)->modType = ModType::BG3_DATA_OVERRIDE;
                    Lamp::Core::lampFilesystem::getInstance().saveModList(Lamp::Core::lampConfig::BG3,ModList);
                }
                ImGui::EndMenu();
            }

            ImGui::TableNextColumn();
            if(ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())){
                std::remove(absolute(path).c_str());
                std::cout<<absolute(path).c_str() << std::endl;
                ModList.remove((*it));
                break;
            }

            ImGui::TableNextRow();
        }
    }
} // Lamp