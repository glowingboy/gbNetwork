cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

find_package(libevent REQUIRED)
include_directories(SYSTEM ${libevent_INCLUDE_DIRS})

find_package(gbUtils REQUIRED)
include_directories(SYSTEM ${gbUtils_INCLUDE_DIRS})

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

gb_add_class(gbClientCore Core)

add_library(gbClient
  ${gb_srcs}
  ${gb_headers}
  )

set(ws2_32_lib)
if(WIN32)
  set(ws2_32_lib "ws_32.lib")
endif()

if(NOT gbUtils_LIBRARIES_d)
  message(WARNING "gbUtilsd not found")
  set(gbUtils_LIBRARIES_d ${gbUtils_LIBRARIES})
endif()

target_link_libraries(gbClient
  ${libevent_LIBRARIES}
  debug ${gbUtils_LIBRARIES_d}
  optimized ${gbUtils_LIBRARIES}
  ${ws2_32_lib}
  )
