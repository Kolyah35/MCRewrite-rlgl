cmake_minimum_required(VERSION 3.10)
project(rd-132211)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:raysan5/raylib#5.5")
# CPMAddpackage("gh:wolfpld/tracy@0.13.1")

file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/src/impl/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} raylib 
    # Tracy::TracyClient
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/resources/terrain.png" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/")
else()
    message("-- WARNING: Don't forget to copy resources from .jar file to a directory with the game before launching!")
endif()