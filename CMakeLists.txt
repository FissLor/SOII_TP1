cmake_minimum_required(VERSION 3.16)
project(SOII_TP1 C)

set(CMAKE_C_STANDARD 11)

find_package( Doxygen )

if ( DOXYGEN_FOUND )
    set( DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs )
#    set( DOXYGEN_COLLABORATION_GRAPH YES )
    set( DOXYGEN_EXTRACT_ALL YES )
    set ( DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES )
    set( DOXYGEN_CLASS_DIAGRAMS YES )
    set (DOXYGEN_INTERACTIVE_SVG YES)
    set( DOXYGEN_DISABLE_INDEX YES )
    set( DOXYGEN_GENERATE_TREEVIEW YES )
    set( DOXYGEN_CLASS_GRAPH YES )
    set( DOXYGEN_CALL_GRAPH YES )
    set( DOXYGEN_CALLER_GRAPH YES )
    set( DOXYGEN_COLLABORATION_GRAPH YES )

    set( DOXYGEN_UML_LOOK YES )
    set( DOXYGEN_UML_LIMIT_NUM_FIELDS 50 )
    set( DOXYGEN_TEMPLATE_RELATIONS YES )
    set( DOXYGEN_DOT_GRAPH_MAX_NODES 100 )
    set( DOXYGEN_DOT_TRANSPARENT YES )

    set( DOXYGEN_HTML_DYNAMIC_SECTIONS YES)

    doxygen_add_docs( doxygen )

else(DOXYGEN_FOUND)

    message( "Doxygen need to be installed to generate the doxygen documentation" )

endif(DOXYGEN_FOUND)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra -Wconversion")

add_subdirectory(lib/MessageQueue)
add_subdirectory(lib/Auth_DB)
add_subdirectory(lib/Util)
add_subdirectory(lib/MD5)
add_subdirectory(lib/MBR)
add_subdirectory(lib/File_DB)


add_executable(AuthService Server/src/AuthService/authService.c Server/src/AuthService/authService.h)
add_executable(FileService Server/src/FileService/fileService.c Server/src/FileService/fileService.h)
add_executable(Server Server/src/PrimaryServer/server.c Server/src/PrimaryServer/server.h)
add_executable(Client Client/src/Client.c Client/src/Client.h)
add_executable(database_init Server/src/AuthService/databaseInit.c)

target_link_libraries(database_init crypt)

target_link_libraries(Client  MD5 crypto MBR)
target_link_libraries(Server  MessageQueue)
target_link_libraries(AuthService MessageQueue Auth_DB crypt Util)
target_link_libraries(FileService MessageQueue Util File_DB MD5 crypto)

set_target_properties( AuthService
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Server/bin"
        )

set_target_properties( database_init
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Server/bin"
        )

set_target_properties( FileService
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Server/bin"
        )
set_target_properties( Server
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Server/bin"
        )
set_target_properties( Client
        PROPERTIES


        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Client/bin"
        )