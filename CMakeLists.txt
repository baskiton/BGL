cmake_minimum_required(VERSION 3.8.2)
project(BGL VERSION 0.0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

option(BGL_SHARED_LIBS "Build shared libraries" ON)
option(BGL_INSTALL "Generate installation target" ON)
option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)

include(GNUInstallDirs)

set(BGL_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bgl")

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(deps/cglm)

configure_file("${CMAKE_CURRENT_LIST_DIR}/bgl.pc.in" bgl.pc @ONLY)

if (BGL_INSTALL)
    install(EXPORT bgl_targets
            FILE bglTargets.cmake
            EXPORT_LINK_INTERFACE_LIBRARIES
            DESTINATION "${BGL_CONFIG_PATH}"
    )
    install(FILES "${BGL_BINARY_DIR}/bgl.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    )
endif()
