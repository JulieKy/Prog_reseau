cmake_minimum_required(VERSION 2.8.3)

project(RE216)

#add here source files you need to compile and link to the server app
SET(SRC_SERVER
	server.c
	${structure_client_files}
	server_tools.h
  server_tools.c
	user_tools.h
	user_tools.c
    )


add_executable(RE216_JALON_SERVER  ${SRC_SERVER})
