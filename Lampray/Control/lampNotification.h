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

        // scale notifications about 25% larger than the rest of the content
        float notificationScale = 1.25f;

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

                    ImGui::SetWindowFontScale(this->notificationScale);
                    for (auto itt = (*it).begin(); itt != (*it).end(); ++itt) {
                        std::string wrappedText = (*itt);
                        auto spaceAvail = ImGui::GetContentRegionAvail().x;
                        auto spaceNeeded = ImGui::CalcTextSize(wrappedText.c_str()).x;

                        // attempt to manually wrap the text for long messages
                        wrappedText = this->manuallyWrapText(wrappedText);

                        if(ImGui::Selectable(wrappedText.c_str())){
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


        std::string manuallyWrapText(std::string textToWrap){
            auto spaceAvail = ImGui::GetContentRegionAvail().x;
            auto spaceNeeded = ImGui::CalcTextSize(textToWrap.c_str()).x;
            // no wrapping needed
            if(spaceNeeded < spaceAvail){
                return textToWrap;
            }

            std::vector<int> spacesToWrapAt = {};
            int approxSizePerByte = std::ceil(spaceNeeded / textToWrap.size());
            int charsThatFit = std::floor(spaceAvail / approxSizePerByte);
            int prevspace = textToWrap.rfind(" ", charsThatFit);

            // calculate the spots to try to wrap at... NOTE: This does not work if there are not well palced spaces!
            for(auto i = charsThatFit; i < textToWrap.size(); i+= charsThatFit){
                prevspace = textToWrap.rfind(" ", i);

                // if we got a repeat prevspace, we are looping the same stuff
                if(std::find(spacesToWrapAt.begin(), spacesToWrapAt.end(), prevspace) != spacesToWrapAt.end()){
                    // we are looping, so just add a wrap anyway to continue...
                    prevspace = i;
                }

                spacesToWrapAt.push_back(prevspace);
                i = prevspace;
            }

            int offsetForInserts = 0;
            // add newlines to manually wrap the text
            for(auto it = spacesToWrapAt.begin(); it != spacesToWrapAt.end(); ++it){
                int strpos = (*it) + offsetForInserts;
                std::string spaceCheckString;
                spaceCheckString += textToWrap.at(strpos);

                if(spaceCheckString == " "){
                    textToWrap.replace(strpos, 1, "\n");
                } else{
                    // character was not a space, so insert instead of replace
                    textToWrap.insert(strpos, "\n");
                    offsetForInserts += 1;
                }
            }

            return textToWrap;
        }
    };
}
#endif //LAMP_LAMPNOTIFICATION_H
