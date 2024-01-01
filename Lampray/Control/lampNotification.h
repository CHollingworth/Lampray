//
// Created by SnazzyPanda on 2023-12-30.
//

#ifndef LAMP_LAMPNOTIFICATION_H
#define LAMP_LAMPNOTIFICATION_H

#include "../../third-party/imgui/imgui.h"
#include "../Lang/lampLang.h"

namespace Lamp::Core{
    class lampNotification{
    public:

        static lampNotification& getInstance()
        {
            static lampNotification instance;
            return instance;
        }


// TODO:
// - Restructure to allow defining the order notifications get displayed
// - decouple type access from enum? May not be necessary
// - simplify initlization of "notifications" vector
// - define colors, in relation to the enums/vector position
// - probably not, but possible allow configuring colors
// - styling tweaks, specifically giving the notification more padding (clickable padding)

        enum NotificationTypes{
            INFO,
            WARNING,
            ERROR,
            SUCCESS,
        };

        void DisplayNotifications(){
            ImGuiStyle& imStyle = ImGui::GetStyle();
            /*
            ImGui::PushStyleColor(ImGuiCol_WindowBg, warningColor);

            ImGui::PushStyleColor(ImGuiCol_HeaderActive, warningColor);
            ImGui::PushStyleColor(ImGuiCol_Header, warningColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, warningColor);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, warningColor);
            ImGui::PushStyleColor(ImGuiCol_TitleBg, warningColor);

            ImGui::PushStyleColor(ImGuiCol_MenuBarBg, warningColor);
            ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, warningColor);
            ImGui::PushStyleColor(ImGuiCol_TableRowBg, warningColor);
            */
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, sectionBGColor);
            //ImGui::PushStyleColor(ImGuiCol_HeaderHovered, warningColor); // this works...


            //ImGui::PushStyleColor(ImGuiCol_Button, warningColor);
            //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, warningColor);
            int outerindex = 0;
            for (auto it = this->notifications.begin(); it != this->notifications.end(); ++it) {
                // TODO: DEFINE COLORS FOR EACH TYPE AND HAVE WAY TO GRAB THEM
                ImVec4 sectionBGColor = ImVec4(
                    lampCustomise::getInstance().floatMap[6][0],
                    lampCustomise::getInstance().floatMap[6][1],
                    lampCustomise::getInstance().floatMap[6][2],
                    lampCustomise::getInstance().floatMap[6][3]
                );
                ImVec4 warningColor = ImVec4(
                    lampCustomise::getInstance().floatMap[7][0],
                    lampCustomise::getInstance().floatMap[7][1],
                    lampCustomise::getInstance().floatMap[7][2],
                    lampCustomise::getInstance().floatMap[7][3]
                );

                if(this->notifications[outerindex].size() > 0){
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, sectionBGColor);
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, warningColor);

                    //ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.5f, 0.5f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(imStyle.ItemSpacing.x, imStyle.CellPadding.y * 2)); // does not add padding as I want right now...
                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.0f)); // center-align text

                    ImGui::BeginChild("NotificationBar", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), ImGuiChildFlags_AutoResizeY);

                    for (auto itt = (*it).begin(); itt != (*it).end(); ++itt) {
                        if(ImGui::Selectable((*itt).c_str())){
                            //std::cout << "\n";
                            //std::cout << warningColor << "\n";
                            //std::cout << "Hit the selectable...\n";

                            this->clearNotification(outerindex, itt);

                            // go back 1 iteration as we just removed this item. Prevents a crash when clearning a notification.
                            itt--;
                        }
                    }

                    ImGui::EndChild();
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor(2);
                }
                outerindex++;
            }
        }

        void pushInfoNotification(std::string message){
            this->addNotification(INFO, message);
        }
        void pushWarningNotification(std::string message){
            this->addNotification(WARNING, message);
        }
        void pushErrorNotification(std::string message){
            this->addNotification(ERROR, message);
        }
        void pushSuccessNotification(std::string message){
            this->addNotification(SUCCESS, message);
        }
        void pushNotification(std::string notiftype, std::string message){

            if(notiftype == "info"){
                this->pushInfoNotification(message);
            } else if(notiftype == "warning"){
                this->pushWarningNotification(message);
            } else if(notiftype == "error"){
                this->pushErrorNotification(message);
            } else if(notiftype == "success"){
                this->pushSuccessNotification(message);
            } else{
                std::cout << "Invalid notification type given: " << notiftype << "\n";
            }
        }

    private:
        // vector containing a vector of notifications for each type, so typeList<notificationList>
        std::vector<std::vector<std::string>> notifications = {
            {}, // info
            {}, // warning
            {}, // error
            {}, // success?
        };

        // TODO: maybe an array of vectors, so a vector for each type


        void addNotification(int notiftype, std::string message){
            this->notifications[notiftype].push_back(message);
        }
        void clearNotification(int notiftype, std::vector<std::string>::iterator item){
            this->notifications[notiftype].erase(item);
        }
    };
}
#endif //LAMP_LAMPNOTIFICATION_H
