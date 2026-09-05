cmake_minimum_required(VERSION 3.17)

project(mcnbt VERSION 2.1.2 LANGUAGES CXX)
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

option(MCNBT_BUILD_ZLIB       "Build Zlib to support compress/decompress NBT data with GZip." ON)
option(MCNBT_USE_BUILTIN_ZLIB "Use built-in Zlib library to build."                           ON)
if(MCNBT_BUILD_ZLIB)
    set(MCNBT_HAS_ZLIB TRUE)
else()
    set(MCNBT_HAS_ZLIB FALSE)
endif()

########################
# Collect source files #
########################

set(EXTRA_HEADERS config.hpp)
if(MCNBT_BUILD_ZLIB)
    list(APPEND EXTRA_HEADERS gzip.hpp)
endif()
foreach(ITEM IN LISTS EXTRA_HEADERS)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/${ITEM}.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/mcnbt/${ITEM}
        @ONLY
    )
endforeach()

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
if(MCNBT_BUILD_ZLIB)
    if(MCNBT_USE_BUILTIN_ZLIB)
        include(FetchContent)
        FetchContent_Declare(
            zlib
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib
        )
        FetchContent_MakeAvailable(zlib)
    else()
        find_package(zlib REQUIRED)
    endif()

    target_link_libraries(${PROJECT_NAME} INTERFACE ZLIB::ZLIB)
endif()

###########
# Install #
###########

install(
    TARGETS ${PROJECT_NAME}
    EXPORT  ${PROJECT_NAME}
)

set(EXPORT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/mcnbt.hpp)
foreach(ITEM IN LISTS EXTRA_HEADERS)
    list(APPEND EXPORT_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include/mcnbt/${ITEM})
endforeach()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mcnbt/be DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
install(FILES     ${EXPORT_HEADERS}                            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcnbt)
install(FILES     LICENSE                                      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/licenses/${PROJECT_NAME})

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
