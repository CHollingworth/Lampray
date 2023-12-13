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
        lampHex Colour_ButtonAlt = ImVec4(0.1f, 0.6f, 0.3f, 1);
        /**
        * @brief The `lampArchiveDisplayHelper` struct provides helper methods for displaying archives.
        */
        struct lampArchiveDisplayHelper{
        private:
            std::list<std::string> columnNames{"Enabled","Mod Name", "Mod Type", "Load Order","Last Updated" ,"Remove Mod"};
            std::vector<Base::lampMod::Mod *>& ModList;
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

            /**
             * @brief Moves an item to specific position in the mod list.
             *
             * @param it Iterator pointing to the item to move.
             * @param position Integer position to move the item to.
             */
            void moveModTo(std::vector<Base::lampMod::Mod*>::iterator& it, int position) {
                int currentPos = it - ModList.begin();
                if(currentPos > position){
                    for(int ind = currentPos; ind > position; ind--){
                        moveUp(it);
                    }
                } else if(currentPos < position){
                    for(int ind = currentPos; ind < position; ind++){
                        moveDown(it);
                    }
                }
            }

        public:


            /**
             * @brief Constructs a `lampArchiveDisplayHelper` object.
             *
             * @param ExtraColumnNames Extra column names to be displayed.
             * @param modList A vector of mod objects.
             * @param extraOptions A list of extra options.
             */
            lampArchiveDisplayHelper(std::list<std::string> ExtraColumnNames, std::vector<Base::lampMod::Mod *> &modList,
                    std::list<std::pair<std::string, bool * >> extraOptions)
            : ModList(modList),
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

                    int i = 0;
                    for (auto it = ModList.begin(); it != ModList.end(); ++it) {

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        if((*it)->enabled) {
                            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)lampControl::getInstance().Colour_ButtonAlt);
                            if (ImGui::Button(("Enabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = false;
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                            ImGui::PopStyleColor(1);
                        }else{
                            if (ImGui::Button(("Disabled##" + std::to_string(i)).c_str())) {
                                (*it)->enabled = true;
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
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
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                        }

                        auto contextId = "MOD_NAME_CONTEXT_" + std::to_string(i);
                        auto renamePopupId = "RENAME_MOD_" + std::to_string(i);
                        if (ImGui::BeginPopupContextItem(contextId.c_str())){
                            if(ImGui::Selectable("Move to top")){
                                moveModTo(it, 0);
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }
                            if(ImGui::Selectable("Move to bottom")){
                                moveModTo(it, std::distance(ModList.begin(), ModList.end()));
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                            }

                            if(ImGui::Selectable("Add mod separator")){
                                // TODO: Allow right-click outside of table (specifically below, in case you don't have many mods)
                                // TODO: allow collapsing everything below a separator, up to the next separator (somewhat like MO2)
                                Lamp::Games::getInstance().currentGame->registerArchive("====================================================", Lamp::Games::getInstance().currentGame->SeparatorModType());
                                // move the separator (now at the end of the ModList) to the index the user interacted at
                                auto tmpSeparator = ModList.end() - 1;
                                moveModTo(tmpSeparator, i);
                            }

                            // restsrict to only mod separators for now as we do not store a separate "name", just a file path for mods
                            if((*it)->modType == Lamp::Games::getInstance().currentGame->SeparatorModType()){
                                // using a button as a Selectable did not work for some reason
                                if(ImGui::Button("Rename", ImVec2(120, 0))){
                                    ImGui::OpenPopup(renamePopupId.c_str());
                                }
                            }

                            if(ImGui::BeginPopupModal(renamePopupId.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)){
                                // 200 characters should hopefully be more than enough
                                static char buf[200];
                                ImGui::InputTextWithHint("##", cutname.c_str(), buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_None);
                                ImGui::Separator();

                                if (ImGui::Button("Save")) {
                                    (*it)->ArchivePath = buf;
                                    Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList,Games::getInstance().currentProfile);

                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::SetItemDefaultFocus();
                                ImGui::SameLine();
                                // right-align the cancel button to help avoid potential misclicks
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Cancel").x - ImGui::GetStyle().ItemSpacing.x);
                                if (ImGui::Button("Cancel")) {
                                    // Do nothing
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::EndPopup();
                            } // end rename popup modal

                            ImGui::EndPopup();
                        }
                        //ImGui::OpenPopupOnItemClick("MODTABLE_CONTEXT", ImGuiPopupFlags_MouseButtonRight);

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
                                auto movingMod = ModList.begin() + *(const int*)payload->Data; // get original position from the payload data
                                moveModTo(movingMod, i);
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

                        if (ImGui::BeginMenu((Lamp::Games::getInstance().currentGame->getModTypesMap()[(*it)->modType] + "##" + std::to_string(i)).c_str())) {
                            int y = 0;
                            ImGui::MenuItem(cutname.c_str());
                            ImGui::MenuItem("------------");

                            for (auto itt = Lamp::Games::getInstance().currentGame->getModTypes().begin(); itt != Lamp::Games::getInstance().currentGame->getModTypes().end(); ++itt){
                                if (ImGui::MenuItem(((*itt).second).c_str())) {
                                    (*it)->modType = (*itt).first;
                                    Core::FS::lampIO::saveModList( Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList);
                                }
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
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                        }
                        ImGui::SameLine();
                        ImGui::Text((std::to_string(i)).c_str());
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                        }
                        ImGui::SameLine();
                        if(ImGui::Button(("Down##"+std::to_string(i)).c_str())){
                            moveDown(it);
                            Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList, Games::getInstance().currentProfile);
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                        }

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                        ImGui::Text((*it)->timeOfUpdate);
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                        }

                        ImGui::TableNextColumn();
                        if(lampConfig::getInstance().listHighlight == i) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, lampControl::getInstance().Colour_SearchHighlight);
                        }


                            ImGui::BeginDisabled((*it)->enabled);

                            if (ImGui::Button(("Remove Mod##" + std::to_string(i)).c_str())) {
                                // I don't think I like this way of checking what the mod type is, but it works for now.
                                if((*it)->modType != Lamp::Games::getInstance().currentGame->SeparatorModType()){
                                    int deleteResult = std::remove(absolute(path).c_str());
                                    if(deleteResult != 0){
                                        std::cout << "Error deleting file: " << absolute(path).c_str() << "\n   Error msg: " << strerror(errno) << "\n";
                                    }
                                }

                                std::cout << absolute(path).c_str() << std::endl;
                                ModList.erase(it);
                                Core::FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand, ModList,Games::getInstance().currentProfile);
                                ImGui::EndDisabled(); // fixes a crash when deleting items (when at least 1 mod has been enabled)
                                break;
                            }

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && (*it)->enabled) {
                                ImGui::SetTooltip("Only disabled mods can be removed.");
                            }
                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                            }

                            ImGui::EndDisabled();


                        for (auto ittt = ExtraOptions.begin(); ittt != ExtraOptions.end(); ++ittt) {
                            ImGui::TableNextColumn();
                            if (ImGui::Button(ittt->first.c_str()))
                                ittt->second = reinterpret_cast<bool *>(!ittt->second);

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lampControl::getInstance().Colour_SearchHighlight);
                            }
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
