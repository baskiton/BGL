include_directories(
        "$<BUILD_INTERFACE:${BGL_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${BGL_INSTALL_INCLUDEDIR}>"
        "${BGL_SOURCE_DIR}/deps/cglm/include"
)

link_libraries(bgl cglm)

if (MATH_LIBRARY)
    link_libraries("${MATH_LIBRARY}")
endif()

add_executable(cube cube.c)
add_executable(gears gears.c)
add_executable(obj obj.c)
add_executable(scene scene.c)
add_executable(window window.c)

set(BINARIES cube gears scene window)
set_target_properties(${BINARIES} PROPERTIES
        C_STANDARD 11
        FOLDER "bgl/examples"
)

if (MSVC)
    set_target_properties(${BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif(CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    set_target_properties(${BINARIES} PROPERTIES LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()

if (BGL_INSTALL)
    install(TARGETS ${BINARIES}
            EXPORT bgl_targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
#            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bgl"
    )
endif()
