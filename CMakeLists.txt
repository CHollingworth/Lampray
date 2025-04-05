cmake_minimum_required(VERSION 3.26)
project(Lampray)

set(CMAKE_CXX_STANDARD 17)


option(USE_XDG_DIRECTORY "Use XDG directory to store data instead of the working directory" ON)

# FetchContent allows us to download "stuff" during configuration and integrates nicely with other cmake projects
include(FetchContent)
FetchContent_Declare(
  bit7z
  GIT_REPOSITORY    https://github.com/rikyoz/bit7z.git
  GIT_TAG           v4.0.0
  PATCH_COMMAND     git apply ${CMAKE_SOURCE_DIR}/bit7z.patch # find_if is not included via <algorithm> -> patch it in
  UPDATE_DISCONNECTED 1 # Only try to patch once
)
FetchContent_Declare(
  pugixml
  GIT_REPOSITORY    https://github.com/zeux/pugixml.git
  GIT_TAG           v1.14
)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY    https://github.com/ocornut/imgui.git
  GIT_TAG           v1.90
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(
  json
  GIT_REPOSITORY    https://github.com/nlohmann/json.git
  GIT_TAG           v3.11.3
)
FetchContent_Declare(
  nfd_content       # None cmake project - will have to be consumed by externalproject
  GIT_REPOSITORY    https://github.com/mlabbe/nativefiledialog.git
  GIT_TAG           release_116
)
FetchContent_Declare(
  lz4_content       # None cmake project - will have to be consumed by externalproject
  GIT_REPOSITORY    https://github.com/lz4/lz4.git
  GIT_TAG           v1.10.0
)
# At this point the downlow will start - building will only happen if a dependee is added
FetchContent_MakeAvailable(bit7z pugixml json imgui nfd_content lz4_content)

FetchContent_GetProperties(imgui SOURCE_DIR imgui_SRC)
add_executable(${PROJECT_NAME} main.cpp
        ${imgui_SRC}/backends/imgui_impl_sdl2.cpp
        ${imgui_SRC}/backends/imgui_impl_sdlrenderer2.cpp
        ${imgui_SRC}/imconfig.h
        ${imgui_SRC}/imgui.cpp
        ${imgui_SRC}/imgui.h
        ${imgui_SRC}/imgui_demo.cpp
        ${imgui_SRC}/imgui_draw.cpp
        ${imgui_SRC}/imgui_internal.h
        ${imgui_SRC}/imgui_tables.cpp
        ${imgui_SRC}/imgui_widgets.cpp
        ${imgui_SRC}/imstb_rectpack.h
        ${imgui_SRC}/imstb_textedit.h
        ${imgui_SRC}/imstb_truetype.h
        Lampray/Base/lampBase.h
        Lampray/Control/lampConfig.cpp
        Lampray/Control/lampConfig.h
        Lampray/Control/lampControl.cpp
        Lampray/Control/lampControl.h
        Lampray/Control/lampGames.h
        Lampray/Filesystem/lampExtract.cpp
        Lampray/Filesystem/lampFS.h
        Lampray/Filesystem/lampIO.cpp
        Lampray/Filesystem/lampShare.cpp
        Lampray/Filesystem/lampTrack.cpp
        Lampray/Filesystem/lampUpdate.cpp
        Lampray/Lang/lampLang.h
        Lampray/Menu/lampCustomise.h
        Lampray/Menu/lampMenu.cpp
        Lampray/Menu/lampMenu.h
        Lampray/Parse/lampParse.h
        VERSION
        game-data/BG3/BG3.cpp
        game-data/BG3/BG3.h
        game-data/C77/C77.cpp
        game-data/C77/C77.h
        game-data/LHL/LHL.cpp
        game-data/LHL/LHL.h
        game-data/gameControl.h
)

if(USE_XDG_DIRECTORY)
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_XDG)
endif(USE_XDG_DIRECTORY)



# Consume nfd from FetchContent
include(ExternalProject)
execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
if(${ARCHITECTURE} STREQUAL x86_64)
  set(nfd_LIB build/lib/Release/x64/libnfd.a)
else()
  message(FATAL_ERROR Missing architecture support)
endif()
FetchContent_GetProperties(nfd_content SOURCE_DIR nfd_SRC)
ExternalProject_Add(
  nfd
  SOURCE_DIR        ${nfd_SRC}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     make -C build/gmake_linux nfd
  BUILD_BYPRODUCTS  "${nfd_LIB}"
  BUILD_IN_SOURCE   1
  INSTALL_COMMAND   "" # All static
)
ExternalProject_Get_Property(nfd SOURCE_DIR)
add_library(libnfd STATIC IMPORTED) # Convenience target
add_dependencies(libnfd nfd)
set_property(TARGET libnfd PROPERTY IMPORTED_LOCATION "${SOURCE_DIR}/${nfd_LIB}")
target_include_directories(libnfd INTERFACE ${SOURCE_DIR}/src/include)

# Consume lz4 from FetchContent
set(lz4_LIB lib/liblz4.a)
FetchContent_GetProperties(lz4_content SOURCE_DIR lz4_SRC)
ExternalProject_Add(
  lz4
  SOURCE_DIR        ${lz4_SRC}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     make lib-release
  BUILD_BYPRODUCTS  "${lz4_LIB}"
  BUILD_IN_SOURCE   1
  INSTALL_COMMAND   "" # All static
)
add_library(liblz4 STATIC IMPORTED) # Convenience target
add_dependencies(liblz4 lz4)
set_property(TARGET liblz4 PROPERTY IMPORTED_LOCATION "${lz4_SRC}/${lz4_LIB}")
target_include_directories(liblz4 INTERFACE ${lz4_SRC}/lib)



# Add dependencies to Lampray
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SRC})

FetchContent_GetProperties(bit7z SOURCE_DIR bit7z_SRC)
target_link_libraries(${PROJECT_NAME} bit7z)
target_include_directories(${PROJECT_NAME} PUBLIC ${bit7z_SRC}/include/bit7z)

target_link_libraries(${PROJECT_NAME} liblz4)
target_link_libraries(${PROJECT_NAME} libnfd)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} pugixml::pugixml)



# Find system packages
find_package(PkgConfig REQUIRED)

find_package(SDL2 REQUIRED) # imgui
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

find_package(CURL REQUIRED) # update check
target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})

find_package(GTK2 REQUIRED) # nfd
target_link_libraries(${PROJECT_NAME} ${GTK2_LIBRARIES})

