//
// Created by lfiss on 28/3/20.
//

/*!
 * @file fileService.h
 *
 * @brief File Service Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 28/3/20
 *
 * Encargado de proveer funcionalidad relacionada a las imágenes
 * disponibles para descarga, cómo obtener nombres, tamaño y sus hash
 * MD5.
 *
 * También es el encargado establecer la transferencia del archivo a
 * través de un nuevo socket.
 *
 *
 */

#ifndef SERVER_FILESERVICE_H
#define SERVER_FILESERVICE_H

#define PROCES_NAME "fileService"     //!< Nombre del proceso, con propositos imprimir mensajes por consola
#define FILE_LS "ls"                  //!< Nombre interno del comando Listar
#define FILE_DOWN "down"              //!< Nombre interno del comando Descargar

#define MSG_IN 20                     //!< Codigo de mensajes entrantes
#define MSG_OUT 40                    //!< Codigo para mensajes salientes

/*!
 * \brief Configura la cola de mensajes y guarda la referencia del socket del servidor
 * @param filePath
 * @param sockfd_str
 * @param database_ptr Contiene toda la informacion relacionado a las imagenes disponibles para descarga
 * @return Si la operacion fue exitosa devuelve el identificador de la cola de mensajes msqid, caso contrario -1
 */
int config (char *filePath, char *sockfd_str, File_DB *database_ptr);

/*!
 * \brief Responde al servidor con la lista de archivos
 * @param database Contiene toda la informacion relacionado a las imagenes disponibles para descarga
 */
void file_ls (File_DB database);

/*!
 * \brief Envia el archivo solicitado a travez de un nuevo socket
 * @param database Contiene toda la informacion relacionado a las imagenes disponibles para descarga
 * @param filecode Codigo del archivo del cual se solicita descarga
 * @return
 */
int send_file (File_DB database, char *filecode);

/*!
 * \brief Maneja las solicitudes recibidas del Servidor.
 * @param database Contiene toda la informacion relacionado a las imagenes disponibles para descarga.
 * @return Al finalizar normalmente devuelve 0.
 */
int8_t handle_req (File_DB database);

/*!
 * \brief Ciclo principal del programa
 * @param database Contiene toda la informacion relacionado a las imagenes disponibles para descarga
 */
void mainloop (File_DB database);

#endif //SERVER_FILESERVICE_H
