cmake_minimum_required(VERSION 3.19)

project(yellow-rats)

find_package(OpenCV 4 REQUIRED)
include(FindPkgConfig)
pkg_search_module(ZBAR REQUIRED zbar>=0.23)

add_executable(${PROJECT_NAME} "main.cpp")

set(DEPS "${CMAKE_SOURCE_DIR}/deps")

target_include_directories(${PROJECT_NAME} PRIVATE ${DEPS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${ZBAR_LIBRARIES})
