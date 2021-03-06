cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ---- Project ----

project(
  InplaceOptional
  VERSION 1.0
  LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.3
)

# ---- Add source files ----

FILE(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# ---- Create library ----

add_library(InplaceOptionalHeaders EXCLUDE_FROM_ALL ${headers})
set_target_properties(InplaceOptional_headers PROPERTIES LINKER_LANGUAGE CXX)

add_library(InplaceOptional INTERFACE)
set_target_properties(InplaceOptional PROPERTIES INTERFACE_COMPILE_FEATURES cxx_std_17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(Greeter PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

target_include_directories(
  InplaceOptional INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
)
