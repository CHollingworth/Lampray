//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPBASE_H
#define LAMP_LAMPBASE_H

#include <string>
#include <pugixml.hpp>
#include <utility>
#include <vector>
#include <sstream>
#include "../../third-party/imgui/imgui.h"
#include <iomanip>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <list>
#include <queue>
#include <fstream>

namespace Lamp::Core::Base{

    class lampTypes{
    public:
        class lampString {
        private:
            std::string data;

        public:
            lampString(std::string  str) : data(std::move(str)) {}
            lampString(const char* str) : data(str) {}
            lampString() : data("") {}

            // Custom operator to concatenate with const char*
            lampString operator+(const char* other) const {
                return {data + other};
            }

            // Custom operator to concatenate
            lampString operator+(const lampString& other) const {
                return {data + other.data};
            }

            friend lampString operator+(const std::string& str1, const lampString& str2) {
                return {str1 + str2.data};
            }

            // Custom operator to concatenate with std::string
            lampString operator+(const std::string& other) const {
                return {data + other};
            }


            // Custom operator to assign from const char*
            lampString& operator=(const char* other) {
                data = other;
                return *this;
            }

            // Custom operator to assign from std::string
            lampString& operator=(const std::string& other) {
                data = other;
                return *this;
            }

            bool operator<(const lampString& other) const {
                return data < other.data;
            }

            // Custom operator to convert to const char*
            operator const char*() const {
                return data.c_str();
            }

            // Custom operator to convert to std::string
            operator std::string() const {
                return data;
            }

            operator std::filesystem::path() const{
                return std::filesystem::path(data);
            };




            bool as_bool(){
                if(data == "1" || data == "true"){
                    return true;
                }else{
                    return false;
                }
            }


        };
        class lampHexAlpha {
        public:
            lampHexAlpha() : color_(0.0f, 0.0f, 0.0f, 1.0f) {}

            // Constructor that takes an ImVec4
            lampHexAlpha(const ImVec4& color) : color_(color) {}

            // Constructor that takes a hex string
            lampHexAlpha(const std::string& hexColor) {
                color_ = HexStringToImVec4(hexColor);
            }

            // Implicit conversion operator to ImVec4
            operator ImVec4() const {
                return color_;
            }

            operator ImU32() const {
                return ImGui::ColorConvertFloat4ToU32(color_);
            }

            // Implicit conversion operator to hex string
            operator std::string() const {
                return ImVec4ToHexString(color_);
            }

        private:
            ImVec4 color_;

            std::string ImVec4ToHexString(const ImVec4& color) const {
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

            ImVec4 HexStringToImVec4(const std::string& hexColor) const {
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
        class lampReturn {
        public:
            int returnClause;
            lampString returnReason = "";

            lampReturn(int returnClause, lampString returnReason)
                    : returnClause(returnClause), returnReason(returnReason) {}

            lampReturn(int returnClause) : returnClause(returnClause) {}

            lampReturn(){}

            operator bool() const {
                if(returnClause == 1){
                    return true;
                }else{
                    return false;
                }
            }

            operator int() const {
                return (int)returnClause;
            }

            operator lampString() const {
                return returnReason;
            }

            operator std::string() const{
                return (std::string) returnReason;
            }
        };
        struct lampIdent{
        public:
            lampTypes::lampString ReadableName;
            lampTypes::lampString ShortHand;
            lampIdent(lampTypes::lampString readableName, lampTypes::lampString shortHand)
                    : ReadableName(readableName), ShortHand(shortHand) {}
        };
    };

    class lampMod {
    public:
        /**
        * @brief The Mod struct represents information about a mod.
        */
        struct Mod{
        public:
            /**
            * @brief The path to the mod's archive.
            */
            lampTypes::lampString ArchivePath;
            /**
            * @brief The type of the mod.
            */
            int modType;
            /**
            * @brief Indicates whether the mod is enabled.
            */
            bool enabled;

            /**
             * @brief Stores a string of the last time the file updated
             */
            lampTypes::lampString timeOfUpdate = "Unknown";

            /**
            * @brief Serialize the Mod struct to an XML node.
            *
            * @param doc The parent XML node to which this Mod will be serialized.
            * @return The serialized XML node.
            */
            pugi::xml_node serialize(pugi::xml_node doc) const {
                pugi::xml_node modNode = doc.append_child("Mod");

                modNode.append_attribute("ArchivePath").set_value(ArchivePath);
                modNode.append_attribute("modType").set_value(modType);
                modNode.append_attribute("enabled").set_value(enabled);
                modNode.append_attribute("timeOfUpdate").set_value(timeOfUpdate);

                return modNode;
            }

            /**
            * @brief Construct a new Mod object.
            *
            * @param archivePath The path to the mod's archive.
            * @param modType The type of the mod.
            * @param enabled Indicates whether the mod is enabled.
            */
            Mod(lampTypes::lampString archivePath, int modType, bool enabled)
                    : ArchivePath(archivePath), modType(modType), enabled(enabled) {
            }
        };
        /**
        * @brief The Profile helper struct
        */
        class Profile{
        public:
            static void addValue(std::string& inputString, const std::string& newValue) {
                if (!inputString.empty()) {
                    inputString += "~";
                }
                inputString += newValue;
            }

            static void removeValue(std::string& inputString, const std::string& valueToRemove) {
                size_t pos = inputString.find(valueToRemove);
                if (pos != std::string::npos) {
                    // Remove the value and the preceding tilde
                    inputString.erase(pos-1, valueToRemove.length() + 1);
                }
            }

            static std::vector<std::string> splitString(const std::string& inputString) {
                std::vector<std::string> values;
                std::istringstream iss(inputString);
                std::string token;

                while (std::getline(iss, token, '~')) {
                    values.push_back(token);
                }

                return values;
            }
        };
    };

    class lampLog{
    public:
        static lampLog& getInstance()
        {
            static lampLog instance;
            return instance;
        }

        lampLog(lampLog const&) = delete;
        void operator=(lampLog const&)  = delete;


        enum warningLevel{
            LOG,
            WARNING,
            ERROR
        };

        enum errorCode{
            LMP_UNK = 0,
            LMP_NODIRCREATION,
            LMP_NO7ZP,
            LMP_NOFILEDROP,
            LMP_CLEANUPFAILED,
            LMP_PREDEPLOYFAILED,
            LMP_DEOPLYMENTFAILED,
            LMP_MODLISTSAVEFAILED,
            LMP_KEYSAVEFAILED,
            LMP_KEYLOADFAILED,
            LMP_EXTRACTIONFALED,
            LMP_NOCONFIG

        };

        std::list<std::string> poplist = {};

        void log(lampTypes::lampString data,warningLevel warningLevel = warningLevel::LOG,bool pop = false, errorCode errorCode = errorCode::LMP_UNK){
            lampTypes::lampString ping;
            switch (warningLevel) {
                case LOG:
                    ping = "[LAMP:LOG:"+ std::to_string(errorCode) +"] "+data;
                    break;
                case WARNING:
                    ping = "[LAMP:WARNING:"+ std::to_string(errorCode) +"] "+data;
                    break;
                case ERROR:
                    ping = "[LAMP:ERROR:"+ std::to_string(errorCode) +"] "+data;
                    break;
            }
            std::cout << ping << std::endl;


            std::ofstream outputFile("lamp.log", std::ios::app);
            if(outputFile) {
                std::string lineToAdd = "This is a new line.";
                outputFile << ping << std::endl;
                outputFile.close();
            }


            if(pop){
              //  if(!lampConfig::getInstance().ShowIntroMenu) {
                    poplist.push_front(ping);
            //    }
            }
        }



        bool showWarns(){
            for (auto it = poplist.begin(); it != poplist.end();) {
                if (!poplist.empty()) {
                    lampTypes::lampString x = *it;
                    ImGuiIO &io = ImGui::GetIO();
                    ImGui::SetNextWindowSize(io.DisplaySize, 0);
                    ImGui::SetNextWindowPos(ImVec2(0, 0));

                    ImGuiWindowFlags windowFlags = 0;
                    windowFlags += ImGuiWindowFlags_NoMove;
                    windowFlags += ImGuiWindowFlags_NoResize;
                    windowFlags += ImGuiWindowFlags_NoCollapse;
                    windowFlags += ImGuiWindowFlags_NoTitleBar;
                    windowFlags += ImGuiWindowFlags_MenuBar;

                    ImGui::Begin(x, NULL, windowFlags);

                    ImGui::Text(x);
                    ImGui::Separator();

                    ImGui::Text("If an error persists please create an issue on GitHub.");
                    if (ImGui::Button("Okay")) {
                        poplist.erase(it);
                    } else {
                        ++it; // Move to the next element
                    }

                    ImGui::End();
                }
                break;
            }
        }

    private:
        lampLog(){}
    };

    class lampExecutor {
    public:
        lampExecutor(std::function<void(const lampMod::Mod *)> code) : code_(code), itemCount(0), finishedCount(0) {}

        int itemCount;
        int finishedCount;

        void execute(const std::vector<lampMod::Mod *>& data) {
            lampLog::getInstance().log("ModList Executor: Starting", lampLog::warningLevel::LOG);
            itemCount = data.size();
            for (const lampMod::Mod * item : data) {
                std::thread t([this, item]() {
                    if (code_) {
                        code_(item);
                    }
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        finishedCount++;
                    }
                    condition_.notify_all();
                });
                t.detach();
            }
            waitForCompletion();
        }

        void waitForCompletion() {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return finishedCount == itemCount;
            });
        }

    private:
        std::function<void(const lampMod::Mod *)> code_;
        std::mutex mutex_;
        std::condition_variable condition_;
    };

    class LampSequencer {
    public:

        // Add a lambda expression to the queue
        static void add(const std::string& queueName, std::function<lampTypes::lampReturn()> lambda) {
            getQueue(queueName).push(lambda);
        }

        // Run all lambda expressions in the queue
        static lampTypes::lampReturn run(const std::string& queueName) {
            lampLog::getInstance().log("Queue '"+queueName+"': Starting", lampLog::warningLevel::LOG);
            lampTypes::lampReturn result;
            while (!getQueue(queueName).empty()) {
                std::function<lampTypes::lampReturn()> lambda = getQueue(queueName).front();
                getQueue(queueName).pop();
                result = lambda();
                logResult(queueName, result);
                if (result.returnClause == -1 || result.returnClause == 0) {
                    break;  // Stop execution on critical failure
                }
            }
            return result;
        }

    private:
        static std::queue<std::function<lampTypes::lampReturn()>>& getQueue(const std::string& queueName) {
            static std::unordered_map<std::string, std::queue<std::function<lampTypes::lampReturn()>>> queues;
            return queues[queueName];
        }

        static void logResult(const std::string& queueName, const lampTypes::lampReturn& result) {
            std::string logMessage = "Queue '" + queueName + "': ";
            if (result.returnClause == 0) {
                logMessage += "Failed - " + (std::string)result.returnReason;
                lampLog::getInstance().log(logMessage, lampLog::warningLevel::ERROR, true);
            } else if (result.returnClause == 1) {
                logMessage += "Success";
                lampLog::getInstance().log(logMessage, lampLog::warningLevel::LOG);
            } else if (result.returnClause == -1) {
                logMessage += "Critically Failed - " + (std::string)result.returnReason;
                lampLog::getInstance().log(logMessage, lampLog::warningLevel::ERROR, true);
            }

        }
    };
}
#endif //LAMP_LAMPBASE_H
