cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (LINUX)
find_package(ALSA REQUIRED)
endif(LINUX)

set(SOURCES
            src/aio_asio.cpp
            src/aio_jack.cpp

            include/aio_asio.h
            include/aio_audio.h
            include/aio_jack.h
        )

if(APPLE)
    set(SOURCES
            ${SOURCES}

            src/osx/aio_asioosx.cpp
            src/osx/aio_coreaudio.cpp

            include/osx/aio_asioosx.cpp
            include/osx/aio_coreaudio.cpp

            )
endif(APPLE)

if(LINUX)
    set(SOURCES
            ${SOURCES}

            src/linux/aio_alsa.cpp
            src/linux/aio_pulse.cpp

            include/linux/aio_alsa.h
            include/linux/aio_pulse.h
        )
endif()

if(WIN32)
    set(SOURCES
            ${SOURCES}

            src/windows/aio_asiowin32.cpp
        )
endif()

include_directories("include")

add_library(audiocard_io STATIC ${SOURCES})

add_subdirectory(demo)
add_subdirectory(tests)
