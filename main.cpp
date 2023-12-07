#include "third-party/imgui/imgui.h"
#include "third-party/imgui/imgui_impl_sdl2.h"
#include "third-party/imgui/imgui_impl_sdlrenderer2.h"

#include "Lamp/Control/lampControl.h"
#include "Lamp/Menu/lampMenu.h"
#include "Lamp/Menu/lampCustomise .h"
#include <stdio.h>
#include "SDL2/SDL.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Lampray", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    std::filesystem::path fontFolder("Font/");

    // Check if the "Font" folder exists
    if (std::filesystem::is_directory(fontFolder)) {
        for (const auto& entry : std::filesystem::directory_iterator(fontFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ttf") {
                io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 16.0f);
                break;
            }
        }
    }

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);





    Lamp::Core::lampControl::getInstance().Colour_SearchHighlight = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[13]);
    ImGui::GetStyle().Colors[ImGuiCol_Text] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[0]);
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[1]);
    ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[2]);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[3]);
    ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[4]);
    ImGui::GetStyle().Colors[ImGuiCol_Button] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[5]);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[6]);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[7]);

    ImGui::GetStyle().Colors[ImGuiCol_Header] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[8]);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[9]);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[10]);
	ImGui::GetStyle().Colors[ImGuiCol_Separator] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[11]);
    ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered] = Lamp::Core::Base::lampTypes::lampHexAlpha(Lamp::Core::lampCustomise::getInstance().defaultColours[12]);

    std::string PreviousGame = Lamp::Core::FS::lampIO::loadKeyData("PreviousGame","LAMP CONFIG");
    if(PreviousGame != "") {
        Lamp::Games::getInstance().currentGameInt = std::stoi(PreviousGame);
        Lamp::Games::getInstance().currentGame = Lamp::Games::getInstance().gameList[Lamp::Games::getInstance().currentGameInt];
    }

    Lamp::Core::Base::lampLog::getInstance().log("Clearing log file.");
    const std::string filename = "lamp.log";
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        file.close();
        Lamp::Core::Base::lampLog::getInstance().log("Couldn't clear the Log.");
    }
    file.close();

    Lamp::Core::Base::lampLog::getInstance().log(Lamp::Core::lampControl::getFormattedTimeAndDate()+" | | Battle Control Online, Welcome Back Commander.", Lamp::Core::Base::lampLog::LOG);

    Lamp::Games::getInstance();

    std::string loadedCheckUpdates = Lamp::Core::FS::lampIO::loadKeyData("Check_Updates_Startup", "LAMP CONFIG");
    bool checkForUpdates = true;
    if(loadedCheckUpdates == "0" || loadedCheckUpdates == "false"){
        checkForUpdates = false;
    }
    if(checkForUpdates){
        Lamp::Core::FS::lampUpdate::getInstance().checkForUpdates();
    }
    Lamp::Core::lampConfig::getInstance().lampFlags["showIntroMenu"]=(std::string)Lamp::Core::FS::lampIO::loadKeyData("showIntroMenu","LAMP CONFIG").returnReason;
    Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation = (std::string)Lamp::Core::FS::lampIO::loadKeyData("bit7zLibaryLocation","LAMP CONFIG").returnReason;
    Lamp::Core::lampConfig::getInstance().init();
    Lamp::Core::FS::lampIO::saveKeyData("bit7zLibaryLocation", Lamp::Core::lampConfig::getInstance().bit7zLibaryLocation, "LAMP CONFIG");

    Lamp::Core::lampMenu Menus;
    // This is a very inefficent way of doing this.


    Lamp::Games::getInstance().currentProfile = Lamp::Games::getInstance().currentGame->KeyInfo()["CurrentProfile"];

    // Try to load and set the global font scale. I am pretty sure this is not a good way of doing this...
    std::string loadedFontScale = Lamp::Core::FS::lampIO::loadKeyData("Font_Scale", "LAMP CONFIG");
    if(loadedFontScale != ""){
        io.FontGlobalScale = std::stof(loadedFontScale);
    }

    Lamp::Core::Base::lampLog::getInstance().log("Creating Directories");
    try {
        std::filesystem::create_directories(Lamp::Core::lampConfig::getInstance().saveDataPath);
        std::filesystem::create_directories(Lamp::Core::lampConfig::getInstance().archiveDataPath);
        std::filesystem::create_directories(Lamp::Core::lampConfig::getInstance().ConfigDataPath);
        std::filesystem::create_directories(Lamp::Core::lampConfig::getInstance().DeploymentDataPath);

        for (Lamp::Game::gameControl *element: Lamp::Games::getInstance().gameList){
            std::filesystem::create_directories(std::filesystem::path(Lamp::Core::lampConfig::getInstance().DeploymentDataPath + element->Ident().ReadableName));
            std::filesystem::create_directories(std::filesystem::path(Lamp::Core::lampConfig::getInstance().archiveDataPath + element->Ident().ReadableName + "/GameFiles"));
        }

    } catch (std::exception ex) {
        Lamp::Core::Base::lampLog::getInstance().log("Could not create base directories", Lamp::Core::Base::lampLog::ERROR,
                                                     Lamp::Core::Base::lampLog::LMP_NODIRCREATION);
    }

    Lamp::Core::Base::lampLog::getInstance().log("Directories Created");

    Lamp::Core::lampCustomise::getInstance().getConfigColours();

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;

            if (event.type == SDL_DROPFILE) {
                // A file has been dropped
                char* droppedFile = event.drop.file;
                Lamp::Core::FS::lampIO::fileDrop(droppedFile);
                SDL_free(droppedFile);
            }
        }


        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Menus.RunMenus();

        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
