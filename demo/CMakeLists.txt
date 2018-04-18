cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)


if(ALSA)
    set(ALSA_SOURCES
            demo_probe.h
            probe_alsa.cpp
            )
    add_executable(demo_alsa ${ALSA_SOURCES})
    target_link_libraries(demo_alsa audiocard_io ${ALSA_LIBRARIES})
endif(ALSA)

if(ASIO)
    set(ASIO_SOURCES
            demo_probe.h
            probe_asio.cpp
            )
    add_executable(demo_asio ${ASIO_SOURCES})
    target_link_libraries(demo_asio audiocard_io ${ASIO_LIBRARIES})
endif(ASIO)
