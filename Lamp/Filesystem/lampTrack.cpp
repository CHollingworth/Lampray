//
// Created by charles on 10/10/23.
//
#include <filesystem>
#include "lampFS.h"
std::string Lamp::Core::FS::lampTrack::getHash(std::filesystem::path filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    unsigned int hash = 0xFFFFFFFF; // Initialize CRC32 with all 1s

    while (file.good()) {
        file.read(buffer, bufferSize);
        const size_t bytesRead = static_cast<size_t>(file.gcount());
        hash = crc32(buffer, bytesRead);
    }

    // Convert the numeric hash to a hexadecimal string
    std::stringstream stream;
    stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << hash;
    return stream.str();
}


Lamp::Core::lampReturn Lamp::Core::FS::lampTrack::handleArchive() {


}


void Lamp::Core::FS::lampTrack::reset(std::string gameFullReadableName) {

}



