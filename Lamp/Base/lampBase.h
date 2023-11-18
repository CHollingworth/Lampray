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
    /**
     * @brief A holding class for some of the Lamp framework's core types.
     *
     */
    class lampTypes{
    public:
        /**
        * @brief A custom string class designed for use in the Lamp framework.
        *
        * This class provides custom functionality for string manipulation and conversion.
        */
        class lampString {
        private:
            std::string data; ///< The underlying std::string data.

        public:
            /**
             * @brief Constructs a lampString from a std::string.
             *
             * @param str The std::string to initialize the lampString with.
             */
            lampString(std::string str) : data(std::move(str)) {}

            /**
             * @brief Constructs a lampString from a C-style string (const char*).
             *
             * @param str The C-style string to initialize the lampString with.
             */
            lampString(const char* str) : data(str) {}

            /**
             * @brief Constructs an empty lampString.
             */
            lampString() : data("") {}

            /**
             * @brief Custom operator to concatenate a lampString with a C-style string (const char*).
             *
             * @param other The C-style string to concatenate with.
             * @return A new lampString containing the concatenated result.
             */
            lampString operator+(const char* other) const {
                return {data + other};
            }

            /**
             * @brief Custom operator to concatenate two lampStrings.
             *
             * @param other The lampString to concatenate with.
             * @return A new lampString containing the concatenated result.
             */
            lampString operator+(const lampString& other) const {
                return {data + other.data};
            }

            /**
             * @brief Custom operator to concatenate a std::string with a lampString.
             *
             * @param str1 The std::string to concatenate with.
             * @param str2 The lampString to concatenate to the end.
             * @return A new lampString containing the concatenated result.
             */
            friend lampString operator+(const std::string& str1, const lampString& str2) {
                return {str1 + str2.data};
            }

            /**
             * @brief Custom operator to concatenate a lampString with a std::string.
             *
             * @param other The std::string to concatenate with.
             * @return A new lampString containing the concatenated result.
             */
            lampString operator+(const std::string& other) const {
                return {data + other};
            }

            /**
             * @brief Custom operator to assign a C-style string (const char*) to a lampString.
             *
             * @param other The C-style string to assign.
             * @return A reference to the modified lampString.
             */
            lampString& operator=(const char* other) {
                data = other;
                return *this;
            }

            /**
             * @brief Custom operator to assign a std::string to a lampString.
             *
             * @param other The std::string to assign.
             * @return A reference to the modified lampString.
             */
            lampString& operator=(const std::string& other) {
                data = other;
                return *this;
            }

            /**
             * @brief Custom comparison operator for lampStrings (less than).
             *
             * @param other The lampString to compare with.
             * @return true if this lampString is less than the other, false otherwise.
             */
            bool operator<(const lampString& other) const {
                return data < other.data;
            }

            /**
             * @brief Custom operator to convert a lampString to a C-style string (const char*).
             *
             * @return A C-style string representing the content of the lampString.
             */
            operator const char*() const {
                return data.c_str();
            }

            /**
             * @brief Custom operator to convert a lampString to a std::string.
             *
             * @return A std::string representing the content of the lampString.
             */
            operator std::string() const {
                return data;
            }

            /**
             * @brief Custom operator to convert a lampString to a std::filesystem::path.
             *
             * @return A std::filesystem::path representing the content of the lampString.
             */
            operator std::filesystem::path() const {
                return std::filesystem::path(data);
            };

            /**
             * @brief Converts the lampString to a boolean value.
             *
             * This function interprets "1" or "true" as `true`, and anything else as `false`.
             *
             * @return true if the string is "1" or "true," false otherwise.
             */
            bool as_bool() {
                return (data == "1" || data == "true");
            }
        };

        /**
        * @brief A custom color class designed for use in the Lamp framework, representing a color with alpha (RGB-A).
        *
        * This class provides functionality to work with colors in RGBA format and supports conversion to/from hex strings.
        */
        class lampHexAlpha {
        public:
            /**
             * @brief Constructs a lampHexAlpha with the default color (black with full alpha).
             */
            lampHexAlpha() : color_(0.0f, 0.0f, 0.0f, 1.0f) {}

            /**
             * @brief Constructs a lampHexAlpha from an ImVec4 color.
             *
             * @param color The ImVec4 color to initialize the lampHexAlpha with.
             */
            lampHexAlpha(const ImVec4& color) : color_(color) {}

            /**
             * @brief Constructs a lampHexAlpha from a hex color string.
             *
             * @param hexColor The hex color string (e.g., "RRGGBB-AA") to initialize the lampHexAlpha with.
             */
            lampHexAlpha(const std::string& hexColor) {
                color_ = HexStringToImVec4(hexColor);
            }

            /**
             * @brief Implicit conversion operator to ImVec4.
             *
             * @return An ImVec4 representing the color.
             */
            operator ImVec4() const {
                return color_;
            }

            /**
             * @brief Implicit conversion operator to ImU32 (ImGui color format).
             *
             * @return An ImU32 representing the color.
             */
            operator ImU32() const {
                return ImGui::ColorConvertFloat4ToU32(color_);
            }

            /**
             * @brief Implicit conversion operator to hex color string.
             *
             * @return A hex color string (e.g., "RRGGBB-AA").
             */
            operator std::string() const {
                return ImVec4ToHexString(color_);
            }

        private:
            ImVec4 color_; ///< The RGBA color represented by ImVec4.

            /**
             * @brief Converts an ImVec4 color to a hex color string.
             *
             * @param color The ImVec4 color to convert.
             * @return A hex color string in the format "RRGGBB-AA".
             */
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

            /**
             * @brief Converts a hex color string to an ImVec4 color.
             *
             * @param hexColor The hex color string in the format "RRGGBB-AA" to convert.
             * @return An ImVec4 color.
             */
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

        /**
        * @brief A custom class for representing return values with an associated reason.
        *
        * This class is designed for use in the Lamp framework to encapsulate return values along with an optional reason.
        */
        class lampReturn {
        public:
            int returnClause;      ///< The return clause indicating the outcome of an operation.
            lampString returnReason = ""; ///< The optional return reason.

            /**
             * @brief Constructs a lampReturn with a return clause and an optional return reason.
             *
             * @param returnClause The return clause indicating the outcome of an operation.
             * @param returnReason The optional return reason as a lampString.
             */
            lampReturn(int returnClause, lampString returnReason)
                    : returnClause(returnClause), returnReason(returnReason) {}

            /**
             * @brief Constructs a lampReturn with a return clause and no return reason.
             *
             * @param returnClause The return clause indicating the outcome of an operation.
             */
            lampReturn(int returnClause) : returnClause(returnClause) {}

            /**
             * @brief Default constructor for a lampReturn with no return clause and no return reason.
             */
            lampReturn() {}

            /**
             * @brief Conversion operator to bool.
             *
             * @return true if the return clause is equal to 1, false otherwise.
             */
            operator bool() const {
                return returnClause == 1;
            }

            /**
             * @brief Conversion operator to int.
             *
             * @return The return clause as an integer.
             */
            operator int() const {
                return returnClause;
            }

            /**
             * @brief Conversion operator to lampString.
             *
             * @return The return reason as a lampString.
             */
            operator lampString() const {
                return returnReason;
            }

            /**
             * @brief Conversion operator to std::string.
             *
             * @return The return reason as a std::string.
             */
            operator std::string() const {
                return static_cast<std::string>(returnReason);
            }
        };

        /**
        * @brief A structure representing identification information for game control.
        *
        * This structure is used to provide readable and shorthand names for game identification purposes.
        */
        struct lampIdent {
            lampTypes::lampString ReadableName; ///< A human-readable name for the game.
            lampTypes::lampString ShortHand;    ///< A shorthand or abbreviated name for the game.

            /**
             * @brief Constructs a lampIdent with a readable name and a shorthand name.
             *
             * @param readableName The human-readable name for the game.
             * @param shortHand The shorthand or abbreviated name for the game.
             */
            lampIdent(lampTypes::lampString readableName, lampTypes::lampString shortHand)
                    : ReadableName(readableName), ShortHand(shortHand) {}
        };
    };

    /**
    * @brief A class for managing game mods in the Lamp framework.
    *
    * This class provides functionality for managing and serializing game mods.
    */
    class lampMod {
    public:
        /**
         * @brief The Mod struct represents information about a mod.
         */
        struct Mod {
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
             * @brief Stores a string of the last time the file updated.
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
         * @brief The Profile helper struct for managing profile-related operations.
         */
        class Profile {
        public:
            /**
             * @brief Adds a value to an input string, separated by a tilde (~) delimiter.
             *
             * @param inputString The input string to which the value will be added.
             * @param newValue The value to add to the input string.
             */
            static void addValue(std::string& inputString, const std::string& newValue) {
                if (!inputString.empty()) {
                    inputString += "~";
                }
                inputString += newValue;
            }

            /**
             * @brief Removes a specified value from an input string.
             *
             * @param inputString The input string from which the value will be removed.
             * @param valueToRemove The value to remove from the input string.
             */
            static void removeValue(std::string& inputString, const std::string& valueToRemove) {
                size_t pos = inputString.find(valueToRemove);
                if (pos != std::string::npos) {
                    // Remove the value and the preceding tilde
                    inputString.erase(pos - 1, valueToRemove.length() + 1);
                }
            }

            /**
             * @brief Splits a delimited input string into a vector of strings.
             *
             * @param inputString The input string to split.
             * @return A vector of strings containing the split values.
             */
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

    /**
    * @brief A singleton class for logging messages and displaying warnings in the Lamp framework.
    *
    * This class provides a centralized logging system and a mechanism to display warning messages.
    * It is implemented as a singleton to ensure a single instance throughout the application.
    */
    class lampLog {
    public:
        /**
         * @brief Returns the singleton instance of the lampLog class.
         *
         * @return A reference to the singleton instance of the lampLog class.
         */
        static lampLog& getInstance()
        {
            static lampLog instance;
            return instance;
        }

        lampLog(lampLog const&) = delete;
        void operator=(lampLog const&) = delete;

        /**
         * @brief Enumeration representing different warning levels for log messages.
         */
        enum warningLevel {
            LOG,     ///< Log message.
            WARNING, ///< Warning message.
            ERROR    ///< Error message.
        };

        /**
         * @brief Enumeration representing error codes for log messages.
         */
        enum errorCode {
            LMP_UNK = 0,           ///< Unknown error.
            LMP_NODIRCREATION,     ///< Failed to create a directory.
            LMP_NO7ZP,             ///< 7-Zip executable not found.
            LMP_NOFILEDROP,        ///< File drop failed.
            LMP_CLEANUPFAILED,     ///< Pre-cleanup failed.
            LMP_PREDEPLOYFAILED,   ///< Pre-deployment tasks failed.
            LMP_DEOPLYMENTFAILED,  ///< Deployment failed.
            LMP_MODLISTSAVEFAILED, ///< Failed to save the mod list.
            LMP_KEYSAVEFAILED,     ///< Failed to save a key.
            LMP_KEYLOADFAILED,     ///< Failed to load a key.
            LMP_EXTRACTIONFALED,   ///< Archive extraction failed.
            LMP_NOCONFIG           ///< Configuration file not found.
        };

        std::list<std::string> poplist = {};
        // Passthrough log.
        lampTypes::lampReturn pLog(lampTypes::lampReturn data,warningLevel warningLevel = warningLevel::LOG,bool pop = false, errorCode errorCode = errorCode::LMP_UNK){
            log(data.returnReason,warningLevel,pop,errorCode);
            return data;
        }

        /**
         * @brief Logs a message with the specified warning level and error code.
         *
         * @param data The message to log.
         * @param warningLevel The warning level (LOG, WARNING, or ERROR).
         * @param pop Indicates whether the message should be displayed as a pop-up.
         * @param errorCode The error code associated with the message.
         */
        void log(lampTypes::lampString data, warningLevel warningLevel = warningLevel::LOG, bool pop = false, errorCode errorCode = errorCode::LMP_UNK) {
            lampTypes::lampString ping;
            switch (warningLevel) {
                case LOG:
                    ping = "[LAMP:LOG:" + std::to_string(errorCode) + "] " + data;
                    break;
                case WARNING:
                    ping = "[LAMP:WARNING:" + std::to_string(errorCode) + "] " + data;
                    break;
                case ERROR:
                    ping = "[LAMP:ERROR:" + std::to_string(errorCode) + "] " + data;
                    break;
            }
            std::cout << ping << std::endl;

            std::ofstream outputFile("lamp.log", std::ios::app);
            if (outputFile) {
                std::string lineToAdd = "This is a new line.";
                outputFile << ping << std::endl;
                outputFile.close();
            }

            if (pop) {
                //  if(!lampConfig::getInstance().ShowIntroMenu) {
                poplist.push_front(ping);
                //    }
            }
        }

        /**
         * @brief Displays warning pop-up messages from the poplist.
         *
         * @return true if there are warnings to display, false otherwise.
         */
        bool showWarns() {
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
        /**
         * @brief Private constructor for the lampLog class to enforce singleton pattern.
         */
        lampLog() {}
    };

    /**
    * @brief A class for asynchronously executing code on a list of mods.
    *
    * The `lampExecutor` class allows you to execute a provided code function asynchronously on a list of mods.
    * It keeps track of the total number of items and the count of finished items.
    */
    class lampExecutor {
    public:
        /**
         * @brief Constructs a `lampExecutor` object with the provided code function.
         *
         * @param code The code function to execute on each mod.
         */
        lampExecutor(std::function<void(const lampMod::Mod *)> code) : code_(code), itemCount(0), finishedCount(0) {}

        int itemCount;          ///< The total number of items to execute.
        int finishedCount;      ///< The count of items that have finished execution.

        /**
         * @brief Executes the provided code function on a list of mods asynchronously.
         *
         * This method creates a separate thread for each mod in the list and executes the provided code function on it.
         * It also keeps track of the number of finished executions.
         *
         * @param data The vector of mod pointers on which to execute the code.
         */
        void execute(const std::vector<lampMod::Mod *>& data, bool detach) {
            lampLog::getInstance().log("ModList Executor: Starting", lampLog::warningLevel::LOG);
            itemCount = data.size();
                for (const lampMod::Mod *item: data) {
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
                    if(detach) {
                        t.detach();
                    }else{
                        t.join();
                    }
                }
            waitForCompletion();
        }

        /**
         * @brief Waits for the completion of all asynchronous executions.
         *
         * This method blocks until all asynchronous executions have finished.
         */
        void waitForCompletion() {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return finishedCount == itemCount;
            });
        }

    private:
        std::function<void(const lampMod::Mod *)> code_;  ///< The code function to execute.
        std::mutex mutex_;                                ///< Mutex for thread synchronization.
        std::condition_variable condition_;               ///< Condition variable for thread synchronization.
    };

    /**
    * @brief A utility class for sequencing and executing lambda expressions in named queues.
    *
    * The `LampSequencer` class allows you to add lambda expressions to named queues and execute them sequentially.
    * It logs the results of the execution, including success, failure, and critical failure.
    */
    class LampSequencer {
    public:
        /**
         * @brief Adds a lambda expression to the specified queue.
         *
         * @param queueName The name of the queue to which the lambda should be added.
         * @param lambda The lambda expression to be added to the queue.
         */
        static void add(const std::string& queueName, std::function<lampTypes::lampReturn()> lambda) {
            getQueue(queueName).push(lambda);
        }

        /**
         * @brief Runs all lambda expressions in the specified queue sequentially.
         *
         * This method executes lambda expressions in the specified queue one by one and logs the results.
         * Execution stops on critical failure (returnClause == -1) to prevent further execution.
         *
         * @param queueName The name of the queue to run.
         * @return A lampReturn object indicating the overall result of the execution.
         */
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
        /**
         * @brief Retrieves the queue associated with the specified name.
         *
         * This method returns the queue associated with the provided name or creates a new one if it doesn't exist.
         *
         * @param queueName The name of the queue to retrieve.
         * @return A reference to the queue associated with the provided name.
         */
        static std::queue<std::function<lampTypes::lampReturn()>>& getQueue(const std::string& queueName) {
            static std::unordered_map<std::string, std::queue<std::function<lampTypes::lampReturn()>>> queues;
            return queues[queueName];
        }

        /**
         * @brief Logs the result of lambda execution for the specified queue.
         *
         * This method logs the result of lambda execution in the specified queue, indicating success, failure, or critical failure.
         *
         * @param queueName The name of the queue.
         * @param result The result of lambda execution.
         */
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
