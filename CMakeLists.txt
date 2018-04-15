cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
find_package(ALSA REQUIRED)
endif(UNIX)

set(SOURCES
            src/aio_asio.cpp
            src/aio_jack.cpp
            src/osx/aio_asioosx.cpp
            src/windows/aio_asiowin32.cpp

            include/aio_jack.h
            include/aio_audio.h
            )


if(UNIX)
    set(SOURCES
            ${SOURCES}
            src/linux/aio_alsa.cpp
            include/linux/aio_alsa.h
            )
endif()

include_directories("include")

add_library(audiocard_io STATIC ${SOURCES})

add_subdirectory(demo)
add_subdirectory(tests)
