cmake_minimum_required(VERSION 3.16)
project(CarGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CarGame CarGame.cpp)

# On MSVC, set the subsystem to console (default, but explicit is better)
if(MSVC)
    target_compile_options(CarGame PRIVATE /W4)
else()
    target_compile_options(CarGame PRIVATE -Wall -Wextra)
endif()

# Install target (optional: cmake --install .)
install(TARGETS CarGame RUNTIME DESTINATION bin)
