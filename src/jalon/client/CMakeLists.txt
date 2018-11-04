cmake_minimum_required(VERSION 2.8.3)

project(RE216)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
    ${client_tools_files}
    client_tools.h
    client_tools.c
	)


add_executable(RE216_JALON_CLIENT ${SRC_CLIENT})

