cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)


if(BUILD_ALSA)
    set(ALSA_SOURCES
            demo_probe.h
            probe_alsa.cpp
            )
    add_executable(demo_alsa ${ALSA_SOURCES})
    target_link_libraries(demo_alsa audiocard_io ${ALSA_LIBRARIES})
endif(BUILD_ALSA)

if(BUILD_ASIO)
    set(ASIO_SOURCES
            demo_probe.h
            probe_asio.cpp
            )
    add_executable(demo_asio ${ASIO_SOURCES})
    target_link_libraries(demo_asio audiocard_io ${ASIO_LIBRARIES})
endif(BUILD_ASIO)

if(BUILD_WASAPI)
    set(WASAPI_SOURCES
            demo_probe.h
            probe_wasapi.cpp
            )
    add_executable(demo_wasapi ${WASAPI_SOURCES})
    target_link_libraries(demo_wasapi audiocard_io)
endif(BUILD_WASAPI)


if (BUILD_PULSE)
    set(PULSE_SOURCES
            demo_probe.h
            probe_pulse.cpp
            )
    add_executable(demo_pulse ${PULSE_SOURCES})
    target_link_libraries(demo_pulse audiocard_io ${PULSE_LIBRARIES})
endif(BUILD_PULSE)

if (BUILD_COREAUDIO)
    set(COREAUDIO_SOURCES
            demo_probe.h
            probe_coreaudio.cpp
            )
    add_executable(demo_coreaudio ${COREAUDIO_SOURCES})

    target_link_libraries(demo_coreaudio audiocard_io ${AUDIOUNIT_LIBRARY} ${COREAUDIO_LIBRARY} ${COREFOUNDATION_LIBRARY})
endif(BUILD_COREAUDIO)

if (BUILD_JACK)
    set(JACK_SOURCES
            demo_probe.h
            probe_jack.cpp
            )
    add_executable(demo_jack ${JACK_SOURCES})

    target_link_libraries(demo_jack audiocard_io ${JACK_LIBRARY})
endif(BUILD_JACK)



