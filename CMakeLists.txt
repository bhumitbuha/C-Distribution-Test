cmake_minimum_required(VERSION 3.16)
project(DistributionTester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# If a local vcpkg_installed/ tree exists, use it without requiring vcpkg on PATH.
# (vcpkg's own toolchain file, if provided, takes precedence and supersedes this.)
if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows" AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
endif()

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)
find_package(TGUI 1 REQUIRED)
find_package(OpenMP)

add_executable(DistributionTester
    main.cpp
    TestDistribution.h
    UniformTest.h
    NormalTest.h
)

target_link_libraries(DistributionTester PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    TGUI::TGUI
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(DistributionTester PRIVATE OpenMP::OpenMP_CXX)
endif()

# Copy all vcpkg runtime DLLs (including transitive deps like freetype, ogg, FLAC)
# next to the executable on Windows. TARGET_RUNTIME_DLLS does not follow
# transitive runtime deps, so we mirror the whole bin/ folder for the active config.
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
    set(_VCPKG_BIN_RELEASE "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/bin")
    set(_VCPKG_BIN_DEBUG   "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/debug/bin")
    add_custom_command(TARGET DistributionTester POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            $<IF:$<CONFIG:Debug>,${_VCPKG_BIN_DEBUG},${_VCPKG_BIN_RELEASE}>
            $<TARGET_FILE_DIR:DistributionTester>
    )
endif()
