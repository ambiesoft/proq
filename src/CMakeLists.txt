cmake_minimum_required(VERSION 3.5)

project(proq LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(proq main.cpp
    ../../lsMisc/stdosd/stdosd.cpp ../../lsMisc/stdosd/stdosd.h ../../lsMisc/stdosd/stdosd_exception.h ../../lsMisc/stdosd/stdosd_literal.h)

if(WIN32)
    target_compile_definitions(proq PRIVATE UNICODE _UNICODE)
    target_sources(proq PRIVATE
        ../../lsMisc/stdosd/stdosd_win.cpp
    )
else()
    target_sources(proq PRIVATE
        ../../lsMisc/stdosd/stdosd_linux.cpp
    )
endif()

include(GNUInstallDirs)

if(WIN32)
    target_link_libraries(proq PRIVATE Kernel32.lib shlwapi.lib)
else()
endif()

install(TARGETS proq
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
