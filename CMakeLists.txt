cmake_minimum_required(VERSION 3.0.0)

project(OTTO CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)
find_package( OpenCV REQUIRED )

add_subdirectory(src)

