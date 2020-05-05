//
// Created by lfiss on 28/3/20.
//

/*!
 * @file server.h
 *
 * @brief Authentication Service Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 28/3/20
 *
 * Es el middleware encargado de establecer conexiones y
 * recibir comandos de conexiones válidas. Hace uso de los otros 2 procesos
 * para cumplir los requerimientos del software.
 *
 *
 */

#ifndef SERVER_SERVER_H

#define SERVER_SERVER_H

#define PROCES_NAME "Server"
#define MSG_AUTH_OUT 10             //!< Codigo de mensajes salientes para el servicio de Autenticacion
#define MSG_FILE_OUT 20             //!< Codigo de mensajes salientes para el servicio de Archivos
#define MSG_AUTH_IN 30              //!< Codigo de mensajes entrantes para el servicio de Autenticacion
#define MSG_FILE_IN 40              //!< Codigo de mensajes entrantes para el servicio de Archivos



#define TAM 256                     //!< Tamano de buffer para la comunicacion via socket
#define DEFAULT_PORT 2000           //!< Puerto por defecto en caso de no haber recibido uno especifico
#define DEFAULT_ADDR INADDR_ANY     //!< IP por defecto en caso de no haber recibido una especifico
#define DEFAULT_DOMAIN AF_INET      //!< El rango de direcciones es IPv4

///

#define AUTH_PATH "./AuthService"   //!< Direccion relativa del ejecutable del servicio de Autenticacion
#define FILE_PATH "./FileService"   //!< Direccion relativa del ejecutable del servicio de Archivos
#define FILE_SID 0                  //!< Constante para iniciar el servicio de Archivos
#define AUTH_SID 1                  //!< Constante para iniciar el servicio de Archivos

/*!
 *
 * @param argc
 * @param argv
 */
int config (int32_t argc, char *argv[]);


/*!
 *
 *
 */
void mainloop ();

/*!
 * \brief
 * @param newsockfd
 */
void attend_client (int32_t newsockfd);

/*!
 * Recibe los parametros de entrada del programa, configura los parametros IP:Puerto a los ingresados, de faltar alguno,
 * usa los valores por defecto.
 * @param argc Parametro del programa principal
 * @param argv Parametro del programa principal
 * @param puerto Puntero al puerto a utilizar
 */
void address_conversion (int argc, char *argv[], __uint16_t *puerto);
/*!
 * \brief Inicializa la estructura addr a partir de los parametros pasados al programa sino usa los valores
 *        por defecto
 * @param newsockfd Socket de la conexion establecida con la aplicacion cliente.
 * @return
 */
int32_t log_in (int32_t newsockfd);
/*!
 * \brief Ejecuta el servicio especificado por @a serv
 * @param serv Identificador del Servicio a iniciar
 */
void service_init (int8_t serv);

#endif //SERVER_FILESERVICE_H
