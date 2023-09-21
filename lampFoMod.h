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

        struct ConditionFlag {
            std::string name;
            int value;
        };

        struct FileDependency {
            std::string file;
            std::string state;
        };

        struct TypeDescriptor {
            std::string name;
            std::string defaultType;
        };

        struct Plugin {
            std::string name;
            std::string description;
            std::string image;
            std::vector<ConditionFlag> conditionFlags;
            TypeDescriptor typeDescriptor;
            std::vector<FileDependency> fileDependencies;
        };

        struct OptionalFileGroup {
            std::string name;
            std::string type;
            std::vector<Plugin> plugins;
        };

        struct InstallStep {
            std::string name;
            std::vector<OptionalFileGroup> optionalFileGroups;
        };

        struct FilePattern {
            std::string pattern;
            std::vector<ConditionFlag> dependencies;
            std::string folderSource;
            std::string folderDestination;
            int priority;
        };

        struct Config {
            std::string moduleName;
            std::string moduleImage;
            std::string moduleDependenciesOperator;
            std::vector<FileDependency> moduleDependencies;
            std::vector<InstallStep> installSteps;
            std::vector<FilePattern> conditionalFilePatterns;

            InstallStep currentStep;
            OptionalFileGroup currentGroup;
            bool populated = true;
        };

        Config ParseModuleConfig(const std::string& xmlContent) {
            // Load and parse the XML document
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(xmlContent.c_str());
            Config config;
            if (!result) {
                std::cerr << "Error parsing XML: " << result.description() << std::endl;
                config.populated = false;
                return config;
            }

            // Create an instance of the Config struct to hold the parsed data


            // Access and parse the XML elements to populate the struct
            pugi::xml_node rootNode = doc.child("config");
            config.moduleName = rootNode.child("moduleName").text().as_string();
            config.moduleImage = rootNode.child("moduleImage").attribute("path").as_string();
            config.moduleDependenciesOperator = rootNode.child("moduleDependencies").attribute("operator").as_string();

            // Parse moduleDependencies
            for (pugi::xml_node fileDependencyNode = rootNode.child("moduleDependencies").child("fileDependency"); fileDependencyNode; fileDependencyNode = fileDependencyNode.next_sibling("fileDependency")) {
                FileDependency fileDependency;
                fileDependency.file = fileDependencyNode.attribute("file").as_string();
                fileDependency.state = fileDependencyNode.attribute("state").as_string();
                config.moduleDependencies.push_back(fileDependency);
            }

            // Parse installSteps
            for (pugi::xml_node installStepNode = rootNode.child("installSteps").child("installStep"); installStepNode; installStepNode = installStepNode.next_sibling("installStep")) {
                InstallStep installStep;
                installStep.name = installStepNode.attribute("name").as_string();

                // Parse optionalFileGroups within installStep
                for (pugi::xml_node optionalFileGroupNode = installStepNode.child("optionalFileGroups").child("group"); optionalFileGroupNode; optionalFileGroupNode = optionalFileGroupNode.next_sibling("group")) {
                    OptionalFileGroup optionalFileGroup;
                    optionalFileGroup.name = optionalFileGroupNode.attribute("name").as_string();
                    optionalFileGroup.type = optionalFileGroupNode.attribute("type").as_string();

                    // Parse plugins within optionalFileGroup
                    for (pugi::xml_node pluginNode = optionalFileGroupNode.child("plugins").child("plugin"); pluginNode; pluginNode = pluginNode.next_sibling("plugin")) {
                        Plugin plugin;
                        plugin.name = pluginNode.attribute("name").as_string();
                        plugin.description = pluginNode.child("description").text().as_string();
                        plugin.image = pluginNode.child("image").attribute("path").as_string();

                        // Parse conditionFlags within plugin
                        for (pugi::xml_node flagNode = pluginNode.child("conditionFlags").child("flag"); flagNode; flagNode = flagNode.next_sibling("flag")) {
                            ConditionFlag flag;
                            flag.name = flagNode.attribute("name").as_string();
                            flag.value = flagNode.text().as_int();
                            plugin.conditionFlags.push_back(flag);
                        }

                        // Parse TypeDescriptor
                        pugi::xml_node typeDescriptorNode = pluginNode.child("typeDescriptor");
                        plugin.typeDescriptor.name = typeDescriptorNode.child("type").name();
                        plugin.typeDescriptor.defaultType = typeDescriptorNode.child("defaultType").attribute("name").as_string();

                        // Parse fileDependencies within plugin
                        for (pugi::xml_node fileDependencyNode = pluginNode.child("fileDependencies").child("fileDependency"); fileDependencyNode; fileDependencyNode = fileDependencyNode.next_sibling("fileDependency")) {
                            FileDependency fileDependency;
                            fileDependency.file = fileDependencyNode.attribute("file").as_string();
                            fileDependency.state = fileDependencyNode.attribute("state").as_string();
                            plugin.fileDependencies.push_back(fileDependency);
                        }

                        optionalFileGroup.plugins.push_back(plugin);
                    }

                    installStep.optionalFileGroups.push_back(optionalFileGroup);
                }

                config.installSteps.push_back(installStep);
            }

            // Parse conditionalFilePatterns
            for (pugi::xml_node patternNode = rootNode.child("conditionalFileInstalls").child("patterns").child("pattern"); patternNode; patternNode = patternNode.next_sibling("pattern")) {
                FilePattern conditionalFilePattern;
                conditionalFilePattern.pattern = patternNode.attribute("pattern").as_string();

                // Parse dependencies within conditionalFilePattern
                for (pugi::xml_node flagDependencyNode = patternNode.child("dependencies").child("flagDependency"); flagDependencyNode; flagDependencyNode = flagDependencyNode.next_sibling("flagDependency")) {
                    ConditionFlag flag;
                    flag.name = flagDependencyNode.attribute("flag").as_string();
                    flag.value = flagDependencyNode.attribute("value").as_int();
                    conditionalFilePattern.dependencies.push_back(flag);
                }

                conditionalFilePattern.folderSource = patternNode.child("source").text().as_string();
                conditionalFilePattern.folderDestination = patternNode.child("destination").text().as_string();
                conditionalFilePattern.priority = patternNode.child("priority").text().as_int();

                config.conditionalFilePatterns.push_back(conditionalFilePattern);
            }

            // Print the parsed data (you can customize this part as needed)
            std::cout << "Module Name: " << config.moduleName << std::endl;
            std::cout << "Module Image Path: " << config.moduleImage << std::endl;
            std::cout << "Module Dependencies Operator: " << config.moduleDependenciesOperator << std::endl;


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
            CurrentConfig.currentStep = CurrentConfig.installSteps[0];
            CurrentConfig.currentGroup = CurrentConfig.installSteps[0].optionalFileGroups[0];
            //std::cout << CurrentConfig.installSteps[0].optionalFileGroups[0].plugins[1].fileDependencies[0].file << std::endl;
            //std::cout << CurrentConfig.installSteps[0].optionalFileGroups[0].plugins[0].fileDependencies[0].state << std::endl;
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
