include_directories(
        "$<BUILD_INTERFACE:${BGL_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${BGL_INSTALL_INCLUDEDIR}>"
        "${BGL_SOURCE_DIR}/deps/cglm/include"
)

link_libraries(
        bgl
        m
)

add_executable(cube cube.c)
add_executable(gears gears.c)
add_executable(obj obj.c)
add_executable(scene scene.c)


if (BGL_INSTALL)
    install(TARGETS
            cube
            gears
            scene
#            EXPORT bgl_targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
#            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bgl"
    )
endif()
