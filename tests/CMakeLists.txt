cmake_minimum_required(VERSION 3.10)
project(audiocard_io)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        tst_main.cpp fakeit.hpp)

if(UNIX)
    set(SOURCES
            ${SOURCES}
            tst_alsa.cpp
             )
endif(UNIX)

add_executable(audiocard_io_test ${SOURCES})
target_link_libraries(audiocard_io_test audiocard_io)