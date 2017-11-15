cmake_minimum_required(VERSION 3.0)
include(../gbCMake/gbCommon.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTHERON_CPP11=1 -Dgb_SVR=1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake/")

find_package(libevent REQUIRED)
# include_directories(SYSTEM ${libevent_INCLUDE_DIRS})

find_package(lua REQUIRED)
# include_directories(SYSTEM ${lua_INCLUDE_DIRS})

find_package(gbUtils REQUIRED)
# include_directories(SYSTEM ${gbUtils_INCLUDE_DIRS})

find_package(libtheron REQUIRED)
include_directories(SYSTEM ${libtheron_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(gb_server_srcs)

gb_add_class(gbServer . gb_server_srcs)
# gb_add_class(gbSvrNet .)
# gb_add_class(gbSvrUsr Core)
gb_add_class(gbActor . gb_server_srcs)
gb_add_class(gbAccInfo Data gb_server_srcs)
gb_add_class(gbSvrLogic . gb_server_srcs)

gb_add_class(gbType .. gb_server_srcs)
gb_add_class(gbIOEvent .. gb_server_srcs)
gb_add_class(gbIOEventHandler .. gb_server_srcs) 
gb_add_class(gbIOTunnel .. gb_server_srcs)
gb_add_class(gbSvrIOEventDispatcher . gb_server_srcs)
gb_add_class(gbSvrIORecvDataDispatcher . gb_server_srcs)
gb_add_class(gbCommunicator .. gb_server_srcs)
gb_add_class(gbReceptionistComm .. gb_server_srcs)
gb_add_class(gbWatchdogComm .. gb_server_srcs)

# adding CommMsg
set(gb_server_srcs ${gb_server_srcs}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CommMsg/gbCommMsg.pb.cc
  )


add_executable(gbServer
  ${gb_server_srcs}
  )

set(ws2_32_lib)
if(WIN32)
  set(ws2_32_lib "ws2_32.lib")
endif()

set(pthread_lib)
if(UNIX)
  set(pthread_lib "pthread")
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
  protobuf-lite
  )


