cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
find_package(libevent REQUIRED)
include_directories(libevent_INCLUDE_DIRS)

find_package(lua REQUIRED)
include_directories(lua_INCLUDE_DIRS)

set(gb_server_srcs)
set(gb_server_headers)

macro(gb_add_class clsName dir)
  set(src ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.cpp)
  set(header ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${clsName}.h)

  if(EXISTS ${src})
    message("src@" ${src} " added")
    set(gb_server_srcs ${gb_server_srcs} ${src})
  else()
    message("src@" ${src} " not found")
  endif()

  if(EXISTS ${header})
    message("header@" ${header} " added")
    set(gb_server_headers ${gb_server_headers} ${header})
  else()
    message("header@" ${header} " not found")
  endif()
  
endmacro()

gb_add_class(server .)

add_executable(server
  ${gb_server_srcs} ${gb_server_headers}
  )

target_link_libraries(server
  ${libevent_LIBRARIES}
  ${lua_LIBRARIES}
  )


