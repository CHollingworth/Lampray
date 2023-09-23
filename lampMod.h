//
// Created by charles on 11/09/23.
//

#ifndef LAMP_LAMPMOD_H
#define LAMP_LAMPMOD_H

#include <string>
#include <list>
#include <pugixml.hpp>

namespace Lamp {
    namespace Core {
        /**
         * @brief The lampMod class contains structures for mods and profiles.
         */
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
               std::string ArchivePath;
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
               std::string timeOfUpdate = "Unknown";

                /**
                * @brief Serialize the Mod struct to an XML node.
                *
                * @param doc The parent XML node to which this Mod will be serialized.
                * @return The serialized XML node.
                */
                pugi::xml_node serialize(pugi::xml_node doc) const {
                    pugi::xml_node modNode = doc.append_child("Mod");

                    modNode.append_attribute("ArchivePath").set_value(ArchivePath.c_str());
                    modNode.append_attribute("modType").set_value(modType);
                    modNode.append_attribute("enabled").set_value(enabled);
                    modNode.append_attribute("timeOfUpdate").set_value(timeOfUpdate.c_str());

                    return modNode;
                }

                /**
                * @brief Construct a new Mod object.
                *
                * @param archivePath The path to the mod's archive.
                * @param modType The type of the mod.
                * @param enabled Indicates whether the mod is enabled.
                */
               Mod(std::string archivePath, int modType, bool enabled)
                       : ArchivePath(archivePath), modType(modType), enabled(enabled) {
               }
            };
            /**
            * @brief The Profile struct represents a profile containing a list of mods.
            */
            // Unimplimented.
            struct Profile{
                /**
                * @brief The list of Mod objects in the profile.
                */
                std::list<Mod> ModList;

                /**
                 * @brief Serialize the Profile struct to an XML node.
                *
                * @param appendedNode The parent XML node to which this Profile will be serialized.
                * @return The serialized XML node.
                */
                pugi::xml_node serialize(pugi::xml_node appendedNode){
                    pugi::xml_node profileNode = appendedNode.append_child("Profile");
                    // Loop through the list of Mod objects and serialize each one
                    for (const Mod& mod : ModList) {
                        mod.serialize(profileNode);
                    }
                    return profileNode;
                }
                /**
                * @brief Construct a new Profile object.
                *
                * @param modList The list of Mod objects in the profile.
                */
                Profile(std::list<Mod> modList) : ModList(modList) {
                }
            };
        };
    }
}

#endif //LAMP_LAMPMOD_H
