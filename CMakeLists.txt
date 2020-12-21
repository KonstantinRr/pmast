cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(pmast)

# CMake Version control
set(CMAKE_PROJECT_VERSION 1.0.0)
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

# Options
option(OPTIMIZE_SIZE "Optimizes for size" OFF)
option(OPTIMIZE_SPEED "Optimizes for speed" OFF)

# The project and dependencies rely on the C11 standard.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# The project and dependencies rely on the CXX17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

######## PMAST SOURCES ########
set(PMAST_SOURCES # All source files in the PMAST project
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/main.cc")
set(PMAST_HEADERS # All header files in the PMAST project
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/pmast.h")
set(PMAST_INCLUDE  # All includes in the PMAST projects
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

######## ENGINE SOURCES ########
set(ENGINE_SOURCES # All source files in the ENGINE project
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/entity.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/matrix.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shader.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/gl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/glx.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/upng/src/upng.c")
set(ENGINE_HEADERS # All header files in the ENGINE project
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/engine.h")
set(ENGINE_INCLUDE # All include files in the ENGINE project
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/upng/include")
######## ENVIRONMENT SOURCES ########
set(ENV_SOURCES # All source files in the ENV project
  "${CMAKE_CURRENT_SOURCE_DIR}/src/env/env_x.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/env/env_w.c")
set(ENV_INCLUDES # All include files in the ENV project
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Prefer libc++ in conjunction with Clang, not on Windows
if (NOT MSVC)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
    CHECK_CXX_COMPILER_AND_LINKER_FLAGS_BASE(HAS_LIBCPP "-stdlib=libc++" "-stdlib=libc++")
    if (HAS_LIBCPP)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -D_LIBCPP_VERSION")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
      message(STATUS "PMAST: using libc++.")
    else()
    CHECK_CXX_COMPILER_AND_LINKER_FLAGS_BASE(HAS_LIBCPP_AND_CPPABI "-stdlib=libc++" "-stdlib=libc++ -lc++abi")
      if (HAS_LIBCPP_AND_CPPABI)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -D_LIBCPP_VERSION")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        message(STATUS "Nyrem: using libc++ and libc++abi.")
      else()
        message(FATAL_ERROR "When Clang is used to compile Nyrem, libc++ must be available -- GCC's libstdc++ is not supported! (please insteall the libc++ development headers, provided e.g. by the packages 'libc++-dev' and 'libc++abi-dev' on Debian/Ubuntu).")
      endif()
    endif()
  endif()
endif()

if (MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  # Finds OpenGL and X11
  find_package(OpenGL REQUIRED)
  find_package(X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

  # lots of warnings and all warnings as errors
  add_compile_options(-Wall -Wextra -pedantic)
  # Checks whether we should optimize for size
  if (OPTIMIZE_SIZE)
    add_compile_options(-Os -ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
  endif()
  # checks whether we should optimize for speed
  if (OPTIMIZE_SPEED)
    add_compile_options(-O3)
  endif()
endif()

# Copies the assets
add_custom_target(copy-files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/assets
  ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# Builds the library
add_library(engine ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_link_libraries(engine PUBLIC m ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(engine PUBLIC ${ENGINE_INCLUDE} ${X11_INCLUDE_DIR})

add_library(pmast ${PMAST_SOURCES} ${PMAST_HEADERS})
target_link_libraries(pmast PUBLIC engine)

add_executable(env ${ENV_SOURCES})
add_dependencies(env copy-files)
target_link_libraries(env PUBLIC pmast engine)
