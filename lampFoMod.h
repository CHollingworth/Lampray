//
// Created by charles on 13/09/23.
//

#ifndef LAMP_LAMPFOMOD_H
#define LAMP_LAMPFOMOD_H

#include <list>
#include <string>
#include <vector>
#include <pugixml.hpp>


namespace Lamp::Core{
    class lampFoMod{

    public:

        static lampFoMod& getInstance()
        {
            static lampFoMod instance;
            return instance;
        }

        lampFoMod(lampFoMod const&) = delete;
        void operator=(lampFoMod const&)  = delete;

        struct Folder {
            std::string source;
            std::string destination;
            int priority;
        };

        struct TypeDescriptor {
            std::string type;
        };

        struct ConditionFlag {
            std::string name;
            std::string value;
        };

        struct Plugin {
            std::string name;
            std::string description;
            std::vector<Folder> files;
            TypeDescriptor type_descriptor;
            std::vector<ConditionFlag> condition_flags;
        };

        struct OptionalFileGroup {
            std::string name;
            std::string type;
            std::vector<Plugin> plugins;
        };

        struct InstallStep {
            std::string name;
            std::vector<OptionalFileGroup> optional_file_groups;
        };

        struct RequiredInstallFile {
            std::string source;
            std::string destination;
        };

        struct FlagDependency {
            std::string flag;
            std::string value;
        };

        struct FilePattern {
            std::string source;
            std::string destination;
            int priority;
            std::vector<FlagDependency> dependencies;
        };

        struct ConditionalFileInstalls {
            std::vector<FilePattern> patterns;
        };

        struct Config {
            std::string module_name;
            std::string module_image_path;
            std::vector<RequiredInstallFile> required_install_files;
            std::vector<InstallStep> install_steps;
            ConditionalFileInstalls conditional_file_installs;
            std::map<std::string, std::string> flags;

            InstallStep currentStep;
            OptionalFileGroup currentGroup;
            bool populated = false;
        };

        Config ParseModuleConfig(const std::string& xmlContent) {
            Config config;

            pugi::xml_document doc;


            if (doc.load_file(xmlContent.c_str())) {
                pugi::xml_node root = doc.child("config");

                if (root) {
                    config.module_name = root.child_value("moduleName");
                    config.module_image_path = root.child("moduleImage").attribute("path").value();

                    // Parse requiredInstallFiles
                    pugi::xml_node requiredInstallFilesNode = root.child("requiredInstallFiles");
                    if (requiredInstallFilesNode) {
                        for (pugi::xml_node folderNode = requiredInstallFilesNode.child("folder");
                             folderNode;
                             folderNode = folderNode.next_sibling("folder")) {
                            RequiredInstallFile requiredInstallFile;
                            requiredInstallFile.source = folderNode.attribute("source").value();
                            requiredInstallFile.destination = folderNode.attribute("destination").value();
                            config.required_install_files.push_back(requiredInstallFile);
                        }
                    }

                    // Parse installSteps
                    pugi::xml_node installStepsNode = root.child("installSteps");
                    if (installStepsNode) {
                        for (pugi::xml_node installStepNode = installStepsNode.child("installStep");
                             installStepNode;
                             installStepNode = installStepNode.next_sibling("installStep")) {
                            InstallStep installStep;
                            installStep.name = installStepNode.attribute("name").value();

                            // Parse optionalFileGroups within installStep
                            for (pugi::xml_node optionalFileGroupNode = installStepNode.child("optionalFileGroups").child("group");
                                 optionalFileGroupNode;
                                 optionalFileGroupNode = optionalFileGroupNode.next_sibling("group")) {
                                OptionalFileGroup optionalFileGroup;
                                optionalFileGroup.name = optionalFileGroupNode.attribute("name").value();
                                optionalFileGroup.type = optionalFileGroupNode.attribute("type").value();

                                // Parse plugins within optionalFileGroup
                                for (pugi::xml_node pluginNode = optionalFileGroupNode.child("plugins").child("plugin");
                                     pluginNode;
                                     pluginNode = pluginNode.next_sibling("plugin")) {
                                    Plugin plugin;
                                    plugin.name = pluginNode.attribute("name").value();
                                    plugin.description = pluginNode.child("description").text().get();

                                    // Parse conditionFlags within plugin
                                    for (pugi::xml_node conditionFlagNode = pluginNode.child("conditionFlags").child("flag");
                                         conditionFlagNode;
                                         conditionFlagNode = conditionFlagNode.next_sibling("flag")) {
                                        ConditionFlag flag;
                                        flag.name = conditionFlagNode.attribute("name").value();
                                        flag.value = conditionFlagNode.text().get();
                                        plugin.condition_flags.push_back(flag);
                                    }

                                    // Parse files within plugin
                                    for (pugi::xml_node fileNode = pluginNode.child("files").child("folder");
                                         fileNode;
                                         fileNode = fileNode.next_sibling("folder")) {
                                        Folder file;
                                        file.source = fileNode.attribute("source").value();
                                        file.destination = fileNode.attribute("destination").value();
                                        file.priority = std::stoi(fileNode.attribute("priority").value());
                                        plugin.files.push_back(file);
                                    }

                                    // Parse typeDescriptor within plugin
                                    pugi::xml_node typeDescriptorNode = pluginNode.child("typeDescriptor").child("type");
                                    if (typeDescriptorNode) {
                                        plugin.type_descriptor.type = typeDescriptorNode.attribute("name").value();
                                    }

                                    optionalFileGroup.plugins.push_back(plugin);
                                }

                                installStep.optional_file_groups.push_back(optionalFileGroup);
                            }

                            config.install_steps.push_back(installStep);
                        }
                    }


                    pugi::xml_node conditionalFileInstallsNode = root.child("conditionalFileInstalls");
                    if (conditionalFileInstallsNode) {
                        for (pugi::xml_node patternNode = conditionalFileInstallsNode.child("patterns").child("pattern");
                             patternNode;
                             patternNode = patternNode.next_sibling("pattern")) {
                            FilePattern filePattern;
                            filePattern.source = patternNode.child("files").child("folder").attribute("source").value();
                            filePattern.destination = patternNode.child("files").child("folder").attribute("destination").value();
                            try {
                                filePattern.priority = std::stoi(
                                        patternNode.child("files").child("folder").attribute("priority").value());
                            }catch (std::exception sx){
                                std::cout << "the heck at "<< filePattern.source << std::endl;
                            }
                            // Parse flagDependencies within pattern
                            for (pugi::xml_node flagDependencyNode = patternNode.child("dependencies").child("flagDependency");
                                 flagDependencyNode;
                                 flagDependencyNode = flagDependencyNode.next_sibling("flagDependency")) {
                                FlagDependency flagDependency;
                                flagDependency.flag = flagDependencyNode.attribute("flag").value();
                                flagDependency.value = flagDependencyNode.attribute("value").value();
                                filePattern.dependencies.push_back(flagDependency);
                            }

                            config.conditional_file_installs.patterns.push_back(filePattern);
                        }
                    }
                    config.populated = true;
                } else {
                    std::cerr << "Error: Root node 'config' not found in XML." << std::endl;
                    config.populated = false;
                }
            } else {
                std::cerr << "Error: XML parsing failed." << std::endl;
                config.populated = false;
            }

//            std::cout << "Module Name: " << config.module_name << std::endl;
//            std::cout << "Module Image Path: " << config.module_image_path << std::endl;
//
//            std::cout << "Required Install Files:" << std::endl;
//            for (const RequiredInstallFile& file : config.required_install_files) {
//                std::cout << "    Source: " << file.source << ", Destination: " << file.destination << std::endl;
//            }
//
//            std::cout << "Install Steps:" << std::endl;
//            for (const InstallStep& installStep : config.install_steps) {
//                std::cout << "  Name: " << installStep.name << std::endl;
//                for (const OptionalFileGroup& optionalFileGroup : installStep.optional_file_groups) {
//                    std::cout << "    Group Name: " << optionalFileGroup.name << ", Type: " << optionalFileGroup.type << std::endl;
//                    for (const Plugin& plugin : optionalFileGroup.plugins) {
//                        std::cout << "      Plugin Name: " << plugin.name << std::endl;
//                        std::cout << "      Description: " << plugin.description << std::endl;
//                        std::cout << "      Type Descriptor: " << plugin.type_descriptor.type << std::endl;
//                        std::cout << "      Condition Flags:" << std::endl;
//                        for (const ConditionFlag& flag : plugin.condition_flags) {
//                            std::cout << "        Flag Name: " << flag.name << ", Value: " << flag.value << std::endl;
//                        }
//                        std::cout << "      Files:" << std::endl;
//                        for (const Folder& file : plugin.files) {
//                            std::cout << "        File Source: " << file.source << ", Destination: " << file.destination << ", Priority: " << file.priority << std::endl;
//                        }
//                    }
//                }
//            }


            return config;
        }

        std::string toLowerCase(const std::string& str) {
            std::string result = str;
            for (char& c : result) {
                c = std::tolower(c);
            }
            return result;
        }

        Config CurrentConfig;
        std::string Pth;
        bool running = false;

        bool runInstaller(const std::string& modFolder){
            std::string directoryPath = modFolder;
            std::string targetDirectory = "fomod";
            Pth = modFolder + "/build";
            fs::create_directories(Pth);

            fs::path searchPath(directoryPath);

            if (fs::exists(searchPath) && fs::is_directory(searchPath)) {
                for (const auto& entry : fs::directory_iterator(searchPath)) {
                    if (entry.is_directory() &&  toLowerCase(entry.path().filename()) == targetDirectory) {
                        for(const auto& subentry : fs::directory_iterator(entry.path())){
                            if(fs::is_regular_file(subentry)){
                                std::string entryFilename = subentry.path().filename().string();
                                std::string lowercaseEntryFilename = "moduleconfig.xml";

                                if(toLowerCase(entryFilename) == lowercaseEntryFilename){
                                    CurrentConfig = ParseModuleConfig(entry.path().string() + "/" + entryFilename);

                                }
                            }
                        }
                        break;
                    }
                }
            } else {
                return false;
            }


            if(!CurrentConfig.populated){
                return false;
            }
            CurrentConfig.currentStep = CurrentConfig.install_steps[1];
            CurrentConfig.currentGroup = CurrentConfig.install_steps[1].optional_file_groups[0];
            running = true;
            return true;

        }

        void build(){
            if(!running) return;

        }

        void pickOption(int i){
            if(!running) return;
        }

        void setFlag(){
            if(!running) return;
        }

        void showMenus(){
            if(!running) return;
            ImGuiIO &io = ImGui::GetIO();
            ImGui::SetNextWindowSize(io.DisplaySize, 0);
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGuiWindowFlags windowFlags = 0;
            windowFlags += ImGuiWindowFlags_NoMove;
            windowFlags += ImGuiWindowFlags_NoResize;
            windowFlags += ImGuiWindowFlags_NoCollapse;
            windowFlags += ImGuiWindowFlags_NoTitleBar;
            windowFlags += ImGuiWindowFlags_MenuBar;
            ImGui::Begin("Bleh",NULL,windowFlags);
            ImGui::SetWindowFocus("Bleh");
            ImGui::Text(CurrentConfig.currentStep.name.c_str());
            ImGui::Text(CurrentConfig.currentStep.name.c_str());
            ImGui::Text(CurrentConfig.currentGroup.type.c_str());
            ImGui::Separator();

            for (const Plugin& plugin : CurrentConfig.currentGroup.plugins) {
                ImGui::Text(plugin.name.c_str());
                ImGui::Text(plugin.description.c_str());

                //ImGui::Text(plugin.type_descriptor.type) // NEED TO ADD THE ABILITY TO DETECT DEPENDENCIES HERE MC.xml LINE 52..
                ImGui::Separator();
            }
            ImGui::End();
        }


    private:
        lampFoMod(){}
    };
}


#endif //LAMP_LAMPFOMOD_H
