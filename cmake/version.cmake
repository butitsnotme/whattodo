if (DEFINED $ENV{CI_BUILD_REF_NAME})
  string(REPLACE "refs/heads/" "" branch $ENV{CI_BUILD_REF_NAME})
else()
  execute_process(
      COMMAND ${GIT_EXECUTABLE} symbolic-ref HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE res
      OUTPUT_VARIABLE ref
      ERROR_QUIET)

  if (0 EQUAL ${res})
    string(REPLACE "refs/heads/" "" branch ${ref})
  else()
    set(branch "unkown-branch")
  endif()
endif()

set(GIT_BRANCH ${branch} CACHE STRING "Git branch")

if (DEFINED $ENV{CI_BUILD_REF})
  set(GIT_REVISION $ENV{CI_BUILD_REF} CACHE STRING "Git revision")
else()
  execute_process (
      COMMAND ${GIT_EXECUTABLE} rev-list --all --max-count=1
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE res
      OUTPUT_VARIABLE out
      ERROR_QUIET)

  if (0 EQUAL ${res})
    set (GIT_REVISION ${out} CACHE STRING "Git revision")
  else ()
    set (GIT_REVISION "unknown revision" CACHE STRING "Git revision")
  endif ()
endif ()

if (DEFINED $ENV{CI_RUNNER_DESCRIPTION})
  set(BUILDER "GitLab CI ($ENV{GITLAB_USER_EMAIL}:$ENV{CI_RUNNER_DESCRIPTION}"
      CACHE STRING "Who built the application")
else()
  if(${WIN32})
    execute_process(
        COMMAND hostname
        OUTPUT_VARIABLE host
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(BUILDER "$ENV{USERNAME}@${host}" CACHE STRING
        "Who built the application")
  else()
    execute_process(
        COMMAND hostname
        OUTPUT_VARIABLE host
        ERROR_QUIET)
    set(BUILDER "$ENV{USER}@${host}" CACHE STRING "who built the application")
  endif()
endif()

string(TIMESTAMP DATE "%Y-%m-%d %H:%M")

if (${branch} MATCHES "^release-*|^releases$")
  set(RELEASE_BUILD OFF CACHE BOOL "Build project in release mode")
else()
  set(RELEASE_BUILD OFF CACHE BOOL "Build project in release mode")
endif()

if (${RELEASE_BUILD})
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "CMake build type" FORCE)
else()
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "CMake build type" FORCE)
endif()
set(WHATTODO_VERSION
    "${WHATTODO_VERSION_MAJOR}.${WHATTODO_VERSION_MINOR}.${WHATTODO_VERSION_PATCH}"
    CACHE STRING "Project Version" FORCE)
