//
// Created by charles on 15/10/23.
//
#include "lampParse.h"

void Lamp::Core::Parse::lampFoModParser::parse(std::string pathToDocument) {
    pugi::xml_document doc;

    // Load the XML file
    if (!doc.load_file(pathToDocument.c_str())) {
        std::cerr << "Failed to load XML file." << std::endl;
        return;
    }

    // Define XPath expressions to extract data
    const char* installStepXPath = "//installStep";
    const char* descriptionXPath = "description/text()";
    const char* imageXPath = "image/@path";
    const char* groupXPath = "optionalFileGroups/group";
    const char* pluginXPath = "plugins/plugin";
    const char* conditionFlagXPath = "conditionFlags/flag";
    const char* conditionalPatternXPath = "//conditionalFileInstalls/pattern";
    const char* fileDependencyXPath = "dependencies/flagDependency";
    const char* pluginFileDependencyXPath = "dependencies/fileDependency";

    // Create a vector to store InstallStep structs
    std::vector<InstallStep> installSteps;

    // Traverse the XML using XPath
    for (pugi::xpath_node installStep : doc.select_nodes(installStepXPath)) {
        InstallStep step;
        step.name = installStep.node().attribute("name").value();

        pugi::xml_node descriptionNode = installStep.node().select_node(descriptionXPath).node();
        if (descriptionNode) {
            step.description = descriptionNode.value();
        }

        pugi::xml_node imageNode = installStep.node().select_node(imageXPath).node();
        if (imageNode) {
            step.imagePath = imageNode.value();
        }

        // Extract group-related data
        for (pugi::xpath_node groupNode : installStep.node().select_nodes(groupXPath)) {
            Group group;
            group.name = groupNode.node().attribute("name").value();
            group.type = groupNode.node().attribute("type").value();

            // Extract plugins within the group
            for (pugi::xpath_node pluginNode : groupNode.node().select_nodes(pluginXPath)) {
                Plugin plugin;
                plugin.name = pluginNode.node().attribute("name").value();

                // Extract condition flags within the plugin
                for (pugi::xpath_node conditionFlagNode : pluginNode.node().select_nodes(conditionFlagXPath)) {
                    ConditionFlag conditionFlag;
                    conditionFlag.name = conditionFlagNode.node().attribute("name").value();
                    conditionFlag.value = conditionFlagNode.node().text().get();
                    plugin.conditionFlags.push_back(conditionFlag);
                }

                pugi::xml_node pluginDescriptionNode = pluginNode.node().child("description");
                if (pluginDescriptionNode) {
                    plugin.description = pluginDescriptionNode.text().get();
                }

                pugi::xml_node pluginImageNode = pluginNode.node().child("image");
                if (pluginImageNode) {
                    plugin.imagePath = pluginImageNode.attribute("path").value();
                }

                // Extract file dependencies within plugins
                for (pugi::xpath_node fileDependencyNode : pluginNode.node().select_nodes(pluginFileDependencyXPath)) {
                    FileDependency fileDependency;
                    fileDependency.file = fileDependencyNode.node().attribute("file").value();
                    fileDependency.state = fileDependencyNode.node().attribute("value").value();
                    plugin.fileDependencies.push_back(fileDependency);
                }

                group.plugins.push_back(plugin);
            }

            step.groups.push_back(group);
        }

        // Extract conditional patterns
        for (pugi::xpath_node patternNode : doc.select_nodes(conditionalPatternXPath)) {
            ConditionalPattern pattern;
            pattern.folderSource = patternNode.node().child("files/folder").attribute("source").value();
            pattern.folderDestination = patternNode.node().child("files/folder").attribute("destination").value();
            pattern.priority = std::stoi(patternNode.node().child("files/folder").attribute("priority").value());

            // Extract file dependencies within conditional patterns
            for (pugi::xpath_node fileDependencyNode : patternNode.node().select_nodes(fileDependencyXPath)) {
                FileDependency fileDependency;
                fileDependency.file = fileDependencyNode.node().attribute("file").value();
                fileDependency.state = fileDependencyNode.node().attribute("value").value();
                pattern.fileDependencies.push_back(fileDependency);
            }

            step.conditionalPatterns.push_back(pattern);
        }

        installSteps.push_back(step);
    }


    for (const InstallStep& step : installSteps) {
        std::cout << "Install Step Name: " << step.name << std::endl;
        std::cout << "Description: " << step.description << std::endl;
        std::cout << "Image Path: " << step.imagePath << std::endl;

        for (const Group& group : step.groups) {
            std::cout << "Group Name: " << group.name << " (Type: " << group.type << ")" << std::endl;

            for (const Plugin& plugin : group.plugins) {
                std::cout << "Plugin Name: " << plugin.name << std::endl;
                std::cout << "Plugin Description: " << plugin.description << std::endl;
                std::cout << "Plugin Image Path: " << plugin.imagePath << std::endl;

                for (const ConditionFlag& flag : plugin.conditionFlags) {
                    std::cout << "Condition Flag Name: " << flag.name << std::endl;
                    std::cout << "Condition Flag Value: " << flag.value << std::endl;
                }

                for (const FileDependency& fileDependency : plugin.fileDependencies) {
                    std::cout << "File Dependency - File: " << fileDependency.file << std::endl;
                    std::cout << "File Dependency - State: " << fileDependency.state << std::endl;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for (const ConditionalPattern& pattern : step.conditionalPatterns) {
            std::cout << "Pattern - Folder Source: " << pattern.folderSource << std::endl;
            std::cout << "Pattern - Folder Destination: " << pattern.folderDestination << std::endl;
            std::cout << "Pattern - Priority: " << pattern.priority << std::endl;

            for (const FileDependency& fileDependency : pattern.fileDependencies) {
                std::cout << "File Dependency - File: " << fileDependency.file << std::endl;
                std::cout << "File Dependency - State: " << fileDependency.state << std::endl;
            }
        }
    }

    return;
}