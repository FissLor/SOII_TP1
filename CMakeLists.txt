cmake_minimum_required(VERSION 3.16)
project(SOI_TP1 C)

set(CMAKE_C_STANDARD 11)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra -Wconversion")

add_subdirectory(lib/MessageQueue)
add_subdirectory(lib/Hash)
add_subdirectory(lib/Util)
add_subdirectory(lib/Directory)
add_subdirectory(lib/MD5)

add_executable(AuthService Server/src/AuthService/src/authService.c Server/src/AuthService/src/authService.h)
add_executable(FileService Server/src/FileService/src/fileService.c Server/src/FileService/src/fileService.h)
add_executable(Server Server/src/PrimaryServer/src/server.c Server/src/PrimaryServer/src/server.h)
add_executable(Client Client/src/Client.c)

target_link_libraries(Client  MD5 crypto)
target_link_libraries(Server  MessageQueue)
target_link_libraries(AuthService MessageQueue Hash crypt Util)
target_link_libraries(FileService MessageQueue Util Directory MD5 crypto)

set_target_properties( AuthService
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "Server/lib"
        LIBRARY_OUTPUT_DIRECTORY "Server/lib"
        RUNTIME_OUTPUT_DIRECTORY "Server/bin"
        )

set_target_properties( FileService
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../Server/lib"
        LIBRARY_OUTPUT_DIRECTORY "../Server/lib"
        RUNTIME_OUTPUT_DIRECTORY "../Server/bin"
        )
set_target_properties( Server
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../Server/lib"
        LIBRARY_OUTPUT_DIRECTORY "../Server/lib"
        RUNTIME_OUTPUT_DIRECTORY "../Server/bin"
        )
set_target_properties( Client
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "../Client/lib"
        LIBRARY_OUTPUT_DIRECTORY "../Client/lib"
        RUNTIME_OUTPUT_DIRECTORY "../Client/bin"
        )