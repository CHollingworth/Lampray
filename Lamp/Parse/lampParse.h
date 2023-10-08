//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPPARSE_H
#define LAMP_LAMPPARSE_H

#include <string>
#include <regex>
#include "../Control/lampConfig.h"
#include "../../third-party/json/json.hpp"

namespace Lamp::Core::Parse{
    class lampFoModParser{

    };

    class lampBG3PakParser{
    public:
       static bool collectJsonData(std::string ArchivePath){
           std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + "/Baldur's Gate 3";

           for (const auto& entry : std::filesystem::directory_iterator((workingDir + "/ext/" + std::filesystem::path(ArchivePath).filename().stem().string()).c_str())) {
               if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(json)$") )) {

                   std::ifstream jsonFile(entry.path());
                   pugi::xml_document doc;

                   if (!jsonFile.is_open()) {
                       std::cerr << "Failed to open JSON file." << std::endl;
                       return false;
                   }


                   while (!doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                       std::cerr << "Failed to load XML file." << std::endl;
                       // return false;
                   }

                   nlohmann::json jsonData;
                   jsonFile >> jsonData;
                   jsonFile.close();

                   if (jsonData.contains("mods") || jsonData.contains("Mods")) {

                       std::string packString = "mods";
                       if(jsonData.contains("Mods")){
                           packString = "Mods";
                       }

                       for (const auto& mod : jsonData[packString]) {
                           std::string modName;
                           if(mod.contains("modName")){
                               modName = mod["modName"];
                           }else if(mod.contains("ModName")){
                               modName = mod["ModName"];
                           }else{
                               modName = mod["Name"];
                           }

                           std::string folderName;
                           if(mod.contains("folderName")){
                               folderName = mod["folderName"];
                           }else if(mod.contains("FolderName")){
                               folderName = mod["FolderName"];
                           }else{
                               folderName = mod["Folder"];
                           }

                           std::string UUID = mod["UUID"];

                           pugi::xml_node moduleNode = doc.select_node("//node[@id='ModOrder']").node();
                           pugi::xml_node childrenNode = moduleNode.child("children");
                           pugi::xml_node newNode = childrenNode.append_child("node");
                           newNode.append_attribute("id") = "Module";
                           pugi::xml_node attributes = newNode.append_child("attribute");
                           attributes.append_attribute("id") = "UUID";
                           attributes.append_attribute("value") = UUID.c_str();
                           attributes.append_attribute("type") = "FixedString";

                           pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();
                           childrenNode = modsNode.child("children");

                           pugi::xml_node newShortDescNode = childrenNode.append_child("node");
                           newShortDescNode.append_attribute("id") = "ModuleShortDesc";

                           pugi::xml_node folderAttrib = newShortDescNode.append_child("attribute");
                           folderAttrib.append_attribute("id") = "Folder";
                           folderAttrib.append_attribute("type") = "LSString";
                           folderAttrib.append_attribute("value") = folderName.c_str();

                           pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
                           md5Attrib.append_attribute("id") = "MD5";
                           md5Attrib.append_attribute("type") = "LSString";
                           md5Attrib.append_attribute("value") = "";

                           pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
                           nameAttrib.append_attribute("id") = "Name";
                           nameAttrib.append_attribute("type") = "LSString";
                           nameAttrib.append_attribute("value") = modName.c_str();

                           pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
                           uuidAttrib.append_attribute("id") = "UUID";
                           uuidAttrib.append_attribute("type") = "FixedString";
                           uuidAttrib.append_attribute("value") = UUID.c_str();


                           pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
                           versionAttrib.append_attribute("id") = "Version64";
                           versionAttrib.append_attribute("value") = "36028797018963968";
                           versionAttrib.append_attribute("type") = "int64";


                           if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                               std::cerr << "Failed to save XML file." << std::endl;
                           }

                           return true;
                       }
                   } else {
                       std::cerr << "JSON data does not contain 'mods' key." << std::endl;
                       return false;
                   }

               }
           }


       }

       static bool extractJsonData(std::string ArchivePath) {

           pugi::xml_document doc;
           std::string uuidout;

           std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + "Baldur's Gate 3";

           while (!doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
               std::cerr << "Failed to load XML file." << std::endl;
               //return false;
           }

           for (const auto& entry : std::filesystem::directory_iterator((workingDir + "/ext/" + std::filesystem::path(ArchivePath).filename().stem().string()).c_str())) {
               if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(pak)$") )) {
                   std::cout << entry.path() << std::endl;
                   std::ifstream inputFile(entry.path());

                   // Check if the file is open
                   if (!inputFile.is_open()) {
                       std::cerr << "Failed to open the file!" << std::endl;
                       return 1;
                   }

                   std::regex uuidPattern("\\b[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\\b");

                   std::string line;

                   while (std::getline(inputFile, line)) {
                       std::smatch match;
                       if (std::regex_search(line, match, uuidPattern)) {
                           std::cout << "Found UUID in line: " << line << std::endl;
                           uuidout = match[0];

                           break;
                       }
                   }


                   std::filesystem::path assumedFolderName(entry.path().c_str());

                   if (uuidout != "") {
                       pugi::xml_node moduleNode = doc.select_node("//node[@id='ModOrder']").node();
                       pugi::xml_node childrenNode = moduleNode.child("children");
                       pugi::xml_node newNode = childrenNode.append_child("node");
                       newNode.append_attribute("id") = "Module";
                       pugi::xml_node attributes = newNode.append_child("attribute");
                       attributes.append_attribute("id") = "UUID";
                       attributes.append_attribute("value") = uuidout.c_str();
                       attributes.append_attribute("type") = "FixedString";

                       pugi::xml_node modsNode = doc.select_node("//node[@id='Mods']").node();
                       childrenNode = modsNode.child("children");

                       pugi::xml_node newShortDescNode = childrenNode.append_child("node");
                       newShortDescNode.append_attribute("id") = "ModuleShortDesc";

                       pugi::xml_node folderAttrib = newShortDescNode.append_child("attribute");
                       folderAttrib.append_attribute("id") = "Folder";
                       folderAttrib.append_attribute("type") = "LSString";
                       folderAttrib.append_attribute("value") = assumedFolderName.filename().c_str();

                       pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
                       md5Attrib.append_attribute("id") = "MD5";
                       md5Attrib.append_attribute("type") = "LSString";
                       md5Attrib.append_attribute("value") = "";

                       pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
                       nameAttrib.append_attribute("id") = "Name";
                       nameAttrib.append_attribute("type") = "LSString";
                       nameAttrib.append_attribute("value") = assumedFolderName.filename().c_str();

                       pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
                       uuidAttrib.append_attribute("id") = "UUID";
                       uuidAttrib.append_attribute("type") = "FixedString";
                       uuidAttrib.append_attribute("value") = uuidout.c_str();


                       pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
                       versionAttrib.append_attribute("id") = "Version64";
                       versionAttrib.append_attribute("value") = "36028797018963968";
                       versionAttrib.append_attribute("type") = "int64";


                       if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                           std::cerr << "Failed to save XML file." << std::endl;
                       }

                       inputFile.close();
                   }
               }
           }
       }
    };
}
#endif //LAMP_LAMPPARSE_H
