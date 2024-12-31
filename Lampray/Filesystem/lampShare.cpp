//
// Created by charles on 07/10/23.
//
#include <lz4hc.h>
#include <climits>

#include "lampFS.h"
#include "../../third-party/nfd/include/nfd.h"

#include "../Control/lampGames.h"
#include "bit7zlibrary.hpp"
#include "bitfilecompressor.hpp"
#include "bitarchivereader.hpp"
#include "../Control/lampControl.h"

bool Lamp::Core::FS::lampShare::compressFile(const std::string& inputFile, const std::string& outputFile) {
    // Open the input file for reading
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return false;
    }

    // Open the output file for writing
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return false;
    }

    // Read the input file into a buffer
    inFile.seekg(0, std::ios::end);
    std::streampos fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<char> inputBuffer(fileSize);
    inFile.read(inputBuffer.data(), fileSize);

    // Compress the input buffer using LZ4HC
    int maxCompressedSize = LZ4_compressBound(fileSize);
    std::vector<char> compressedBuffer(maxCompressedSize);

    int compressedSize = LZ4_compress_HC(inputBuffer.data(), compressedBuffer.data(), fileSize, maxCompressedSize, LZ4HC_CLEVEL_MAX);

    if (compressedSize <= 0) {
        std::cerr << "Compression failed." << std::endl;
        return false;
    }

    // Write the compressed data to the output file
    outFile.write(compressedBuffer.data(), compressedSize);

    return true;
}
bool Lamp::Core::FS::lampShare::decompressFile(const std::string& inputFile, const std::string& outputFile, std::uintmax_t volumeSize) {    // Open the input file for reading
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return false;
    }

    // Open the output file for writing
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return false;
    }

    // Read the compressed input file into a buffer
    inFile.seekg(0, std::ios::end);
    std::streampos compressedSize = inFile.tellg();

    if (compressedSize > 0) {
        compressedSize -= 1; // Drop the last byte from the size
    }

    inFile.seekg(0, std::ios::beg);


    std::vector<char> compressedBuffer(compressedSize);
    inFile.read(compressedBuffer.data(), compressedSize);

    // Determine the decompressed size based on your knowledge of the original size
    // If you don't know the original size, you may need to store it somewhere or use a different compression library.
    // For this example, we assume you know the original size.
    std::streampos decompressedSize = volumeSize;

    // Allocate a buffer for the decompressed data
    std::vector<char> decompressedBuffer(decompressedSize);

    // Perform the decompression
    int result = LZ4_decompress_safe(compressedBuffer.data(), decompressedBuffer.data(), compressedSize, decompressedSize);

    if (result <= 0) {
        std::cerr << "Decompression failed." << std::endl;
        return false;
    }

    // Write the decompressed data to the output file
    outFile.write(decompressedBuffer.data(), decompressedSize);
    return true;
}

void Lamp::Core::FS::lampShare::InsertXMLintoFile(const std::string& filename, const pugi::xml_node& metadata) {
    // Open the file for reading and writing
    std::fstream file(filename, std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Create a string containing the XML data
    std::stringstream xmlStream;
    metadata.print(xmlStream);

    // Read the existing content
    std::string existingContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Trim trailing newline character from existing content if present
    if (!existingContent.empty() && existingContent.back() == '\n') {
        existingContent.pop_back();
    }

    // Close the file
    file.close();

    // Reopen the file for writing (truncate mode)
    file.open(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Insert the XML data at the start of the file
    file << xmlStream.str() << existingContent;
}
void Lamp::Core::FS::lampShare::ExtractXMLfromFile(const std::string& filename, std::string& content) {
    // Open the file for reading
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Open a temporary file for writing
    std::string tempFilename = filename + ".temp";
    std::ofstream tempFile(tempFilename);
    if (!tempFile.is_open()) {
        std::cerr << "Error opening temporary file." << std::endl;
        inputFile.close();
        return;
    }

    // Read and process the file line by line
    std::string line;
    std::string xmlContent;
    bool inXmlSection = false;

    while (std::getline(inputFile, line)) {
        if (line == "<metadata>") {
            inXmlSection = true;
            xmlContent += line + "\n"; // Include the "<root>" tag
        } else if (inXmlSection) {
            xmlContent += line + "\n";

            if (line == "</metadata>") {
                inXmlSection = false;
            }
        } else {
            // Write non-XML lines to the temporary file
            tempFile << line << "\n";
        }
    }

    // Close both input and temporary files
    inputFile.close();
    tempFile.close();

    // Rename the temporary file to replace the original file
    if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
        std::cerr << "Error renaming temporary file." << std::endl;
        return;
    }

    // Load the extracted XML content into pugixml
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xmlContent.c_str());
    if (result) {
        content = xmlContent; // The entire extracted content is the XML data
    } else {
        std::cerr << "Error parsing extracted XML content." << std::endl;
    }
}

void Lamp::Core::FS::lampShare::importProfile() {
    std::thread([] {
    Lamp::Core::lampControl::getInstance().inDeployment = true;
    Lamp::Core::lampControl::getInstance().deploymentStageTitle = "Importing Profile";
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,11};
    bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibraryLocation};
    std::filesystem::create_directories("import/");
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( "lampProfile", NULL, &outPath );
    if ( result == NFD_OKAY )
    {
        puts(outPath);
    }
    else if ( result == NFD_CANCEL )
    {
        Lamp::Core::lampControl::getInstance().inDeployment = false;
        return;
    }
    else
    {
        Lamp::Core::lampControl::getInstance().inDeployment = false;
        return;
    }
    Lamp::Core::lampControl::getInstance().deplopmentTracker = {1,11};
    std::filesystem::path sourcePath = outPath;  // Replace with your source file path
    std::filesystem::path destinationPath = "import/";      // Replace with your destination folder path

    try {
        // Check if the source file exists
        if (std::filesystem::exists(sourcePath)) {
            // Move the file to the destination folder
            std::filesystem::rename(sourcePath, destinationPath / sourcePath.filename());
        } else {
            Lamp::Core::lampControl::getInstance().inDeployment = false;
            return;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        Lamp::Core::lampControl::getInstance().inDeployment = false;
        return;
    }

    Lamp::Core::lampControl::getInstance().deplopmentTracker = {2,11};

    try {
        std::string content;
        Lamp::Core::FS::lampShare::ExtractXMLfromFile((destinationPath / sourcePath.filename()), content);
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {3,11};
        pugi::xml_document dcl;
        dcl.load_string(content.c_str());

        // Find the "Profile" node
        pugi::xml_node profileNode = dcl.child("metadata").child("Profile");

        // Check if the "Profile" node exists
        if (!profileNode) {
            Lamp::Core::lampControl::getInstance().inDeployment = false;
            return;
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {4,11};
        // Find the "Volume" attribute within the "Profile" node
        pugi::xml_attribute volumeAttr = profileNode.attribute("Volume");

        // Check if the "Volume" attribute exists
        if (!volumeAttr) {
            Lamp::Core::lampControl::getInstance().inDeployment = false;
            return;
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {5,11};
        std::uintmax_t volumeValue;
        try {
            volumeValue = std::stoull(volumeAttr.value());
        } catch (const std::invalid_argument &e) {
            std::cerr << "Failed to convert 'Volume' attribute value to std::uintmax_t: " << e.what() << std::endl;
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {6,11};
        decompressFile((destinationPath / sourcePath.filename()), "import/lampPackage", volumeValue);
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {7,11};
        bit7z::BitArchiveReader reader{lib, "import/lampPackage", bit7z::BitFormat::SevenZip};
        reader.test();
        reader.extract(Lamp::Core::lampConfig::getInstance().archiveDataPath +
                       Lamp::Games::getInstance().currentGame->Ident().ReadableName + "/");
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {8,11};
        std::vector<Base::lampMod::Mod *> newList;
        for (pugi::xml_node modNode = profileNode.child("Mod"); modNode; modNode = modNode.next_sibling(
                "Mod")) {
            std::string archivePath = modNode.attribute("ArchivePath").as_string();
            int modType = modNode.attribute("modType").as_int();
            bool enabled = modNode.attribute("enabled").as_bool();

            Base::lampMod::Mod *temp = new Base::lampMod::Mod{archivePath, modType, enabled};

            newList.emplace_back(temp);
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {9,11};


        // Add new profile to profile list.
        Lamp::Core::Base::lampMod::Profile::addValue(Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"],
                                                     profileNode.attribute("Name").value());
        // Overwrite the current modlist with the new one.
        Lamp::Games::getInstance().currentGame->getModList() = newList;
        // save it under the profile name
        FS::lampIO::saveModList(Lamp::Games::getInstance().currentGame->Ident().ShortHand,
                                Lamp::Games::getInstance().currentGame->getModList(),
                                profileNode.attribute("Name").value());
        // reload that new profile name
        Lamp::Games::getInstance().currentGame->getModList() = FS::lampIO::loadModList(
                Lamp::Games::getInstance().currentGame->Ident().ShortHand, profileNode.attribute("Name").value());
        // set the current profile to the new one.
        Lamp::Games::getInstance().currentProfile = profileNode.attribute("Name").value();
        // Save our profile list.
        FS::lampIO::saveKeyData("ProfileList", Lamp::Games::getInstance().currentGame->KeyInfo()["ProfileList"],
                                Lamp::Games::getInstance().currentGame->Ident().ShortHand);

        Lamp::Core::lampControl::getInstance().deplopmentTracker = {10,11};
        try {
            if (std::filesystem::exists("import/lampPackage")) {
                std::filesystem::remove("import/lampPackage");
                std::filesystem::remove(outPath);
            }
        } catch (const std::filesystem::filesystem_error &e) {
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {11,11};
        Lamp::Core::lampControl::getInstance().inDeployment = false;
    }catch (std::exception ex){

    }
        Lamp::Core::lampControl::getInstance().inDeployment = false;
    }).detach();
}
void Lamp::Core::FS::lampShare::exportProfile(std::string profileNameS) {
    std::thread([&profileNameS] {
    try {
        std::string profileName = profileNameS;
        Lamp::Core::lampControl::getInstance().inDeployment = true;
        Lamp::Core::lampControl::getInstance().deploymentStageTitle = "Creating "+profileName +".lampProfile";
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {0,3};
        bit7z::Bit7zLibrary lib{Lamp::Core::lampConfig::getInstance().bit7zLibraryLocation};
        std::vector<Lamp::Core::Base::lampMod::Mod *> TempModList = Lamp::Core::FS::lampIO::loadModList(
                Lamp::Games::getInstance().currentGame->Ident().ShortHand, profileName);
        std::map<std::string, std::string> files_map = {};
        for (const Base::lampMod::Mod *mod: TempModList) {
            if (mod->enabled) {
                files_map[mod->ArchivePath] = std::filesystem::path(mod->ArchivePath).filename().string();
            }
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {1,3};
        bit7z::BitFileCompressor compressorSevenZip{lib, bit7z::BitFormat::SevenZip};
        compressorSevenZip.compress(files_map, "lampPackage");
        compressorSevenZip.setCompressionLevel(bit7z::BitCompressionLevel::Ultra);
        compressFile("lampPackage",  profileName + ".lampProfile");
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {2,3};
        // Create the profile metadata
        pugi::xml_document metadataDoc;
        pugi::xml_node root = metadataDoc.append_child("metadata");

        pugi::xml_node profileNewNode = root.append_child("Profile");
        profileNewNode.append_attribute("Name").set_value(profileName.c_str());
        for (const Base::lampMod::Mod *mod: TempModList) {
            if (mod->enabled) {
                mod->serialize(profileNewNode);
            }
        }
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {3,3};
        std::uintmax_t fileSize = std::filesystem::file_size("lampPackage");
        profileNewNode.append_attribute("Volume").set_value(fileSize);
        Lamp::Core::lampControl::getInstance().deplopmentTracker = {3,3};
        // Delete lampPackage
        try {
            if (std::filesystem::exists("lampPackage")) {
                std::filesystem::remove("lampPackage");
            }
        } catch (const std::filesystem::filesystem_error &e) {
        }
        InsertXMLintoFile( profileName + ".lampProfile", root);

        Lamp::Core::lampControl::getInstance().inDeployment = false;
        Lamp::Core::Base::lampLog::getInstance().log("Profile Exported: " + absolute(fs::path( profileName + ".lampProfile")).string(),Base::lampLog::LOG,true);
    }catch(std::exception e){

    }
    }).detach();
}
