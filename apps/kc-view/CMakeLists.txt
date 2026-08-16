pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
find_package(OpenGL REQUIRED)

add_executable(kc-view
    main.cpp
    camera.cpp)

target_link_libraries(kc-view PRIVATE kc PkgConfig::GLFW OpenGL::GL)
target_compile_definitions(kc-view PRIVATE GL_SILENCE_DEPRECATION)
target_compile_options(kc-view PRIVATE -Wall -Wextra)
