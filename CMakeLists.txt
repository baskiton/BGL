cmake_minimum_required(VERSION 3.8.2)
project(BGL VERSION 0.0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

option(BGL_INSTALL "Generate installation target" ON)

include(GNUInstallDirs)

add_subdirectory(deps/cglm)
add_subdirectory(src)
add_subdirectory(example)

#if (BGL_INSTALL)
#    install(DIRECTORY include/bgl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#endif()
