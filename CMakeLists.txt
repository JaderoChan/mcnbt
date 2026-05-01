cmake_minimum_required(VERSION 3.17)

project(mcnbt VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MSG_PREFIX "MC NBT")

include(GNUInstallDirs)

# Check whether the project is the root project.
if(NOT MCNBT_IS_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(MCNBT_IS_MASTER_PROJECT ON)
    else()
        set(MCNBT_IS_MASTER_PROJECT OFF)
    endif()
endif()

###########
# Options #
###########

option(MCNBT_ENABLE_GZIP "Enable GZip to support compress/decompress NBT data." ON)
option(MCNBT_USE_INBUILD_ZLIB "Use the inbuild zlib libarary to build with GZip" ON)
option(MCNBT_DISABLE_EXCEPTION "Disable exception throw for better performance" OFF)

########################
# Collect source files #
########################

# Configure config header.
set(CONFIG_HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/include/mcnbt/config.hpp)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/config.hpp.in
    ${CONFIG_HEADER_PATH}
    @ONLY
)

# Headers
set(GZIP_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/gzip.hpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/*.hpp)
if(NOT MCNBT_ENABLE_GZIP)
    list(REMOVE_ITEM HEADERS ${GZIP_HEADER})
endif()
list(APPEND HEADERS ${CONFIG_HEADER_PATH})

############################
# Make header-only library #
############################

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

##########################
# Configure dependencies #
##########################

# Configure include directories.
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Configure third-party dependencies.
if(MCNBT_ENABLE_GZIP)
    if(MCNBT_USE_INBUILD_ZLIB)
        include(FetchContent)
        FetchContent_Declare(
            zlib
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib
        )
        FetchContent_MakeAvailable(zlib)
    else()
        find_package(zlib REQUIRED)
    endif()

    target_link_libraries(${PROJECT_NAME} INTERFACE zlib)
endif()

###########
# Install #
###########

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/be DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/mcnbt.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
if(MCNBT_ENABLE_GZIP)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/gzip.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
endif()
install(FILES ${CONFIG_HEADER_PATH} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/licenses/${PROJECT_NAME})

install(
    EXPORT ${PROJECT_NAME}
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

#################
# Build Example #
#################

option(MCNBT_BUILD_EXAMPLE "Build example" ${MCNBT_IS_MASTER_PROJECT})
if(MCNBT_BUILD_EXAMPLE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)
endif()
