//
// Created by charles on 02/09/23.
//

#include <iostream>
#include "LampMenus.h"
#include "LampFS.h"
#include "nfd/include/nfd.h"

void LampMenus::Menu() {
    io = ImGui::GetIO();
    io.DisplaySize.x;
    io.DisplaySize.y;
    ImGui::SetNextWindowSize(io.DisplaySize, 0);
    ImGui::SetNextWindowPos(ImVec2(0, 0));


    MainMenu();
}


void LampMenus::MainMenu(){

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!", NULL, CreateFlags());
    MenuBar();

    ImGui::Text("Hello! This is a very basic modding tool made for linux. Make sure you configure game paths before deploying mods.");
    ImGui::Text("This tool works by forcefully overwriting the mods directory, currently bin & data overwite mods CANNOT be undone.");



    if(ImGui::Button("Deploy")){
        LampFS::getInstance().deploy();
    }

    ImGui::Separator();

    if(ImGui::BeginTable("Split", 3)){
        ImGui::TableNextColumn();
        ImGui::Text("Mod Name");
        ImGui::TableNextColumn();
        ImGui::Text("Mod Type");
        ImGui::TableNextColumn();
        ImGui::Text("Remove Mod");
        ImGui::TableNextRow();

        // Test row

        LampFS::getInstance().listArchives();


        ImGui::EndTable();
    }
    ImGui::Separator();

    ImGui::Separator();

   // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();


    // 3. Show another simple window.
    if (bg3_conf)
    {
        io.DisplaySize.x;
        io.DisplaySize.y;
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("BG3 Steam Path Setup", NULL, CreateFlags());   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

        ImGui::Text("BG3 Steam Directory");
        if(ImGui::Button((LampFS::getInstance().installDirPath+"##installpath").c_str())){
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_PickFolder( NULL, &outPath );

            if ( result == NFD_OKAY ) {
                puts(outPath);
                LampFS::getInstance().installDirPath = outPath;
                LampFS::getInstance().saveConfig();
            }
            else if ( result == NFD_CANCEL ) {
                puts("User pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError() );
            }
        }
        ImGui::Separator();

        ImGui::Text("BG3 AppData Directory");
        if(ImGui::Button((LampFS::getInstance().appDataPath+"##appdatapath").c_str())){
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_PickFolder( NULL, &outPath );

            if ( result == NFD_OKAY ) {
                puts(outPath);
                LampFS::getInstance().appDataPath = outPath;
                LampFS::getInstance().saveConfig();
            }
            else if ( result == NFD_CANCEL ) {
                puts("User pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError() );
            }
        }
        ImGui::Separator();

        if (ImGui::Button("Close & Save"))
            bg3_conf = false;
        ImGui::End();
    }

    if(LampFS::getInstance().deploymentStage != LampFS::deployment_stages::BG3_IDLE){
        io.DisplaySize.x;
        io.DisplaySize.y;
        ImGui::SetNextWindowSize(io.DisplaySize, 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Deploying", NULL, CreateFlags());   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text(("Deploying Mods: "+std::to_string(LampFS::getInstance().deployDisplayCount) +" / "+ std::to_string(LampFS::getInstance().deployDisplayMax)).c_str());

        std::string ds = "";
        switch (LampFS::getInstance().deploymentStage) {
            case LampFS::deployment_stages::BG3_PRE_DEPLOYMENT:
                ds = "Pre Deployment";
                break;
            case LampFS::deployment_stages::BG3_PRE_DEPLOYMENT_CLEAN_UP:
                ds = "Cleaning Up";
                break;
            case LampFS::deployment_stages::BG3_DEPLOYMENT:
                ds = "Deployment";
                break;
            case LampFS::deployment_stages::BG3_CLEANUP:
                ds = "Cleaning Up";
                break;
        }
        ImGui::Text(("Deployment Stage: "+ds).c_str());
        ImGui::Text("Please do not close this program.");
        ImGui::End();
    }

}

void LampMenus::MenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game Configuration")) {
                bg3_conf = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About")) {
            ImGui::MenuItem("Created by Charles Hollingworth");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

}

ImGuiWindowFlags LampMenus::CreateFlags(){
    ImGuiWindowFlags windowFlags = 0;
    windowFlags += ImGuiWindowFlags_NoMove;
    windowFlags += ImGuiWindowFlags_NoResize;
    windowFlags += ImGuiWindowFlags_NoCollapse;
    windowFlags += ImGuiWindowFlags_NoTitleBar;
    windowFlags += ImGuiWindowFlags_MenuBar;
    return windowFlags;
}