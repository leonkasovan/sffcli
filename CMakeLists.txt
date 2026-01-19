cmake_minimum_required(VERSION 3.10)
project(sffcli VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# Library sources
set(LIBSFF_SOURCES
    src/libsff/sff.cpp
)

set(LIBSFF_HEADERS
    src/libsff/sff.hpp
)

# Static library
add_library(sff STATIC ${LIBSFF_SOURCES} ${LIBSFF_HEADERS})
target_include_directories(sff PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)
target_link_libraries(sff PUBLIC
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

# Shared library
add_library(sff_shared SHARED ${LIBSFF_SOURCES} ${LIBSFF_HEADERS})
target_include_directories(sff_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)
target_link_libraries(sff_shared PUBLIC
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

# Set output names
set_target_properties(sff_shared PROPERTIES OUTPUT_NAME sff)

# CLI executable
add_executable(sffcli src/main.cpp)
target_include_directories(sffcli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(sffcli PRIVATE sff)

# Installation rules
install(TARGETS sff sff_shared sffcli
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${LIBSFF_HEADERS}
    DESTINATION include/libsff
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libsff.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libsff.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libsff.pc
    DESTINATION lib/pkgconfig
)
