//
// Created by lfiss on 28/3/20.
//

#ifndef SERVER_SERVER_H

#define SERVER_SERVER_H
#define PROCES_NAME "Server"
#define MSG_AUTH_OUT 10
#define MSG_FILE_OUT 20
#define MSG_AUTH_IN 30
#define MSG_FILE_IN 40



#define TAM 256
#define DEFAULT_PORT 2000
#define DEFAULT_ADDR INADDR_ANY //Localhost o 0.0.0.0
#define DEFAULT_DOMAIN AF_INET



#define AUTH_PATH "./AuthService"
#define FILE_PATH "./FileService"
#define FILE_SID 0
#define AUTH_SID 1

#endif //SERVER_FILESERVICE_H
