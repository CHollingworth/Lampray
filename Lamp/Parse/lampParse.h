//
// Created by charles on 27/09/23.
//

#ifndef LAMP_LAMPPARSE_H
#define LAMP_LAMPPARSE_H

#include <string>
#include <regex>
#include "../Control/lampConfig.h"
#include "../../third-party/json/json.hpp"
#include "../../third-party/l4z/lz4frame.h"
#include <lz4.h>

namespace Lamp::Core::Parse{
    class lampFoModParser{

    };


    // Go Star smacx250s project for this amazing pearl script made c++ https://github.com/smacx250/BG3
    class lampBG3PakParser{
    private:

        static constexpr int FILE_HDR_LEN = 24;
        static constexpr int TBL_HDR_LEN = 8;
        static constexpr int TBL_ENT_LEN = 272;

        static std::vector<char> uncompressTableEntries(const std::vector<char>& compressedTableEntries) {
            size_t maxDecompressedSize = TBL_ENT_LEN * compressedTableEntries.size(); // Max possible size
            std::vector<char> tblEntries(maxDecompressedSize);

            int decompressedSize = LZ4_decompress_safe(compressedTableEntries.data(), tblEntries.data(), compressedTableEntries.size(), maxDecompressedSize);

            if (decompressedSize < 0) {
                std::cerr << "Error during LZ4 decompression" << std::endl;
                exit(1);
            }

            // Resize the vector to the actual decompressed size
            tblEntries.resize(decompressedSize);
            return tblEntries;
        }

        static void extractMetadataFromLSX(const std::string& lsxData) {
            bool gotModInfo = false;
            std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + "/Baldur's Gate 3";
            std::string UUID;
            std::string Folder;
            std::string Version;
            std::string MD5;
            std::string Name;

            std::regex pattern("value=\"([^\"]+)\"");

            size_t pos = 0;
            while ((pos = lsxData.find('<', pos)) != std::string::npos) {
                size_t endPos = lsxData.find('>', pos);
                if (endPos != std::string::npos) {
                    std::string attribute = lsxData.substr(pos + 1, endPos - pos - 1);

                    if (attribute.find("ModuleInfo") != std::string::npos) {
                        gotModInfo = true;
                    }

                    if (gotModInfo){

                        std::smatch match;
                        std::string value;
                        if (std::regex_search(attribute, match, pattern)) {
                            if (match.size() > 1) {
                                value = match[1];
                            }
                        }

                        if(attribute.find("Folder") != std::string::npos){
                            Folder = value;
                        }
                        if(attribute.find("MD5") != std::string::npos){
                            MD5 = value;
                        }
                        if(attribute.find("Name") != std::string::npos){
                            Name = value;
                        }
                        if(attribute.find("UUID") != std::string::npos){
                            UUID = value;
                        }
                        if(attribute.find("Version") != std::string::npos){
                            Version = value;
                        }
                    }


                    pos = endPos + 1;
                }
            }
            pugi::xml_document doc;
            while (!doc.load_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                std::cerr << "Failed to load XML file." << std::endl;

            }

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
            folderAttrib.append_attribute("value") = Folder.c_str();

            pugi::xml_node md5Attrib = newShortDescNode.append_child("attribute");
            md5Attrib.append_attribute("id") = "MD5";
            md5Attrib.append_attribute("type") = "LSString";
            md5Attrib.append_attribute("value") = MD5.c_str();

            pugi::xml_node nameAttrib = newShortDescNode.append_child("attribute");
            nameAttrib.append_attribute("id") = "Name";
            nameAttrib.append_attribute("type") = "LSString";
            nameAttrib.append_attribute("value") = Name.c_str();

            pugi::xml_node uuidAttrib = newShortDescNode.append_child("attribute");
            uuidAttrib.append_attribute("id") = "UUID";
            uuidAttrib.append_attribute("type") = "FixedString";
            uuidAttrib.append_attribute("value") = UUID.c_str();


            pugi::xml_node versionAttrib = newShortDescNode.append_child("attribute");
            versionAttrib.append_attribute("id") = "Version64";
            versionAttrib.append_attribute("value") = Version.c_str();
            versionAttrib.append_attribute("type") = "int64";


            if (!doc.save_file((workingDir + "/PlayerProfiles/Public/modsettings.lsx").c_str())) {
                std::cerr << "Failed to save XML file." << std::endl;
            }



        }

    public:

        static bool extractJsonData(std::string ArchivePath) {
           std::vector<int> SUP_VER = {15, 18};
           std::string workingDir = Lamp::Core::lampConfig::getInstance().DeploymentDataPath + "/Baldur's Gate 3";

           for (const auto& entry : std::filesystem::directory_iterator((workingDir + "/ext/" + std::filesystem::path(ArchivePath).filename().stem().string()).c_str())) {
               if(std::regex_match (entry.path().filename().string(), std::regex("^.*\\.(pak)$") )) {
                   // debugging from pearl script. set to 5 for full info dump;
                   int argc = 0;

                   const std::string pakName = entry.path().string();

                   // Open the file
                   std::ifstream pakFile(pakName, std::ios::binary);
                   if (!pakFile.is_open()) {
                       std::cerr << "Can't open '" << pakName << "' for reading" << std::endl;
                       return 0;
                   }

                   // Read the file header
                   std::vector<char> fileHeader(FILE_HDR_LEN);
                   if (!pakFile.read(fileHeader.data(), FILE_HDR_LEN)) {
                       std::cerr << "Unable to read file header from '" << pakName << "'" << std::endl;
                       return 0;
                   }

                   // Check ID
                   if (std::string(fileHeader.data(), 4) != "LSPK") {
                       std::cerr << "Invalid file ID, need 'LSPK', got '" << std::string(fileHeader.data(), 4) << "'" << std::endl;
                       return 0;
                   }

                   // Check version
                   uint32_t version = *reinterpret_cast<uint32_t*>(&fileHeader[4]);
                   if (version < SUP_VER[0] || version > SUP_VER[1]) {
                       std::cerr << "Warning: file version '" << version << "' is outside of values believed to work ["
                                 << SUP_VER[0] << "," << SUP_VER[1] << "]!" << std::endl;
                   }

                   // Get table offset
                   uint64_t tblOff = *reinterpret_cast<uint64_t*>(&fileHeader[8]);

                   // Read the table header
                   pakFile.seekg(tblOff);
                   std::vector<char> tblHeader(TBL_HDR_LEN);
                   if (!pakFile.read(tblHeader.data(), TBL_HDR_LEN)) {
                       std::cerr << "Unable to read table header from '" << pakName << "'" << std::endl;
                       return 0;
                   }

                   uint32_t numFiles = *reinterpret_cast<uint32_t*>(&tblHeader[0]);
                   uint32_t tblCmpLen = *reinterpret_cast<uint32_t*>(&tblHeader[4]);
                   uint32_t tblLen = numFiles * TBL_ENT_LEN;

                   // Read the compressed table entries
                   std::vector<char> cmpTblEntries(tblCmpLen);
                   if (!pakFile.read(cmpTblEntries.data(), tblCmpLen)) {
                       std::cerr << "Unable to read table entries from '" << pakName << "'" << std::endl;
                       return 0;
                   }

                   if (argc > 2) {
                       std::ofstream ctblOut("ctbl.bin", std::ios::binary);
                       ctblOut.write(cmpTblEntries.data(), cmpTblEntries.size());
                   }

                   // Uncompress the table entries
                   std::vector<char> tblEntries = uncompressTableEntries(cmpTblEntries);

                   if (argc > 2) {
                       std::ofstream uctblOut("uctbl.bin", std::ios::binary);
                       uctblOut.write(tblEntries.data(), tblEntries.size());
                   }

                   // Iterate through the files, looking for "meta.lsx"
                   uint64_t fOfst = 0;
                   uint32_t fcLen = 0;
                   uint32_t fLen = 0;
                   for (int i = 0; i < numFiles; i++) {
                       int entOff = i * TBL_ENT_LEN;
                       std::string fName(tblEntries.data() + entOff, 256);

                       if (fName.find("meta.lsx") != std::string::npos) {
                           entOff += 256;
                           fOfst = *reinterpret_cast<uint64_t*>(tblEntries.data() + entOff) & 0x00ffffffffffffff;
                           fcLen = *reinterpret_cast<uint32_t*>(tblEntries.data() + entOff + 8);
                           fLen = *reinterpret_cast<uint32_t*>(tblEntries.data() + entOff + 12);

                           break;
                       }
                   }

                   // Couldn't find it!
                   if (fOfst == 0) {
                       std::cerr << "Was not able to find 'meta.lsx' in '" << pakName << "'" << std::endl;
                       return 0;
                   }

                   // Now read the file
                   pakFile.seekg(fOfst);
                   std::vector<char> rawFile(fcLen);
                   if (!pakFile.read(rawFile.data(), fcLen)) {
                       std::cerr << "Unable to read 'meta.lsx' file from '" << pakName << "'" << std::endl;
                       return 0;
                   }

                   std::vector<char> ucFile;
                   if (fLen == 0) {
                       ucFile = rawFile;
                   } else {
                       ucFile = uncompressTableEntries(rawFile);
                   }

                   if (argc > 2) {
                       std::cout << "\nmeta.lsx:\n" << std::string(ucFile.data(), ucFile.size()) << "\n";
                   }

                   std::vector<std::string> seenAttr;
                   extractMetadataFromLSX(std::string(ucFile.data(), ucFile.size()));



               }}
            return 1;
        }
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

            return false;
        }

    };
}
#endif //LAMP_LAMPPARSE_H
