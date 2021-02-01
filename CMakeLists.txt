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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#include(CheckCXXSourceRuns)

# This macro checks whether a specific CXX compiler and linker flags work.
macro(CHECK_CXX_COMPILER_AND_LINKER_FLAGS_BASE _RESULT _CXX_FLAGS _LINKER_FLAGS)
  set(CMAKE_REQUIRED_FLAGS ${_CXX_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_LINKER_FLAGS})
  set(CMAKE_REQUIRED_QUIET TRUE)
  set(CMAKE_REQUIRED_FLAGS "")
  set(CMAKE_REQUIRED_LIBRARIES "")
endmacro()

######## PMAST SOURCES ########
set(PMAST_SOURCES # All source files in the PMAST project
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/main.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/agent.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/internal.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/geom.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/osm_graph.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/osm_mesh.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/osm.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/parser.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/render.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/mapcanvas.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/mapworld.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmast/main.cc")
set(PMAST_HEADERS # All header files in the PMAST project
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/pmast.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/agent.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/internal.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/geom.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/osm_graph.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/osm_mesh.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/osm.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/parser.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/mapcanvas.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/mapworld.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pmast/render.hpp")
set(PMAST_INCLUDE  # All includes in the PMAST projects
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glm"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/json"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/rapidxml"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/robin_hood")
######## ENGINE SOURCES ########
set(ENGINE_SOURCES # All source files in the ENGINE project
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/camera.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/entity.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/glmodel.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/resource.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shader.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shader.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/graphics.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/window.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/thread.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/internal.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/util.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/glad.c")
set(ENGINE_HEADERS # All header files in the ENGINE project
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/camera.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/engine.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/entity.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/glmodel.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/internal.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/resource.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/shader.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/graphics.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/window.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/thread.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/util.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/listener.hpp")

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

# Builds the library
add_library(engine ${ENGINE_SOURCES} ${ENGINE_HEADERS})
add_executable(pmast ${PMAST_SOURCES} ${PMAST_HEADERS})

if (MSVC)
  # warning level 4 and all warnings as errors
  #add_compile_options(/W4 /WX)
else()
  # Finds OpenGL and X11
  find_package(OpenGL REQUIRED)
  find_package(X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

  # lots of warnings and all warnings as errors
  # add_compile_options(-Wall -Wextra)
  # Checks whether we should optimize for size
  if (OPTIMIZE_SIZE)
    add_compile_options(-Os -ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
  endif()
  # checks whether we should optimize for speed
  if (OPTIMIZE_SPEED)
    add_compile_options(-O3)
  endif()

  target_link_libraries(engine PUBLIC m ${X11_LIBRARIES})
  target_include_directories(engine PUBLIC ${X11_INCLUDE_DIR})
endif()

# Copies the assets
add_custom_target(copy-files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/assets
  ${CMAKE_CURRENT_BINARY_DIR}/assets
)

set(SPDLOG_FMT_EXTERNAL ON) # Specifies that FMT is linked external   
add_subdirectory("ext/fmt")
add_subdirectory("ext/glfw")
add_subdirectory("ext/spdlog")


target_link_libraries(engine
  PUBLIC spdlog
  PRIVATE glfw
  PRIVATE fmt::fmt
  PRIVATE ${CMAKE_DL_LIBS})
target_include_directories(engine
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ext/glad"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/upng/include"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ext/glm"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/fmt/include"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ext/spdlog/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/earcut/include")

target_link_libraries(pmast PRIVATE engine)
target_include_directories(pmast PRIVATE ${PMAST_INCLUDE})
add_dependencies(pmast copy-files)