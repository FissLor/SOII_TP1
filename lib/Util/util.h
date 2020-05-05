//
// Created by lfiss on 19/4/20.
//
/*!
 * @file util.h
 *
 * @brief Util lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 19/4/20
 *
 * Libreria de utilidades, implementa la funcionalidad para parsear una linea de comandos
 *
 */


#ifndef _UTIL_H_
#define _UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOK_BUFSIZE 64 ///< Tamano de buffer para un token
#define TOK_DELIM " \t\r\n\a\0" ///< Delimitadores de los comandos a parsear en la linea


/*!
 * \brief Realiza la descomposicion de una linea ingresada como parametro
 * @param line Line a ser descompuesta
 * @return La lista de los componentes de la linea ingresada
 */
char **split_line(char *line);

#endif //_UTIL_H_
