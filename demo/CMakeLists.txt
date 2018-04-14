cmake_minimum_required(VERSION 3.10)
project(audio_io)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
            demo_alsa.cpp demo_alsa.cpp)

add_executable(demo_alsa ${SOURCES})
target_link_libraries(demo_alsa audiocard_io ${ALSA_LIBRARIES})