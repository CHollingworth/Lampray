//
// Created by charles on 11/09/23.
//

#ifndef LAMP_LAMPFILESYSTEM_H
#define LAMP_LAMPFILESYSTEM_H

#include <string>
#include <list>

namespace Lamp {
    namespace Core {
        class lampFilesystem {
        public:
            bool createFileStructure(std::list<std::string> structure);
        };
    }
} // Lamp

#endif //LAMP_LAMPFILESYSTEM_H
