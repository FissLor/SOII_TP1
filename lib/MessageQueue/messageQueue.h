//
// Created by lfiss on 4/4/20.
//

/*!
 * @file messageQueue.h
 *
 * @brief Message Queue lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 23/4/20
 *
 * Libreria que funciona como interfaz a las colas de mensajes
 *
 */

#ifndef SERVER_MESSAGEQUEUE_H
#define SERVER_MESSAGEQUEUE_H

#include <zconf.h>
#ifdef PROCES_NAME
#define PROCES_NAME_MQ PROCES_NAME
#endif
#define PERMS 0644
#define MSG_SIZE 512

typedef struct msg {
  long mtype;
  char mtext[MSG_SIZE];
}msgbuf;//!< Estructura para el envio de mensajes entre procesos

/*!
 * \brief Crea una cola de mensajes
 * @param msqid Identificador de la cola de mensajes
 * @param filePath Direccion del archivo con el cual se calculara el token para la creacion de la cola
 * @return Devuelve 0 si la operacion se llevo a cabo con exito
 */
int create_mq(int *msqid , char *filePath);

/*!
 * \brief Envia un mensaje a travez de una cola de mensajes
 * @param msg Mensaje a enviar
 * @param msqid Identificador de la cola de mensajes
 * @return Devuelve 0 si la operacion se llevo a cabo con exito
 */
int mq_send(msgbuf msg, int msqid);


/*!
 * \brief Recive un mensaje a travez de una cola de mensajes
 * @param msqid Identificador de la cola de mensajes
 * @param msg Estructura msgbuf en la que se almacenara el mensaje recibido
 * @param mtype Tipo de mensaje que se sacara de la cola
 * @return Devuelve 0 si la operacion se llevo a cabo con exito
 */
int mq_recv(int msqid, msgbuf *msg, long mtype); //blocking

#endif // SERVER_MESSAGEQUEUE_H
