cmake_minimum_required(VERSION 3.17)
project(Advanced_RAT2)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

find_package(SFML 2.5.1 COMPONENTS audio graphics window system REQUIRED)
include_directories("C:\\vcpkg\\installed\\x86-windows\\include\\SFML")


add_executable(Advanced_RAT2 main.cpp Connection.cpp Connection.h ShareScreen.cpp ShareScreen.h ShareCamera.cpp ShareCamera.h ShareMicOutput.cpp ShareMicOutput.h)

TARGET_LINK_LIBRARIES (Advanced_RAT2 ${OpenCV_LIBS})
target_link_libraries(Advanced_RAT2 sfml-graphics sfml-audio sfml-window sfml-system)