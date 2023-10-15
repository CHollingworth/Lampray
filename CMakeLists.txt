cmake_minimum_required(VERSION 3.26)
project(Lamp)

set(CMAKE_CXX_STANDARD 17)




add_executable(Lamp main.cpp
        third-party/imgui/imconfig.h
        third-party/imgui/imgui.cpp
        third-party/imgui/imgui.h
        third-party/imgui/imgui_demo.cpp
        third-party/imgui/imgui_draw.cpp
        third-party/imgui/imgui_impl_vulkan.cpp
        third-party/imgui/imgui_impl_vulkan.h
        third-party/imgui/imgui_internal.h
        third-party/imgui/imgui_tables.cpp
        third-party/imgui/imgui_widgets.cpp
        third-party/imgui/imstb_rectpack.h
        third-party/imgui/imstb_textedit.h
        third-party/imgui/imstb_truetype.h
        third-party/imgui/imgui_impl_glfw.cpp
        third-party/json/json.hpp
        game-data/gameControl.h
        third-party/l4z/lz4.c
        third-party/l4z/lz4.h
        VERSION
        Lamp/Base/lampBase.h
        Lamp/Filesystem/lampFS.h
        Lamp/Control/lampControl.h
        Lamp/Control/lampControl.cpp
        Lamp/Parse/lampParse.h
        game-data/BG3/BG3.cpp
        game-data/BG3/BG3.h
        Lamp/Control/lampConfig.h
        Lamp/Control/lampConfig.cpp
        Lamp/Filesystem/lampExtract.cpp
        Lamp/Filesystem/lampIO.cpp
        Lamp/Menu/lampMenu.h
        Lamp/Menu/lampMenu.cpp
        Lamp/Filesystem/lampUpdate.cpp
        Lamp/Control/lampGames.h
        Lamp/Filesystem/lampShare.cpp
        Lamp/Menu/lampColour.h
        Lamp/Filesystem/lampTrack.cpp
        game-data/SSE/SSE.cpp
        game-data/SSE/SSE.h
)




find_package(PkgConfig REQUIRED)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/bit7z/lib/x64/libbit7z64.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third-party/bit7z/include/include/bit7z )

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/nfd/lib/libnfd.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third-party/nfd/include/include/nfd.h )



pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})



find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

find_package(pugixml REQUIRED)
target_link_libraries(${PROJECT_NAME} pugixml::pugixml)
add_definitions(-DGLFW_INCLUDE_VULKAN)


find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third-party/l4z/liblz4.so)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third-party/l4z/)

