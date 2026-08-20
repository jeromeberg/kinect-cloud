add_executable(kc-record main.cpp)

target_link_libraries(kc-record PRIVATE kc)
target_compile_options(kc-record PRIVATE -Wall -Wextra -Werror)
