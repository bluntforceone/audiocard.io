cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (APPLE OR WIN32)
    set(ASIO TRUE)
endif()

if (APPLE)
    set(COREAUDIO TRUE)
endif()

if (LINUX)
    set(ALSA TRUE)
    set(JACK TRUE)
    set(PULSE TRUE)
endif()

if (ALSA)
find_package(ALSA REQUIRED)
endif(ALSA)

if(COREAUDIO)
    set(SOURCES
            ${SOURCES}

            src/osx/aio_coreaudio.cpp
            include/osx/aio_coreaudio.cpp
            )
endif()

if(ALSA)
    set(SOURCES
            ${SOURCES}

            src/linux/aio_alsa.cpp
            include/linux/aio_alsa.h
        )
endif()


if(PULSE)
    set(SOURCES
            ${SOURCES}

            src/linux/aio_pulse.cpp
            include/linux/aio_pulse.h
            )
endif()

if(JACK)
    set(SOURCES
            src/aio_jack.cpp
            include/aio_jack.h
            )
endif()


if(ASIO)
    include_directories(3rd_party/ASIOSDK2.3/host)
    include_directories(3rd_party/ASIOSDK2.3/common)
    set(SOURCES
            ${SOURCES}

            3rd_party/ASIOSDK2.3/host/asiodrivers.h
            3rd_party/ASIOSDK2.3/host/asiodrivers.cpp

            src/aio_asio.cpp
            include/aio_asio.h
            )
    if(WIN32)
        include_directories(3rd_party/ASIOSDK2.3/host/pc)
        set(SOURCES
                ${SOURCES}

                3rd_party/ASIOSDK2.3/host/pc/asiolist.cpp
                3rd_party/ASIOSDK2.3/host/pc/asiolist.h

                src/windows/aio_asiowin32.cpp
                )
    endif()
    if(APPLE)
        include_directories(3rd_party/ASIOSDK2.3/host/mac)
        set(SOURCES
                ${SOURCES}

                3rd_party/ASIOSDK2.3/host/mac/asiolist.cpp
                3rd_party/ASIOSDK2.3/host/mac/asiolist.h

                src/osx/aio_asioosx.cpp
                include/osx/aio_asioosx.cpp
                )
    endif()
endif()


include_directories("include")

add_library(audiocard_io STATIC ${SOURCES})

add_subdirectory(demo)
add_subdirectory(tests)
