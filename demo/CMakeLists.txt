cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        demo_probe.h

        probe_alsa.cpp
        probe_alsa.cpp
        )

add_executable(demo_alsa ${SOURCES})
target_link_libraries(demo_alsa audiocard_io ${ALSA_LIBRARIES})