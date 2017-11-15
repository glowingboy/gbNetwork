cmake_minimum_required(VERSION 3.0)
include(../gbCMake/gbCommon.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Dgb_CLNT=1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake/")

find_package(libevent REQUIRED)
include_directories(SYSTEM ${libevent_INCLUDE_DIRS})

find_package(gbUtils REQUIRED)
include_directories(SYSTEM ${gbUtils_INCLUDE_DIRS})

find_package(lua REQUIRED)
include_directories(SYSTEM ${lua_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(gb_srcs)

gb_add_class(gbIOEvent .. gb_srcs)
gb_add_class(gbIOEventHandler .. gb_srcs)
gb_add_class(gbClntIORecvDataHandler . gb_srcs)
gb_add_class(gbClntIOEventHandler . gb_srcs)
gb_add_class(gbIOTunnel .. gb_srcs)
gb_add_class(gbCommunicator .. gb_srcs)
gb_add_class(gbReceptionistComm .. gb_srcs)
gb_add_class(gbWatchdogComm .. gb_srcs)

gb_add_class(test . gb_srcs)

set(gb_srcs ${gb_srcs}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CommMsg/gbCommMsg.pb.cc
  )

add_executable(gbClient
  ${gb_srcs}
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

target_link_libraries(gbClient
  ${libevent_LIBRARIES}
  debug ${gbUtils_LIBRARIES_d}
  optimized ${gbUtils_LIBRARIES}
  ${lua_LIBRARIES}
  dl
  ${ws2_32_lib}
  ${pthread_lib}
  protobuf-lite
  )
