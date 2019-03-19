cmake_minimum_required(VERSION 3.13)

set(VERSION 0.0.1)

string(REGEX REPLACE "-.*" "" FRIENDLY_VERSION ${VERSION})

project(oxidising-ocelots-backend VERSION "${FRIENDLY_VERSION}")

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)

# Warnings
add_compile_options("-Wall" "-Wextra")
# Security
add_compile_options("-D_FORTIFY_SOURCE=2")
add_compile_options("-fstack-protector-strong")
# Signed o/f protection
add_compile_options("-ftrapv")

find_package(c3-nu REQUIRED)

# Makes a bunch of things r/o so it is harder to exploit
#add_link_options("-Wl,-z,relro,-z,now")

include_directories(include)

file(GLOB_RECURSE source src/*.cpp)

add_executable(${PROJECT_NAME} ${source})

target_link_libraries(${PROJECT_NAME} ${BOTAN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} c3-nu)
