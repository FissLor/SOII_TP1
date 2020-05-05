//
// Created by lfiss on 23/4/20.
//

/*!
 * @file MD5.h
 *
 * @brief MD5 lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 23/4/20
 *
 * Libreria que permite almacenar calcular el hash MD5 de un archivo para
 * la comprobacion del checksum.
 *
 *
 */

#ifndef _MD5_H_
#define _MD5_H_

/*!
 * \brief Calcula el hash MD5 del contenido de un archivo
 * @param file_Path Direccion del archivo al cual se le calculara el checksum (MD5)
 * @return Hash del archivo del cual se paso el @a file_Path
 */
unsigned char *md5(char* file_Path, long filesize);
#endif //_MD5_H_
