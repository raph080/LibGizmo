cmake_minimum_required(VERSION 3.12)

project(libgizmo VERSION 0.0.1 DESCRIPTION "libgizmo")

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DGLEW_STATIC)

add_subdirectory(vendors)
add_subdirectory(app)


option(BUILD_SHARED_LIBS "Build shared library" ON)

# gather all sources in a variable
file(GLOB SRC_CPP
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "src/*.cpp"
)

# gather all private headers in a variable
file(GLOB SRC_H
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "src/*.h"
)

# gather all public headers in a variable
file(GLOB_RECURSE PUBLIC_HEADERS
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "include/*.h"
)

# condition to build shared or static
if( BUILD_SHARED_LIB )
    add_library(${PROJECT_NAME} SHARED ${SRC_CPP} ${SRC_H})
else()
    add_library(${PROJECT_NAME} STATIC ${SRC_CPP} ${SRC_H})
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# add glm lib in executable
target_link_libraries(${PROJECT_NAME}
    glm
    libglew_static
    glfw
)

# note that ${PUBLIC_HEADERS} has to be in quotes
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

# install the target and create export-set
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(
    TINY_RAY_TRACER_INSTALL_INCLUDEDIR
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
)