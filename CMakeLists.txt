cmake_minimum_required(VERSION 3.17)
project(Advanced_RAT2)

set(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

#find_package (SFML REQUIRED system window graphics network audio)
#include_directories(${SFML_INCLUDE_DIR})



add_executable(Advanced_RAT2 main.cpp Connection.cpp Connection.h ShareScreen.cpp ShareScreen.h ShareCamera.cpp ShareCamera.h ShareMicOutput.cpp ShareMicOutput.h)

TARGET_LINK_LIBRARIES (Advanced_RAT2 ${OpenCV_LIBS})
#target_link_libraries(Advanced_RAT2 ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})