cmake_minimum_required(VERSION 3.16)

project(Hanoi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(LIB_DIR ${CMAKE_SOURCE_DIR}/libs)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "src/*.cpp")
message(STATUS "Source files: ${SRCS}")

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(Freetype REQUIRED)
find_package(assimp QUIET)

add_executable(main ${SRCS} ${LIB_DIR}/glad/src/glad.c)

target_include_directories(main PRIVATE
    src
    ${LIB_DIR}/glad/include
    ${LIB_DIR}/glm
    ${GLFW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(main PRIVATE
    OpenGL::GL
    ${GLFW_LIBRARIES}
    Freetype::Freetype
    ${CMAKE_DL_LIBS}
)

if(assimp_FOUND)
    target_link_libraries(main PRIVATE assimp::assimp)
endif()

# irrKlang support (disabled - commented out)
# set(IRRKLANG_ROOT ${CMAKE_SOURCE_DIR}/libs/irrKlang-64bit-1.6.0 CACHE PATH "Path to irrKlang root")
# find_path(IRRKLANG_INCLUDE_DIR irrKlang.h
#     PATHS ${IRRKLANG_ROOT}/include /usr/include /usr/local/include
# )
# find_library(IRRKLANG_LIBRARY NAMES IrrKlang irrKlang
#     PATHS ${IRRKLANG_ROOT}/bin/linux-64 ${IRRKLANG_ROOT}/lib/linux-64 /usr/lib /usr/local/lib
# )
# 
# if(IRRKLANG_INCLUDE_DIR AND IRRKLANG_LIBRARY)
#     target_include_directories(main PRIVATE ${IRRKLANG_INCLUDE_DIR})
#     target_link_libraries(main PRIVATE ${IRRKLANG_LIBRARY})
# else()
#     message(WARNING "IrrKlang not found. Download the Linux irrKlang SDK and set IRRKLANG_ROOT or install it system-wide.")
# endif()

# 拷贝资源文件到运行目录
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
    COMMENT "Copying resources to build directory"
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying shaders to build directory"
)