//
// Created by lfiss on 12/4/20.
//

/*!
 * @file File_DB.h
 *
 * @brief File_DB lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 12/4/20
 *
 * Libreria para gestionar la base de datos de las imagenes descargables
 *
 *
 */
#ifndef _FILE_DB_H_
#define _FILE_DB_H_
#define SIZE_STR 512 //!< Tamaño asignado para los string  de la base de datos

typedef struct databaseFile{
    ///Lista de archivos
    char (*file_list)[SIZE_STR];
    ///Lista de direcciones de los archivos(relativas)
    char (*file_paths)[SIZE_STR];
    ///lista de tamanos de los archivos en formato imprimible
    char (*file_size_str)[SIZE_STR];
    ///Lista de los tamanos de los archivos
    long int *file_size;
    ///Lista de los hash MD5 de cada archivo
    unsigned char **hash;
    /// Cantidad de archivos en la base de datos
    u_int32_t file_count;

}File_DB;//!< Tipo definido para la estructura de la base de datos, contiene toda la info sobre los archivos descargables



/*!
 *  Filtro para listar solo los archivos de interes
 * @param entry Elemento encontrado en el directorio a analizar, se decidira si se incluye en la lista o no
 * @return Retorna 1 si @a entry se incluira en la lista, de lo contrario devuelve 1.
 */
int my_file_filter (const struct dirent *entry);


/*!
 * Inicializa la base de datos con los datos requeridos sobre las imagenes descargables en un directorio
 * @param path
 * @param database
 * @return
 */
int get_files_data (char *path, File_DB *database);


#endif //_FILE_DB_H_
