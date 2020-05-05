//
// Created by lfiss on 4/5/20.
//

/*!
 * @file Client.h
 *
 * @brief Client Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 12/4/20
 *
 * Programa Cliente
 *
 *
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

/*!
 * \brief Funcion de configuracion inicial del programa, se encarga de levantar el socket
 *
 * @param argc
 * @param argv
 * @return El file descriptor del socket
 */
int32_t config (int argc, char *argv[]);

/*!
 * \brief Ciclo principal del programa
 */
void mainloop ();
/*!
 * \brief funcion que se encarga del logueo en el servidor
 * @param buffer
 */
void log_in (char *buffer);
/*!
 * \brief Funcion para obtener la contrasena sin mostrarla en la terminal
 * @param password
 * @return
 */
int get_pass (char *password);
/*!
 * \brief Funcion para la descarga de la imagen booteable
 */
void download ();
/*!
 * \brief Funcion para explorar un directorio y devolver su contenido como una lista de string
 * @param path
 * @return
 */
char **list_files (char *path);

#endif //_CLIENT_H_
