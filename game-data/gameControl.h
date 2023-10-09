//
// Created by charles on 11/09/23.
//

#ifndef LAMP_GAMECONTROL_H
#define LAMP_GAMECONTROL_H

#include <string>
#include <list>
#include <map>
#include "../Lamp/Base/lampBase.h"


namespace Lamp::Game {
    typedef Core::Base::lampTypes::lampString lampString;
    typedef Core::Base::lampTypes::lampHexAlpha lampHex;
    typedef Core::Base::lampTypes::lampReturn lampReturn;
    typedef Core::Base::lampExecutor lampExecutor;

    /**
 * @brief A base class for controlling game-related functionality.
 *
 * This class defines a set of virtual functions that need to be implemented by derived classes
 * to manage various aspects of a game.
 */
    class gameControl {
    public:

        /**
         * @brief Registers an archive with the game control.
         * This is called during a file drop event onto lamp.
         * @param Path The path to the archive to be registered.
         * @return A lampReturn object indicating the success or failure of the operation.
         */
        virtual lampReturn registerArchive(lampString Path) = 0;

        /**
         * @brief Displays a configuration menu for the game.
         * See Lamp::Game::BG3 for usage and Lamp::Core::lampControl::lampGameSettingsDisplayHelper for creating config menu items.
         * @return A lampReturn object indicating the success or failure of the operation.
         */
        virtual lampReturn ConfigMenu() = 0;

        /**
         * @brief Starts the deployment process for the game.
         * Called externally by Lamp::Core to start deployment.
         * @return A lampReturn object indicating the success or failure of the deployment.
         */
        virtual lampReturn startDeployment() = 0;

        /**
         * @brief Performs pre-cleanup tasks before preDeployment.
         * Called internally, this is the step for cleaning up game files and internal folders.
         * @return A lampReturn object indicating the success or failure of the pre-cleanup.
         */
        virtual lampReturn preCleanUp() = 0;

        /**
         * @brief Performs pre-deployment tasks before Deployment.
         * Called internally, this is the step for extracting archives and moving files to relevant positions before linking.
         * @return A lampReturn object indicating the success or failure of the pre-deployment.
         */
        virtual lampReturn preDeployment() = 0;

        /**
         * @brief Deploys mod files.
         * Called internally, this is the step for linking the local mod setup to the actual game.
         * @return A lampReturn object indicating the success or failure of the deployment.
         */
        virtual lampReturn deployment() = 0;

        /**
         * @brief Performs post-deployment tasks after deployment.
         * Called internally, this would be used for calls to external programs such as fnis or Bodyslide.
         * @return A lampReturn object indicating the success or failure of the post-deployment tasks.
         */
        virtual lampReturn postDeploymentTasks() = 0;

        /**
         * @brief Lists the registered archives in the game control.
         * Called Externally, this should be used with Lamp::Core::lampControl::lampArchiveDisplayHelper unless a custom solution is desired.
         */
        virtual void listArchives() = 0;

        /**
         * @brief Retrieves key information associated with the game control.
         * Returns an internal map that must contain and is also used for any relevant string information.
         * {"ProfileList","Default"},
         * {"CurrentProfile", "Default"}
         * @return A reference to a map containing key-value pairs of information.
         */
        virtual std::map<std::string, std::string>& KeyInfo() = 0;

        /**
         * @brief Retrieves the identification information for the game control.
         * This is the games identifier something that must be set, see Core::Base::lampTypes::lampIdent.
         * @return A lampIdent object representing the identification information.
         */
        virtual Core::Base::lampTypes::lampIdent Ident() = 0;

        /**
         * @brief Retrieves a list of game mods associated with the game control.
         * Returns the current modlist. This is used heavily within lamp.
         * @return A reference to a vector of lampMod pointers representing the game mods.
         */
        virtual std::vector<Core::Base::lampMod::Mod *>& getModList() = 0;

        /**
         * @brief Runs any setup tasks required for the game.
         */
        virtual void launch() = 0;

    protected:
        /**
         * @brief Protected constructor for the game control class.
         *
         * This constructor is protected to ensure that only derived classes can be instantiated.
         */
        gameControl(){}
    };
} // Lamp

#endif //LAMP_GAMECONTROL_H
