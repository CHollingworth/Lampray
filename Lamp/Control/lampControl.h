//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPCONTROL_H
#define LAMP_LAMPCONTROL_H

#include <list>
#include <filesystem>
#include "../../game-data/gameControl.h"
#include "../Base/lampBase.h"
#include "../Filesystem/lampFS.h"
#include "lampGames.h"
#include "../../third-party/nfd/include/nfd.h"

namespace Lamp::Core{
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;
    /**
    * @brief The `lampControl` class manages Lamp's control and user interface.
    *
    * The `lampControl` class provides access to various control and user interface-related functionalities in Lamp.
    */
    class lampControl{
    public:
        /**
         * @brief Gets the singleton instance of the `lampControl` class.
         *
         * @return The reference to the singleton instance.
         */
        static lampControl& getInstance()
        {
            static lampControl instance;
            return instance;
        }

        lampControl(lampControl const&) = delete;
        void operator=(lampControl const&)  = delete;

        /**
         * @brief Gets the current formatted time and date as a string.
         *
         * @return The formatted time and date string.
         */
        static lampString getFormattedTimeAndDate();


        lampHex Colour_SearchHighlight = ImVec4(0.3f, 0.f, 0.3f, 1);
        /**
        * @brief The `lampArchiveDisplayHelper` struct provides helper methods for displaying archives.
        */
        struct lampArchiveDisplayHelper{
        private:
            std::list<std::string> columnNames{"Enabled","Mod Name", "Mod Type", "Load Order","Last Updated" ,"Remove Mod"};
            std::vector<Base::lampMod::Mod *>& ModList;
            std::vector<std::string> typeNames;
            std::list<std::pair<std::string,bool *>> ExtraOptions;
            std::string temp{"0"};


            /**
            * @brief Calculates the Levenshtein distance between two strings.
            *
            * @param s1 The first string.
            * @param s2 The second string.
            * @return The Levenshtein distance between the two strings.
             */
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

                if(std::strlen(lampConfig::getInstance().searchBuffer) <= 1 ){
                    return -1;
                }

                int closestMatchPosition = -1;
                int minDistance = std::numeric_limits<int>::max();

                for (auto it = ModList.begin(); it != ModList.end(); ++it) {
                    std::filesystem::path path((*it)->ArchivePath);
                    std::string cutname = path.filename().c_str();

                    while (cutname.length() < 100) {
                        cutname += '-';
                    }

                    int distance = calculateLevenshteinDistance(cutname, lampConfig::getInstance().searchBuffer);

                    if (distance < minDistance) {
                        minDistance = distance;
                        closestMatchPosition = std::distance(ModList.begin(), it);
                    }
                }

                return closestMatchPosition;
            }

            /**
             * @brief Moves an item up in the mod list.
             *
             * @param it Iterator pointing to the item to move.
             */
            void moveUp(std::vector<Base::lampMod::Mod*>::iterator& it) {
                if (ModList.size() <= 1 || it == ModList.begin()) {
                    return; // Nothing to move or already at the beginning
                }

                std::swap(*it, *(it - 1));
                --it;
            }
            /**
             * @brief Moves an item down in the mod list.
             *
             * @param it Iterator pointing to the item to move.
             */
            void moveDown(std::vector<Base::lampMod::Mod*>::iterator& it) {
                if (ModList.size() <= 1 || it == ModList.end() - 1) {
                    return; // Nothing to move or already at the end
                }

                std::swap(*it, *(it + 1));
                ++it;
            }

        public:


            /**
             * @brief Constructs a `lampArchiveDisplayHelper` object.
             *
             * @param ExtraColumnNames Extra column names to be displayed.
             * @param modList A vector of mod objects.
             * @param typeNames A vector of mod type names.
             * @param extraOptions A list of extra options.
             */
            lampArchiveDisplayHelper(std::list<std::string> ExtraColumnNames, std::vector<Base::lampMod::Mod *> &modList,
                    std::vector<std::string> typeNames,
                    std::list<std::pair<std::string, bool * >> extraOptions)
            : ModList(modList), typeNames(typeNames),
            ExtraOptions(extraOptions) {
                columnNames.insert(columnNames.end(), ExtraColumnNames.begin(), ExtraColumnNames.end());
            }
            /**
         * @brief Creates an ImGui menu for displaying mod archives.
         */
            void createImguiMenu(){
                ImGuiIO &io = ImGui::GetIO();
                ImGui::SetNextItemWidth(io.DisplaySize.x);
                if (ImGui::InputTextWithHint("##searcher","Type here to search your mods...", lampConfig::getInstance().searchBuffer, 250)) {
                    lampConfig::getInstance().listHighlight = findClosestMatchPosition();
                }
				
                ImGuiTableFlags mod_table_flags = 0;
                mod_table_flags |= ImGuiTableFlags_SizingStretchProp;
                mod_table_flags |= ImGuiTableFlags_Hideable; // allow hiding coumns via context menu
                mod_table_flags |= ImGuiTableFlags_Reorderable; // allow reordering columns

                if(ImGui::BeginTable(Lamp::Games::getInstance().currentGame->Ident().ShortHand, columnNames.size() + 1, mod_table_flags)) {
                    for (auto it = columnNames.begin(); it != columnNames.end(); ++it) {
                        ImGui::TableSetupColumn((*it).c_str());
                    }
					ImGui::TableHeadersRow();
                    ImGui::TableNextRow();

                    int dnd_move_from = -1, dnd_move_to = -1; // initialize position tracking vars for drag and drop functionality
                    int i = 0;
                    for (auto it = ModList.begin(); it != ModList.end(); ++it) {

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        if((*it)->enabled) {
                            if (ImGui::Button(("Enabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = false;
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                        }else{
                            if (ImGui::Button(("Disabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = true;
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                        }
                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,lampControl::getInstance().Colour_SearchHighlight);
                        }

                        std::filesystem::path path((*it)->ArchivePath);
                        std::string cutname = path.filename().c_str();
                        if(Lamp::Games::getInstance().currentGame->Ident().ShortHand == "BG3") {
                            size_t pos = cutname.find('-');
                            if (pos != std::string::npos) {
                                cutname.erase(pos);
                            }
                        }

                        ImGui::Text(cutname.c_str());
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                            ImGui::SetTooltip((*it)->ArchivePath);
                        }

                        // start drag and drop handling
                        ImGuiDragDropFlags src_flags = 0;
                        src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
                        src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
                        src_flags |= ImGuiDragDropFlags_SourceAllowNullID; // apparently needed?
                        //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
                        if (ImGui::BeginDragDropSource(src_flags))
                        {
                            if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                                ImGui::Text("Moving \"%s\"", cutname.c_str());
                            ImGui::SetDragDropPayload("MODLIST_DND", &i, sizeof(int));
                            ImGui::EndDragDropSource();
                        }

                        if (ImGui::BeginDragDropTarget())
                        {
                            ImGuiDragDropFlags target_flags = 0;
                            //target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
                            target_flags |= ImGuiDragDropFlags_SourceAllowNullID;
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODLIST_DND", target_flags))
                            {
                                dnd_move_from = *(const int*)payload->Data;
                                dnd_move_to = i;

                                auto* tmp = ModList[dnd_move_from];
                                // update the ModList (this also seems to update the UI immediately)
                                if(dnd_move_from > dnd_move_to){
                                    // if moving a mod to a higher position, shift things down and then place the moved mod
                                    for(int ind = dnd_move_from; ind > dnd_move_to; ind--){
                                        ModList[ind] = ModList[ind - 1];
                                    }
                                } else{
                                    // if moving a mod to a lower position, shift things up and then place the moved mod
                                    for(int ind = dnd_move_from; ind < dnd_move_to; ind++){
                                        ModList[ind] = ModList[ind + 1];
                                    }
                                }
                                ModList[dnd_move_to] = tmp;

                                // save the change to the profile's Mod_List
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                            ImGui::EndDragDropTarget();
                        }
                        // end drag and drop handling

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }

                        if (ImGui::BeginMenu((typeNames[(*it)->modType] + "##" + std::to_string(i)).c_str())) {
                            int y = 0;
                            ImGui::MenuItem(cutname.c_str());
                            ImGui::MenuItem("------------");

                            for (auto itt = typeNames.begin(); itt != typeNames.end(); ++itt) {
                                if (ImGui::MenuItem((*itt).c_str())) {
                                    (*it)->modType = y;
                                    Core::FS::lampIO::saveModList( Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList);
                                }
                                y++;
                            }
                            ImGui::EndMenu();
                        }


                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        if(ImGui::Button(("Up##"+std::to_string(i)).c_str())){
                            moveUp(it);
                            Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                        }
                        ImGui::SameLine();
                        ImGui::Text((std::to_string(i)).c_str());
                        ImGui::SameLine();
                        if(ImGui::Button(("Down##"+std::to_string(i)).c_str())){
                            moveDown(it);
                            Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                        }

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        ImGui::Text((*it)->timeOfUpdate);

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        if (ImGui::Button(("Delete Mod##" + std::to_string(i)).c_str())) {
                            //std::remove(absolute(path).c_str());
                            std::cout << absolute(path).c_str() << std::endl;
                            ModList.erase(it);
                            Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList,Games::getInstance().currentProfile);
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

        struct lampGameSettingsDisplayHelper{
            explicit lampGameSettingsDisplayHelper(
                    std::string displayString, std::string stringTarget, std::string toolTip, std::string keyPath);

        public:

            std::string  displayString;
            std::string  stringTarget;
            std::string  keyPath;
            std::string  toolTip;


            /**
         * @brief Creates an ImGui menu for displaying game settings.
         */
            void createImGuiMenu(){
                ImGui::Text(displayString.c_str());
                ImGui::Text(toolTip.c_str());
                if(ImGui::Button((stringTarget+"##"+displayString).c_str())) {
                    nfdchar_t *outPath = NULL;
                    nfdresult_t result = NFD_PickFolder(NULL, &outPath);

                    if (result == NFD_OKAY) {
                        puts(outPath);
                        Lamp::Core::FS::lampIO::saveKeyData(keyPath,outPath,Lamp::Games::getInstance().currentGame->Ident().ShortHand);
                        Lamp::Games::getInstance().currentGame->KeyInfo()[keyPath] = outPath;
                    } else if (result == NFD_CANCEL) {
                        puts("User pressed cancel.");
                    } else {
                        printf("Error: %s\n", NFD_GetError());
                    }
                }
            }

        };

        std::pair<int, int> deplopmentTracker; ///< A pair of integers representing deployment progress.
        std::string deploymentStageTitle; ///< The title of the current deployment stage.
        bool inDeployment = false; ///< Indicates whether Lamp is in the deployment process.


    private:
        /**
         * @brief Private constructor for the `lampControl` class.
         *
         * The constructor is private to ensure that only one instance of `lampControl` can exist.
         */
        lampControl(){};


    };


}
#endif //LAMP_LAMPCONTROL_H
