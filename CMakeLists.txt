cmake_minimum_required(VERSION 3.18)

if(NOT DEFINED SKBUILD_PROJECT_VERSION)
  set(SKBUILD_PROJECT_VERSION 0.1.0)
endif()

project(simdjson_test_wrapper VERSION ${SKBUILD_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PYTHON_MODULE_NAME simdjson_test_wrapper)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(SIMDJSON_VERSION 4.6.2)
set(SIMDJSON_DIR "${CMAKE_BINARY_DIR}/_deps/simdjson-${SIMDJSON_VERSION}")
set(SIMDJSON_HEADER "${SIMDJSON_DIR}/simdjson.h")
set(SIMDJSON_SOURCE "${SIMDJSON_DIR}/simdjson.cpp")

file(MAKE_DIRECTORY "${SIMDJSON_DIR}")

function(download_simdjson_file url path)
  set(should_download TRUE)

  if(EXISTS "${path}")
    file(SIZE "${path}" existing_size)
    if(existing_size GREATER 0)
      set(should_download FALSE)
    endif()
  endif()

  if(should_download)
    file(DOWNLOAD
      "${url}"
      "${path}"
      STATUS download_status
      LOG download_log
      SHOW_PROGRESS
      TLS_VERIFY ON)

    list(GET download_status 0 status_code)
    list(GET download_status 1 status_message)

    if(NOT status_code EQUAL 0)
      file(REMOVE "${path}")
      message(FATAL_ERROR
        "Failed to download ${url}: ${status_message}\n${download_log}")
    endif()
  endif()
endfunction()

download_simdjson_file(
  "https://github.com/simdjson/simdjson/releases/download/v${SIMDJSON_VERSION}/simdjson.h"
  "${SIMDJSON_HEADER}")

download_simdjson_file(
  "https://github.com/simdjson/simdjson/releases/download/v${SIMDJSON_VERSION}/simdjson.cpp"
  "${SIMDJSON_SOURCE}")

pybind11_add_module(${PYTHON_MODULE_NAME} src/simdjson_test_wrapper.cpp "${SIMDJSON_SOURCE}")

target_include_directories(${PYTHON_MODULE_NAME} PRIVATE "${SIMDJSON_DIR}")
target_compile_features(${PYTHON_MODULE_NAME} PRIVATE cxx_std_17)

install(TARGETS ${PYTHON_MODULE_NAME}
  LIBRARY DESTINATION .
  RUNTIME DESTINATION .)
