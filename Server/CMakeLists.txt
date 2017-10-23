cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -DTHERON_CPP11=1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/")

find_package(libevent REQUIRED)
include_directories(SYSTEM ${libevent_INCLUDE_DIRS})

find_package(lua REQUIRED)
include_directories(SYSTEM ${lua_INCLUDE_DIRS})

find_package(gbUtils REQUIRED)
include_directories(SYSTEM ${gbUtils_INCLUDE_DIRS})

find_package(libtheron REQUIRED)
include_directories(SYSTEM ${libtheron_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(gb_server_srcs)
set(gb_server_headers)

macro(gb_add_class clsName dir)
  set(src ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.cpp)
  set(header ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.h)

  if(EXISTS ${src})
    message(STATUS "src@" ${src} " added")
    set(gb_server_srcs ${gb_server_srcs} ${src})
  else()
    message("src@" ${src} " not found")
  endif()

  if(EXISTS ${header})
    message(STATUS "header@" ${header} " added")
    set(gb_server_headers ${gb_server_headers} ${header})
  else()
    message("header@" ${header} " not found")
  endif()
  
endmacro()

gb_add_class(gbServer .)
gb_add_class(gbSvrNet .)
gb_add_class(gbSvrUsr Core)
# gb_add_class(gbUDPDataHandler .)
gb_add_class(gbActor .)
gb_add_class(gbAccInfo Data)
gb_add_class(gbSvrLogic .)
# gb_add_class(gbTCPPkg ..)
# gb_add_class(gbTCPPkgHandler .)
# gb_add_class(gbAppPkg ..)
# gb_add_class(gbByteOrderAmend ..)
# gb_add_class(gbUDPData ..)
# gb_add_class(gbAppPkg .. )
# gb_add_class(gbSvrLogicLoop .)
gb_add_class(gbCommunicator ..)
gb_add_class(gbType ..)
gb_add_class(gbNWMessageDispatcher .)
gb_add_class(gbSocketData ..)
add_executable(gbServer
  ${gb_server_srcs} ${gb_server_headers}
  )

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

if(NOT libtheron_LIBRARIES_d)
  message(WARNING "libtherond not found")
  set(libtheron_LIBRARIES_d ${libtheron_LIBRARIES})
endif()

target_link_libraries(gbServer
  ${libevent_LIBRARIES}
  debug ${gbUtils_LIBRARIES_d}
  optimized ${gbUtils_LIBRARIES}
  debug ${libtheron_LIBRARIES_d}
  optimized ${libtheron_LIBRARIES}
  ${lua_LIBRARIES}
  ${ws2_32_lib}
  ${pthread_lib}
  dl
  )


