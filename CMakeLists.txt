cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32)
    set(BUILD_ASIO TRUE)
    set(BUILD_WASAPI TRUE)
endif()

if (APPLE)
    set(BUILD_COREAUDIO TRUE)
endif()

if (LINUX)
    set(BUILD_ALSA TRUE)
    set(BUILD_JACK TRUE)
    set(BUILD_PULSE TRUE)
endif()

if (BUILD_ALSA)
find_package(ALSA REQUIRED)
endif(BUILD_ALSA)

if (BUILD_JACK)
    find_library(JACK_LIBRARY NAMES jack)
endif(BUILD_JACK)


set(SOURCES include/aio_audio.h)


if(BUILD_COREAUDIO)
    set(SOURCES
            ${SOURCES}

            src/osx/aio_coreaudio.cpp
            include/osx/aio_coreaudio.h

            include/osx/corefoundation/cf_string.h
            )
    find_library(COREAUDIO_LIBRARY NAMES CoreAudio)
    find_library(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
    find_library(AUDIOUNIT_LIBRARY NAMES AudioUnit)
endif()

if(BUILD_ALSA)
    set(SOURCES
            ${SOURCES}

            src/linux/aio_alsa.cpp
            include/linux/aio_alsa.h
        )
endif()


if(BUILD_PULSE)
    set(SOURCES
            ${SOURCES}

            src/linux/aio_pulse.cpp
            include/linux/aio_pulse.h
            )
endif()

if(BUILD_JACK)
    set(SOURCES
            ${SOURCES}

            src/aio_jack.cpp
            include/aio_jack.h
            )
endif()


if(BUILD_ASIO)
    include_directories(3rd_party/ASIOSDK2.3/host)
    include_directories(3rd_party/ASIOSDK2.3/common)
    set(SOURCES
            ${SOURCES}

            3rd_party/ASIOSDK2.3/common/asio.cpp
            3rd_party/ASIOSDK2.3/common/asio.h


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

                )
    endif()
endif()

if(BUILD_WASAPI)
    if(WIN32)
        set(SOURCES
                ${SOURCES}

                include/windows/com/aio_coinit.h
                include/windows/com/aio_com.h
                include/windows/com/aio_propvariant.h

                include/windows/aio_waveformatex.h
                include/windows/aio_wasapi.h
                include/windows/aio_string.h
                src/windows/aio_wasapi.cpp

                )
    endif()
endif()


include_directories("include")

add_library(audiocard_io STATIC ${SOURCES})

add_subdirectory(demo)
add_subdirectory(tests)
