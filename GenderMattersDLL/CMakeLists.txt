cmake_minimum_required(VERSION 3.21)

project(GenderMattersDLL VERSION 1.0.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# This expects to be built the same way the official CommonLibSSE-NG sample
# plugin is built: with vcpkg providing CommonLibSSE-NG itself. See README.md
# for the exact one-time setup (Visual Studio + vcpkg + CMake, ~15 minutes).
# ---------------------------------------------------------------------------

find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME}
	SOURCES
		src/main.cpp
		src/Settings.cpp
		src/PerkPatcher.cpp
		src/GenderDistributor.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Drop the compiled DLL straight into a Data-folder-shaped layout next to the
# build directory, so you can just copy build/Data/* into your game's Data
# folder when done.
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Data/SKSE/Plugins"
)
