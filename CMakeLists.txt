cmake_minimum_required(VERSION 3.26)
project(Lampray)

set(CMAKE_CXX_STANDARD 17)


option(USE_XDG_DIRECTORY "Use XDG directory to store data instead of the working directory" ON)
option(CHECK_FOR_UPDATES "Enables checking for updates from the Github repo" ON)

add_executable(${PROJECT_NAME} main.cpp
        third-party/imgui/imconfig.h
        third-party/imgui/imgui.cpp
        third-party/imgui/imgui.h
        third-party/imgui/imgui_demo.cpp
        third-party/imgui/imgui_draw.cpp
        third-party/imgui/imgui_internal.h
        third-party/imgui/imgui_tables.cpp
        third-party/imgui/imgui_widgets.cpp
        third-party/imgui/imstb_rectpack.h
        third-party/imgui/imstb_textedit.h
        third-party/imgui/imstb_truetype.h
        third-party/json/json.hpp
        game-data/gameControl.h
        third-party/l4z/lz4.c
        third-party/l4z/lz4.h
        VERSION
        Lampray/Base/lampBase.h
        Lampray/Filesystem/lampFS.h
        Lampray/Control/lampControl.h
        Lampray/Control/lampControl.cpp
        Lampray/Parse/lampParse.h
        game-data/BG3/BG3.cpp
        game-data/BG3/BG3.h
        Lampray/Control/lampConfig.h
        Lampray/Control/lampConfig.cpp
        Lampray/Filesystem/lampExtract.cpp
        Lampray/Filesystem/lampIO.cpp
        Lampray/Menu/lampMenu.h
        Lampray/Menu/lampMenu.cpp
        Lampray/Filesystem/lampUpdate.cpp
        Lampray/Control/lampGames.h
        Lampray/Filesystem/lampShare.cpp
        Lampray/Menu/lampCustomise.h
        Lampray/Filesystem/lampTrack.cpp
        game-data/C77/C77.cpp
        game-data/C77/C77.h
        third-party/imgui/imgui_impl_sdl2.cpp
        third-party/imgui/imgui_impl_sdlrenderer2.cpp
        game-data/LHL/LHL.cpp
        game-data/LHL/LHL.h
        Lampray/Lang/lampLang.h
)

if(USE_XDG_DIRECTORY) 
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_XDG)
endif(USE_XDG_DIRECTORY)


if(CHECK_FOR_UPDATES)
  target_compile_definitions(${PROJECT_NAME} PUBLIC CHECK_FOR_UPDATES)
endif(CHECK_FOR_UPDATES)


find_package(PkgConfig REQUIRED)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/bit7z/lib/x64/libbit7z64.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third-party/bit7z/include/include/bit7z )

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/nfd/lib/libnfd.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third-party/nfd/include/include/nfd.h )


# Find the SDL2 package
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

#add_subdirectory(third-party/SDL2)
#target_link_libraries(${PROJECT_NAME} SDL)

#find_package(pugixml REQUIRED)
#target_link_libraries(${PROJECT_NAME} pugixml::pugixml)

add_subdirectory(third-party/pugixml)
target_link_libraries(${PROJECT_NAME} pugixml::pugixml)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})


target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/l4z/liblz4.so)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third-party/l4z/)

