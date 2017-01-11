project(whattodo C CXX ASM)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Extra dependencies
include(CPack)
include(FeatureSummary)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE)
endif()

find_program(TMPLCC cppcms_tmpl_cc)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/approxidate)

set(WHATTODO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/whattodo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/todo.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/default_view.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/approxidate/approxidate.c
)

add_executable(whattodo ${WHATTODO_SOURCES})
target_link_libraries(whattodo cppcms booster cppdb)

file(COPY ${PROJECT_SOURCE_DIR}/config.js
    DESTINATION ${PROJECT_BINARY_DIR})

install (TARGETS whattodo DESTINATION bin)
install (FILES ${PROJECT_BINARY_DIR}/config.js DESTINATION share/whattodo)
