cmake_minimum_required(VERSION 3.26)
project(Lamp)

set(CMAKE_CXX_STANDARD 17)




add_executable(Lamp main.cpp
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_impl_vulkan.cpp
        imgui/imgui_impl_vulkan.h
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_impl_glfw.cpp
        json/json.hpp
        lampConfig.h
        lampMenus.cpp
        lampMenus.h
        lampFilesystem.cpp
        lampFilesystem.h
        lampMod.h
        game-data/gameControl.h
        game-data/BG3/BG3.cpp
        game-data/BG3/BG3.h
        lampWarn.h
        l4z/lz4.c
        l4z/lz4.h
        game-data/FO4/FO4.cpp
        game-data/FO4/FO4.h
        lampFoMod.h
        VERSION
        lampArchiveDisplayHelper.h
        lampUpdate.h
)




find_package(PkgConfig REQUIRED)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/bit7z/lib/x64/libbit7z64.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/bit7z/include/include/bit7z )

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/nfd/lib/libnfd.a )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/nfd/include/include/nfd.h )



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

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/l4z/liblz4.so)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/l4z/)

