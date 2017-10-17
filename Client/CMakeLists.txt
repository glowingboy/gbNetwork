cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/")

find_package(libevent REQUIRED)
include_directories(SYSTEM ${libevent_INCLUDE_DIRS})

find_package(gbUtils REQUIRED)
include_directories(SYSTEM ${gbUtils_INCLUDE_DIRS})

find_package(lua REQUIRED)
include_directories(SYSTEM ${lua_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(gb_srcs)
set(gb_headers)

macro(gb_add_class clsName dir)
  set(src ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.cpp)
  set(header ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.h)

  if(EXISTS ${src})
    message(STATUS "src@" ${src} " added")
    set(gb_srcs ${gb_srcs} ${src})
  else()
    message("src@" ${src} " not found")
  endif()

  if(EXISTS ${header})
    message(STATUS "header@" ${header} " added")
    set(gb_headers ${gb_headers} ${header})
  else()
    message("header@" ${header} " not found")
  endif()
  
endmacro()

# gb_add_class(gbClientCore Core)
gb_add_class(gbNWClient .)
# gb_add_class(gbUDPData ..)
# gb_add_class(gbAppPkg ..)
# gb_add_class(gbByteOrderAmend ..)

gb_add_class(test .)


add_executable(gbClient
  ${gb_srcs}
  ${gb_headers}
  )

# add_library(gbClient
#   ${gb_srcs}
#   ${gb_headers}
#   )

set(ws2_32_lib)
if(WIN32)
  set(ws2_32_lib "ws2_32.lib")
endif()

set(pthread_lib)
if(UNIX)
  set(pthread_lib "pthread")
endif()

if(NOT gbUtils_LIBRARIES_d)
  message(WARNING "gbUtilsd not found")
  set(gbUtils_LIBRARIES_d ${gbUtils_LIBRARIES})
endif()

target_link_libraries(gbClient
  ${libevent_LIBRARIES}
  debug ${gbUtils_LIBRARIES_d}
  optimized ${gbUtils_LIBRARIES}
  ${lua_LIBRARIES}
  dl
  ${ws2_32_lib}
  ${pthread_lib}
  )
