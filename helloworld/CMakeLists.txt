cmake_minimum_required(VERSION 3.10)

project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(HelloWorld
    main.cpp
    src/logger.cpp
)

# Automatically add all example sources under src/examples
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/*.cpp)
if (EXAMPLE_SOURCES)
    target_sources(HelloWorld PRIVATE ${EXAMPLE_SOURCES})
endif()

target_include_directories(HelloWorld PRIVATE ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HelloWorld PRIVATE fmt::fmt ${OpenCV_LIBS})

# clang-tidy integration (runs during build if available)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if (CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# clang-format helper target (format all sources in this target)
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if (CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-format on sources")
endif()

# Keep compile_commands.json next to sources for editor tooling (clangd, VS Code)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/compile_commands.json
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Copy compile_commands.json to source dir")
endif()

# ----------------------------------------------------------------------------
# Documentation: Doxygen (with optional PlantUML)
# ----------------------------------------------------------------------------
find_package(Doxygen QUIET)
if (DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
    set(DOXYGEN_INPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/docs"
    )
    string(REPLACE ";" " " DOXYGEN_INPUT_STR "${DOXYGEN_INPUT}")

    # Allow user to pass -DPLANTUML_JAR=/path/to/plantuml.jar
    set(PLANTUML_JAR "" CACHE FILEPATH "Path to plantuml.jar for Doxygen PlantUML integration")
    if (NOT PLANTUML_JAR)
        # Try common path on Debian/Ubuntu
        find_file(PLANTUML_JAR_PATH_DEFAULT plantuml.jar PATHS /usr/share/plantuml NO_CACHE)
        set(PLANTUML_JAR_PATH "${PLANTUML_JAR_PATH_DEFAULT}")
    else()
        set(PLANTUML_JAR_PATH "${PLANTUML_JAR}")
    endif()

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else()
    message(STATUS "Doxygen not found: 'docs' target will be a stub")
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Doxygen not found. Install 'doxygen' (and graphviz, plantuml) then reconfigure."
        VERBATIM)
endif()
