//
// Created by charles on 18/12/23.
//

#ifndef LAMPRAY_LAMPLANG_H
#define LAMPRAY_LAMPLANG_H

#include <filesystem>
#include <string>
#include <map>
#include <pugixml.hpp>
#include <unordered_set>
#include "../Control/lampConfig.h"
#include "../Base/lampBase.h"

namespace Lamp {
    namespace Core {

        class lampLang {
        public:

            static lampLang& getInstance()
            {
                static lampLang instance;
                return instance;
            }

            lampLang(lampLang const&) = delete;
            void operator=(lampLang const&)  = delete;


            struct LanguageContainer{
            private:
                std::string LanguageName;
                std::map<std::string,std::string> LanguageMap;
                std::unordered_set<std::string> KnownKeys;
            public:
                Base::lampTypes::lampString S(std::string S){

                    if (KnownKeys.find(S) != KnownKeys.end()) {
                        return LanguageMap[S];
                    } else {
                        auto it = LanguageMap.find(S);
                        if (it != LanguageMap.end()) {
                            KnownKeys.insert(S);
                            return LanguageMap[S];
                        } else {
                            return "[?]"+S;
                        }
                    }

                }

                Base::lampTypes::lampReturn build(const std::filesystem::path& filePath){

                    if (!std::filesystem::exists(filePath)) {
                        return {false, "File not found: " + filePath.string()};
                    }

                    // Load the XML document
                    std::string xmlPath = filePath;

                    pugi::xml_document doc;

                    if (doc.load_file(xmlPath.c_str())) {

                    }

                    pugi::xml_node langNode = doc.child("LamprayLang");
                    if (langNode) {
                        LanguageName = langNode.attribute("name").value();
                    } else {
                        return {false, "Failed to load language. No Name."};
                    }

                    for (pugi::xml_node node = langNode.child("LangNode"); node; node = node.next_sibling("LangNode")) {
                        std::string key = node.attribute("key").value();
                        std::string value = node.child_value();
                        LanguageMap[key] = value;
                    }

                    return {true, "Language Loaded."};
                }

            };


            static Base::lampTypes::lampString localizedString(Base::lampTypes::lampString key){
                return Lamp::Core::lampLang::getInstance().CurrentLanguage.S(key);
            }

            static Base::lampTypes::lampString LS(Base::lampTypes::lampString k){
                return lampLang::localizedString(k);
            }

            LanguageContainer CurrentLanguage;

            std::filesystem::path createEnglishUK(){
                pugi::xml_document doc;
                auto root = doc.append_child("LamprayLang");
                root.append_attribute("name").set_value("English (UK)");

                auto addLangNode = [&](const char* key, const char* text) {
                    auto node = root.append_child("LangNode");
                    node.append_attribute("key").set_value(key);
                    node.text().set(text);
                };

                addLangNode("LAMPRAY_LONGNAME", "Linux Application Modding Platform");
                addLangNode("LAMPRAY_LICENSE", R"(
This is free and unencumbered software released into the public domain.


Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.


In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.


For more information, please refer to <https://unlicense.org>)");
                addLangNode("LAMPRAY_AKNOWL", "I have read and understood the text above.");
                addLangNode("LAMPRAY_DRAGANDDROP", "Drag archives onto this window to start adding mods to your game!");
                addLangNode("LAMPRAY_DEPLOY", "Deploy");
                addLangNode("LAMPRAY_RESET", "Reset");
                addLangNode("LAMPRAY_CHECK", "Checks");
                addLangNode("LAMPRAY_STARTTEXT", "Start Deployment?");
                addLangNode("LAMPRAY_START", "Start");
                addLangNode("LAMPRAY_GOBACK", "Back");
                addLangNode("LAMPRAY_GAMEPICK", "Game Selection");
                addLangNode("LAMPRAY_GAMECONFIG", "Game Configuration");
                addLangNode("LAMPRAY_PROFSELECT", "Profile Selection");
                addLangNode("LAMPRAY_PROFLOAD", "Load Profile");
                addLangNode("LAMPRAY_PROFDEL", "Delete Profile");
                addLangNode("LAMPRAY_PROFCRE", "Create New Profile");
                addLangNode("LAMPRAY_PROFCRES", "Create");
                addLangNode("LAMPRAY_PROFEXP", "Export Profile");
                addLangNode("LAMPRAY_PROFIMP", "Import Profile");
                addLangNode("LAMPRAY_PROFFRM", "Copy From");
                addLangNode("LAMPRAY_PROFNON", "None");
                addLangNode("LAMPRAY_PROFNM", "New Profile Name");
                addLangNode("LAMPRAY_CUSTOM", "Customise Lampray");
                addLangNode("LAMPRAY_ABT", "About");
                addLangNode("LAMPRAY_ABT1", "Created by");
                addLangNode("LAMPRAY_QUIT", "Quit");
                addLangNode("LAMPRAY_MODLIST_ENABLE", "Enabled");
                addLangNode("LAMPRAY_MODLIST_DISABLE", "Disabled");
                addLangNode("LAMPRAY_MODLIST_EXPND", "Expand");
                addLangNode("LAMPRAY_MODLIST_COLL", "Collapse");
                addLangNode("LAMPRAY_MODLIST_REMOVE_MOD", "Remove Mod");
                addLangNode("LAMPRAY_MODLIST_REMOVE_SEP", "Remove");
                addLangNode("LAMPRAY_MODLIST_MV_TOP", "Move to Top");
                addLangNode("LAMPRAY_MODLIST_MV_BTTM", "Move to Bottom");
                addLangNode("LAMPRAY_MODLIST_RENAME", "Rename Separator");
                addLangNode("LAMPRAY_MODLIST_CREATE_SEP", "Create Separator");
                addLangNode("LAMPRAY_MODLIST_SEARCH", "Type here to search your mods...");
                addLangNode("LAMPRAY_MODLIST_UP", "Up");
                addLangNode("LAMPRAY_MODLIST_DOWN", "Down");
                addLangNode("LAMPRAY_MODLIST_WARN", "Only disabled mods can be removed.");
                addLangNode("LAMPRAY_MODLIST_CONFRIMDEL", "Confirm Deletion");
                addLangNode("LAMPRAY_MODLIST_WARN_FINAL", R"(?

This action cannot be undone.)");
                addLangNode("LAMPRAY_MODLIST_WARN_SURE", "Are you sure you want to delete: ");
                addLangNode("LAMPRAY_MODLIST_DELETE", "Delete");
                addLangNode("LAMPRAY_MODLIST_TITLE_ENABLED", "Enabled");
                addLangNode("LAMPRAY_MODLIST_TITLE_MODNAME", "Mod Name");
                addLangNode("LAMPRAY_MODLIST_TITLE_MODTYPE", "Mod Type");
                addLangNode("LAMPRAY_MODLIST_TITLE_LASTUPDATE", "Last Updated");
                addLangNode("LAMPRAY_MODLIST_TITLE_ORDER", "Load Order");
                addLangNode("LAMPRAY_MODLIST_TITLE_REMOVEMOD", "Remove Mod");
                addLangNode("LAMPRAY_UPDATE_CHECK", "Check for Updates");
                addLangNode("LAMPRAY_UPDATE_RULES", "Update Rules");
                addLangNode("LAMPRAY_UPDATE_RULES1", "Check for updates at startup");
                addLangNode("LAMPRAY_CUSTOM_LANG", "Language");
                addLangNode("LAMPRAY_CUSTOM_COLOUR", "Colours");
                addLangNode("LAMPRAY_CUSTOM_FONT", "Font Size");
                addLangNode("LAMPRAY_CUSTOM_SV", "Save");
                addLangNode("LAMPRAY_SELECT_PATH", "Select Path");
                addLangNode("LAMPRAY_ERROR_7Z", "Failed to find 7z.so! Many actions, such as deployment, will not function correctly. See the wiki for more information.");
                addLangNode("LAMPRAY_WARN_GAME_PATH", " directories are not set. Deployment will not work until you have set them in the Game Configuration menu.");

                auto baseDirectory = Lamp::Core::lampConfig::getInstance().baseDataPath + "Language/";
                std::filesystem::create_directory(baseDirectory);
                
                std::filesystem::path path = baseDirectory + "English (UK).xml";
                doc.save_file(path.c_str());
                return path; 
            }

        private:
            lampLang(){};
        };

    } // Core
} // Lamp

#endif //LAMPRAY_LAMPLANG_H
