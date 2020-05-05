//
// Created by lfiss on 25/4/20.
//

/*!
 * @file MBR.h
 *
 * @brief MBR lib Header File
 *
 * @author Lorenzo Fissore
 *
 * @date 25/4/20
 *
 * Libreria para la lectura e impresion del Master Boot Record de un archivo
 *
 *
 */

#include <stdint.h>
#ifndef _MBR_H_
#define _MBR_H_
#define MBR_SIZE 512

/*!
 * \brief Estructura para representar la informacion de una particion de la tabla
 */
typedef struct partition {
    /// Boot Indicator (80h = active)
    uint8_t boot;
    /// Starting CHS values
    uint8_t chs_start[3];
    /// Partition-type Descriptor
    uint8_t type;
    ///Table Entry for Primary Partition # 4
    uint8_t chs_end[3];
    uint32_t start;
    uint32_t size;

} __attribute__ ((__packed__)) PARTITION;

/*!
 * \brief Estructura que representa la Tabla de particiones
 */
typedef struct mpt {
    /// Table Entries for Primary Partition
    PARTITION table_entries[4];
} __attribute__ ((__packed__)) MPT;

/*!
 * \brief Estructura que representa el Master Boot Record
 */
typedef struct mbr {
    /// Code Area
    uint8_t code_area[446];
    ///
    MPT partition_table;
    /// 2 byte Master Record Signature
    uint8_t signature[2];
} __attribute__ ((__packed__)) MBR;

/*!
 * \brief Obtiene el Master Boot Record del archivo @a file
 *
 *
 * @param[in] file Puntero al archivo del que se cargara el MBR
 * @param[in,out] masterBR Puntero a estructura MBR en la que se guardara la informacion
 */
void getMBR (FILE *file, MBR *masterBR);

/*!
 * \brief Imprime la Tabla de particiones del MBR
 * @param[in] masterBR Master Boot Record del que se imprimira la tabla de particiones
 */
void printPT (MBR *masterBR);

#endif //_MBR_H_









