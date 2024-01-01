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

        struct NotificationColors{
            ImColor notificationBG;
            ImColor notificationBGHover;
        };

        NotificationColors DefaultTypeColors = {
            ImColor(100, 100, 100, 255),
            ImColor(50, 50, 50, 255),
        };
        NotificationColors InfoTypeColors = {
            ImColor(0, 100, 200, 255),
            ImColor(0, 100, 150, 255),
        };
        NotificationColors SuccessTypeColors = {
            ImColor(0, 175, 50, 255),
            ImColor(0, 125, 50, 255),
        };
        NotificationColors WarningTypeColors = {
            ImColor(175, 100, 0, 255),
            ImColor(150, 50, 0, 255),
        };
        NotificationColors ErrorTypeColors = {
            ImColor(200, 0, 0, 255),
            ImColor(150, 0, 0, 255),
        };

        void DisplayNotifications(){
            ImGuiStyle& imStyle = ImGui::GetStyle();

            int outerindex = 0;
            for (auto it = this->notifications.begin(); it != this->notifications.end(); ++it) {
                // get the coloring for this notification type
                NotificationColors notifColors = this->getNotificationColors(outerindex);

                if(this->notifications[outerindex].size() > 0){
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, notifColors.notificationBG.Value);
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, notifColors.notificationBGHover.Value);

                    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.0f)); // center-align text

                    std::string NOTIF_BAR_CONTAINER_ID = "NotificationBar_" + outerindex;
                    ImGui::BeginChild(NOTIF_BAR_CONTAINER_ID.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), ImGuiChildFlags_AutoResizeY);

                    // scale this child window ~25% larger
                    ImGui::SetWindowFontScale(1.25f);
                    for (auto itt = (*it).begin(); itt != (*it).end(); ++itt) {
                        if(ImGui::Selectable((*itt).c_str())){
                            this->clearNotification(outerindex, itt);
                            // go back 1 iteration as we just removed this item. Prevents a crash when clearning a notification.
                            itt--;
                        }
                    }

                    ImGui::EndChild();

                    ImGui::PopStyleVar(1);
                    ImGui::PopStyleColor(2);
                }
                outerindex++;
            }
        }

        void pushInfoNotification(std::string message){
            this->addNotification(INFO, message);
        }
        void pushSuccessNotification(std::string message){
            this->addNotification(SUCCESS, message);
        }
        void pushWarningNotification(std::string message){
            this->addNotification(WARNING, message);
        }
        void pushErrorNotification(std::string message){
            this->addNotification(ERROR, message);
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
        // NOTE: Do not customize the values as it will break init/display logic
        // (re-ordering everything except the NOTIFTYPE_END_PLACEHOLDER is fine)
        enum NotificationTypes{
            INFO,
            SUCCESS,
            WARNING,
            ERROR,

            NOTIFTYPE_END_PLACEHOLDER, // used for initializations. KEEP THIS AT THE END
        };

        std::array<NotificationColors, NOTIFTYPE_END_PLACEHOLDER> initNotificationColors(){
            std::array<NotificationColors, NOTIFTYPE_END_PLACEHOLDER> colArray = {};
            colArray[INFO] = InfoTypeColors;
            colArray[SUCCESS] = SuccessTypeColors;
            colArray[WARNING] = WarningTypeColors;
            colArray[ERROR] = ErrorTypeColors;

            return colArray;
        }

        // array containing a vector of notifications for each type, so typeList<notificationList>
        std::array<std::vector<std::string>, NOTIFTYPE_END_PLACEHOLDER> notifications;

        void addNotification(int notiftype, std::string message){
            this->notifications[notiftype].push_back(message);
        }
        void clearNotification(int notiftype, std::vector<std::string>::iterator item){
            this->notifications[notiftype].erase(item);
        }


        // first element is background, second is background hover (TODO: Better way to store these)
        std::array<NotificationColors, NOTIFTYPE_END_PLACEHOLDER> notificationColorValues = initNotificationColors();

        NotificationColors getNotificationColors(int notificationType){
            // if not in that array, return default colors
            if(notificationType < 0 || notificationType >= this->notificationColorValues.size()){
                return DefaultTypeColors;
            }

            return this->notificationColorValues[notificationType];
        }
    };
}
#endif //LAMP_LAMPNOTIFICATION_H
