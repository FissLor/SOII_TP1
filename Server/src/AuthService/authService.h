//
// Created by lfiss on 28/3/20.
//

/*!
 * @file authService.h
 *
 * @brief Authentication Service Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 28/3/20
 *
 * Encargado de proveer toda la funcionalidad asociada al
 * manejo de la base de datos de usuarios, esto es, validar usuarios, bloquear
 * usuario, listar usuarios y cambiar la contraseña de los mismos.
 *
 *
 */

#ifndef SERVER_AUTHSERVICE_H
#define SERVER_AUTHSERVICE_H
#define PROCES_NAME "authService"     //!< Nombre del proceso, con propositos imprimir mensajes por consola

#define USER_LS "ls"                  //!< Nombre interno del comando Listar
#define USER_PASSWD "passwd"          //!< Nombre interno del comando Cambiar Contraseña
#define MSG_IN 10                     //!< Codigo de mensajes entrantes
#define MSG_OUT 30                    //!< Codigo para mensajes salientes




/*!
 * \brief Encargada de la configuracion inicial, abre la cola de mensajes
 * @param[in] filePath Ruta al archivo con el cual se configurara la cola de mensajes
 *                     puede ser un relativo e.g.(./Server)
 * @param[in,out] database Contiene toda la informacion relacionada a la autenticacion
 * @return Si la operacion fue exitosa devuelve el identificador de la cola de mensajes msqid, caso contrario -1
 */
int config (char *filePath, Auth_DB *database);

/*!
 * \brief Verifica el logueo de usuarios, con los datos proporcionados por Primary Server
 * @param [in]msqid Identificador de la cola de mensajes
 * @param [in]database Contiene toda la informacion relacionada a la autenticacion
 * @return Devuelve 0, si la operacion fue exitosa.
 */
int log_in (int msqid, Auth_DB database);

/*!
 * \brief Encargada de manejar las solicitudes derivadas al servicio de autenticacion por el Primary Server
 * @param [in]msqid Identificador de la cola de mensajes
 * @param [in,out]database Contiene toda la informacion relacionada a la autenticacion
 * @return Devuelve 0, si la operacion fue exitosa.
 */
__int8_t handle_req (int msqid, Auth_DB *database);

/*!
 * \brief Ciclo principal del servicio
 * @param [in]msqid Identificador de la cola de mensajes
 * @param [in,out]database Contiene toda la informacion relacionada a la autenticacion
 */
void mainloop (int msqid, Auth_DB *database);

/*!
 * \brief Funcion encargada de devolver al Servidor la lista de usuarios en la base de datos
 * @param [in]msqid Identificador de la cola de mensajes
 * @param [in]database Contiene toda la informacion relacionada a la autenticacion
 * @return Devuelve 0, si la operacion fue exitosa.
 */
int list_users (int msqid, Auth_DB database);

/*!
 * \brief Se encarga de recibir una nueva contraseña, encriptarla y guardarla
 * @param [in]new_password
 * @param [in]msqid Identificador de la cola de mensajes
 * @param [in,out]database Contiene toda la informacion relacionada a la autenticacion
 * @return Devuelve 0 si la operacion fue exitosa.
 */
int change_password (char *new_password, int msqid, Auth_DB *database);
#endif //SERVER_AUTHSERVICE_H
