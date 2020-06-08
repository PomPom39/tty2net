cmake_minimum_required(VERSION 3.5.1)
project(tty2ethernet)

#Header directory
include_directories(include)


#Adding own libraries
#file( GLOB LIB_SOURCES libs/*c)
#file( GLOB LIB_HEADERS libs/*h)

#add_library(netconf ${LIB_SOURCES} ${LIB_HEADERS} )


#manually adding sources
set(SOURCES src/main.c)
set(CMAKE_C_FLAGS "-O0")




#can also use the file glob
#file(GLOB SOURCES "src/*.c")

add_executable(main ${SOURCES})

#add_executable(stdin2ethernet ${SOURCES})
#TARGET_LINK_LIBRARIES(mainApp netconf pthread)