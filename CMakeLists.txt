project(whattodo C CXX ASM)
cmake_minimum_required(VERSION 3.0)

# Extra dependencies
include(CPack)
include(FeatureSummary)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE)
endif()

include_directories(${PROJECT_SOURCE_DIR}/inc)

list(APPEND WHATTODO_SOURCES ${PROJECT_SOURCE_DIR}/src/whattodo.cpp)
list(APPEND WHATTODO_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(whattodo ${WHATTODO_SOURCES})
target_link_libraries(whattodo cppcms booster cppdb)

file(COPY ${PROJECT_SOURCE_DIR}/config.js
    DESTINATION ${PROJECT_BINARY_DIR})

install (TARGETS whattodo DESTINATION bin)
install (FILES ${PROJECT_BINARY_DIR}/config.js DESTINATION share/whattodo)
