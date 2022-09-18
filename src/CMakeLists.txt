find_package(Threads REQUIRED)

set(BGL_LIBRARY_TYPE "${BGL_LIBRARY_TYPE}" CACHE STRING
        "Library type override for BGL (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

if (BGL_LIBRARY_TYPE)
    if (BGL_LIBRARY_TYPE STREQUAL "SHARED")
        set(BGL_BUILD_SHARED_LIBRARY TRUE)
    else()
        set(BGL_BUILD_SHARED_LIBRARY FALSE)
    endif()
else()
    set(BGL_BUILD_SHARED_LIBRARY ${BUILD_SHARED_LIBS})
endif()

add_library(bgl SHARED
        bgl.c
        callbacks.c
        time.c
        window.c
        tools/open_obj.c
        pipeline/pipeline.c
        pipeline/vertex_buffer.c
        pipeline/index_buffer.c
        pipeline/viewport.c
        pipeline/uniform.c
)
#add_subdirectory()


###############################################################################

include(CMakeDependentOption)

cmake_dependent_option(BGL_BUILD_WIN32 "Build support for Win32" ON "WIN32" OFF)
cmake_dependent_option(BGL_BUILD_COCOA "Build support for Cocoa" ON "APPLE" OFF)
cmake_dependent_option(BGL_BUILD_X11 "Build support for X11" ON "UNIX;NOT APPLE" OFF)
cmake_dependent_option(BGL_BUILD_WAYLAND "Build support for Wayland" "${BGL_USE_WAYLAND}" "UNIX;NOT APPLE" OFF)

if (BGL_BUILD_WIN32)
    message(STATUS "Including Win32 support")

    target_compile_definitions(bgl PRIVATE _BGL_WIN32)
    target_sources(bgl PRIVATE
            win32/init.c
            win32/platform.c
            win32/window.c
            win32/render/base.c
    )
    list(APPEND bgl_PKG_LIBS "-lgdi32")
elseif (BGL_BUILD_COCOA)
    message(FATAL_ERROR "Cocoa is not supported yet")
#    message(STATUS "Including Cocoa support")
#    target_compile_definitions(bgl PRIVATE _BGL_COCOA)
elseif (BGL_BUILD_WAYLAND)
    message(FATAL_ERROR "Wayland is not supported yet")
#    message(STATUS "Including Wayland support")
#
#    include(CheckIncludeFiles)
#    include(CheckFunctionExists)
#    check_function_exists(memfd_create HAVE_MEMFD_CREATE)
#    find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
#
#    include(FindPkgConfig)
#    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.15)
#    pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)
#    pkg_get_variable(WAYLAND_CLIENT_PKGDATADIR wayland-client pkgdatadir)
#    pkg_check_modules(Wayland REQUIRED
#            wayland-client>=0.2.7
#            wayland-cursor>=0.2.7
#            wayland-egl>=0.2.7
#            xkbcommon>=0.5.0
#    )
#    target_include_directories(bgl PRIVATE ${Wayland_INCLUDE_DIRS})
#
#    target_compile_definitions(bgl PRIVATE _BGL_WAYLAND)
#    target_include_directories(bgl PRIVATE "${Wayland_INCLUDE_DIRS}")
elseif (BGL_BUILD_X11)
    message(STATUS "Including X11 support")

    find_package(X11 REQUIRED)
#    find_package(X11 COMPONENTS Xrender REQUIRED)

    target_compile_definitions(bgl PRIVATE _BGL_X11)
    target_include_directories(bgl PRIVATE "${X11_X11_INCLUDE_PATH}")
    target_sources(bgl PRIVATE
#            x11/context.c
            x11/init.c
            x11/window.c
            x11/render/base.c
    )
    target_link_libraries(bgl PRIVATE
            ${X11_LIBRARIES}
#            ${X11_Xrender_LIB}
    )
endif()

if(BGL_BUILD_SHARED_LIBRARY)
    if (BGL_BUILD_WIN32 AND MINGW)
        # Remove the dependency on the version of libgcc
        # NOTE: MinGW-w64 has the correct default but MinGW need this
        target_link_libraries(bgl PRIVATE "-static-libgcc")
    endif()

    if (UNIX)
        # Hide symbols not explicitly tagged for export from the shared library
        target_compile_options(bgl PRIVATE "-fvisibility=hidden")
    endif()
endif()

if (APPLE)
    target_sources(bgl PRIVATE
            cocoa/time.c
#            posix/thread.c
    )
elseif (WIN32)
    target_sources(bgl PRIVATE
            win32/time.c
#            win32/thread.c
    )
else()  # UNIX
    target_sources(bgl PRIVATE
            posix/time.c
#            posix/thread.c
    )
endif()

if (UNIX AND NOT APPLE)
    find_library(RT_LIBRARY rt)
    mark_as_advanced(RT_LIBRARY)
    if (RT_LIBRARY)
        target_link_libraries(bgl PRIVATE "${RT_LIBRARY}")
        list(APPEND bgl_PKG_LIBS "-lrt")
    endif()

    find_library(MATH_LIBRARY m)
    mark_as_advanced(MATH_LIBRARY)
    if (MATH_LIBRARY)
        target_link_libraries(bgl PRIVATE "${MATH_LIBRARY}")
        list(APPEND bgl_PKG_LIBS "-lm")
    endif()

    if (CMAKE_DL_LIBRARY)
        target_link_libraries(bgl PRIVATE "${CMAKE_DL_LIBRARY}")
        list(APPEND bgl_PKG_LIBS "-l${CMAKE_DL_LIBRARY}")
    endif()
endif()

###############################################################################


target_include_directories(bgl
        PUBLIC
            "$<BUILD_INTERFACE:${BGL_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${BGL_INSTALL_INCLUDEDIR}>"
        PRIVATE
            "${BGL_SOURCE_DIR}/src"
            "${BGL_SOURCE_DIR}/deps/cglm/include"
)

target_link_libraries(bgl PRIVATE
        Threads::Threads
        cglm
)

set_property(TARGET bgl
        APPEND
        PROPERTY PUBLIC_HEADER
            ${BGL_SOURCE_DIR}/include/bgl/bgl.h
            ${BGL_SOURCE_DIR}/include/bgl/bglm.h
)

set_target_properties(bgl PROPERTIES
        OUTPUT_NAME bgl
        VERSION ${BGL_VERSION}
        SOVERSION ${BGL_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
        C_STANDARD 11
        C_EXTENSIONS ON
        DEFINE_SYMBOL _BGL_BUILD_DLL
        FOLDER "bgl"
)

foreach(arg ${bgl_PKG_DEPS})
    string(APPEND deps " ${arg}")
endforeach()
foreach(arg ${bgl_PKG_LIBS})
    string(APPEND libs " ${arg}")
endforeach()

set(BGL_PKG_CONFIG_REQUIRES_PRIVATE "${deps}" CACHE INTERNAL "BGL pkg-config Requires.private")
set(BGL_PKG_CONFIG_LIBS_PRIVATE "${libs}" CACHE INTERNAL "BGL pkg-config Libs.private")

if (BGL_INSTALL)
    install(TARGETS bgl
            EXPORT bgl_targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bgl"
    )
endif()
