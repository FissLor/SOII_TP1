//
// Created by lfiss on 12/4/20.
//

/*!
 * @file Auth_DB.h
 *
 * @brief Auth_DB lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 12/4/20
 *
 * Libreria para gestionar la base de datos de usuarios y contraseñas
 *
 *
 */
#ifndef SERVER_HASH_H
#define SERVER_HASH_H

typedef struct databaseAuth {
    ///Lista de usuarios
    char (*users)[64];
    ///Lista de contraseñas
    char (*passwords)[64];
    ///Cantidad de usuarios en la base de datos
    u_int32_t user_count;
    ///Cantidad de fallos en en el logueo (por usuario)
    u_int8_t *strikes;
    ///Banderas señalizando si un usuario esta bloqueado
    u_int8_t *blocked;
    ///Banderas que señalizan el usuario activo
    int8_t active;
    ///Array que contiene las marcas de tiempo de la ultima conexion de cada usuario
    time_t *last_conection;
}Auth_DB; //!< Estructura de datos que representa la base de datos de usuarios y contrasenas para la autenticacion de las conexiones con clientes

/*!
 * \brief Se encarga de levantar los datos de la base de datos AuthService/dat
 * @param[in,out] database Puntero a estructura que toda la informacion de la base de datos
 * @return Al retornar los punteros estaran inicializados con las respectivas listas
 *         de usuarios  contraseñas. Ademas devuelve 0 si la operacion fue exitosa
 *         o un valor distinto de 0 en caso contrario
 */
int data_fetch (Auth_DB *database);

/*!
 * \brief Funcion que clacula el hash (SHA256) de una contraseña en texto plano
 * @param plain_password Contraseña en texto plano de la que se calculara el hash (SHA256)
 * @return Hash de @a plain_password
 */
char *hash (char *plain_password);

/*!
 * \brief Funcion que compara las contraseñas con las de la base de datos
 * @param database Base de datos sobre la cual chequear
 * @param password_to_check Contraseña que debe chequearse en la base de datos
 * @return
 */
int check_pass (Auth_DB database, char *password_to_check );

/*!
 * \brief Funcion para comparar los usuarios con la base de datos
 * @param database  Base de datos sobre la cual chequear
 * @param user_to_check Usuario que debe chequearse en la base de datos
 * @return
 */
int8_t check_user (Auth_DB database, char *user_to_check);

#endif // SERVER_HASH_H
