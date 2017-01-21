project(whattodo C CXX ASM)
cmake_minimum_required(VERSION 3.0)

# Stop CMake from building the program on install
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

if (NOT "${CMAKE_CXX_FLAGS}" MATCHES "-std=c\\+\\+14")
  get_property(CXX_FLAGS_DESCRIPTION CACHE "CMAKE_CXX_FLAGS" PROPERTY HELPSTRING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" CACHE STRING
      "${CXX_FLAGS_DESCRIPTION}" FORCE)
endif()

set(WHATTODO_VERSION_MAJOR 0 CACHE STRING "WhatTODO Major Version" FORCE)
set(WHATTODO_VERSION_MINOR 1 CACHE STRING "WhatTODO Minor Version" FORCE)
set(WHATTODO_VERSION_PATCH 1 CACHE STRING "WhatTODO Patch Level" FORCE)

set(WHATTODO_DATA_DIRECTORY "/var/lib/whattodo" CACHE STRING
    "Directory where whattodo will store its data")

find_program(GIT git)
find_program(TMPLCC cppcms_tmpl_cc)
find_program(SQLITE3 sqlite3)

# Debian package properties
set(CPACK_PACKAGE_VERSION_MAJOR ${WHATTODO_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${WHATTODO_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${WHATTODO_VERSION_PATCH})
set(CMAKE_INSTALL_PREFIX "/usr")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "whattodo")
set(CPACK_DEBIAN_PACKAGE_VERSION ${WHATTODO_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "butitsnotme")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "A small web-based todo manager written in C++.")
set(CPACK_DEBIAN_PACKAGE_SECTION "web")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/butitsnotme/whattodo")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "dbconfig-common, systemd, sqlite3")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/conffiles;${CMAKE_CURRENT_SOURCE_DIR}/debian/config;${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/preinst")


# Extra dependencies
include(cmake/version.cmake)
include(CPack)
include(FeatureSummary)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE)
endif()

set(WHATTODO_TEMPLATES
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/master.tmpl
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/login.tmpl
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/main.tmpl
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/index.tmpl
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/new.tmpl
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/todo.tmpl
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/default_view.cpp
    COMMAND ${TMPLCC}
            -d whattodo
            -o ${CMAKE_CURRENT_BINARY_DIR}/default_view.cpp
            ${WHATTODO_TEMPLATES}
    DEPENDS ${WHATTODO_TEMPLATES})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

set(WHATTODO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo/whattodo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo/todo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo/user.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/default_view.cpp
)

set(WHATTODOCTL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo/user.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/whattodoctl/main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/data.S
)

set(WHATTODO_DATA_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/data/VERSION
    ${CMAKE_CURRENT_SOURCE_DIR}/data/VERSION_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/data/VERSION_TEMPLATE_DEV
    ${CMAKE_CURRENT_SOURCE_DIR}/data/LICENSE
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/VERSION
    ${CMAKE_CURRENT_BINARY_DIR}/data/VERSION)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data.S
    ${CMAKE_CURRENT_BINARY_DIR}/src/data.S)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.js
    ${CMAKE_CURRENT_BINARY_DIR}/config.js)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/whattodo.service
    ${CMAKE_CURRENT_BINARY_DIR}/whattodo.service)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodoctl/main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/whattodoctl/main.cpp)

set_source_files_properties(${PROJECT_BINARY_DIR}/src/data.S
    PROPERTIES OBJECT_DEPENDS "${WHATTODO_DATA_FILES}")

find_library(CPPCMS NAME cppcms PATHS /usr/lib/)
find_library(CPPDB NAME cppdb PATHS /usr/lib/)
find_library(BOOSTER NAME booster PATHS /usr/lib/)
find_library(SODIUM NAME libsodium.a PATHS /usr/lib/)

add_executable(whattodo ${WHATTODO_SOURCES})
target_link_libraries(whattodo ${CPPDB} ${CPPCMS} ${BOOSTER} ${SODIUM})

add_executable(whattodoctl ${WHATTODOCTL_SOURCES})
target_link_libraries(whattodoctl ${CPPDB} ${SODIUM})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/debian DESTINATION /)

install (TARGETS whattodo whattodoctl DESTINATION bin)
install (FILES ${PROJECT_BINARY_DIR}/config.js DESTINATION /etc/whattodo)
install (FILES ${PROJECT_SOURCE_DIR}/sql/sqlite3.sql DESTINATION share/dbconfig-common/data/whattodo/install RENAME sqlite3)
install (FILES ${PROJECT_BINARY_DIR}/whattodo.service DESTINATION /etc/systemd/system)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
