//
// Created by lfiss on 25/4/20.
//

#include <stdint.h>
#ifndef _MBR_H_
#define _MBR_H_
#define MBR_SIZE 512




typedef struct partition {
    /// Boot Indicator (80h = active)
    uint8_t boot;
    /// Starting CHS values
    uint8_t chs_start[3];
    /// Partition-type Descriptor
    uint8_t type;
    ///Table Entry for Primary Partition # 4
    uint8_t chs_end[3];
    uint8_t start[4];
    uint8_t size[4];


} __attribute__ ((__packed__)) partition;

typedef struct mpt {
    /// Table Entry for Primary Partition # 1
    partition table_entry1;
    /// Table Entry for Primary Partition # 2
    partition table_entry2;
    /// Table Entry for Primary Partition # 3
    partition table_entry3;
    ///Table Entry for Primary Partition # 4
    partition table_entry4;
} __attribute__ ((__packed__)) mpt;



typedef struct mbr {
    /// Code Area
    uint8_t code_area[446];
    ///
    mpt partition_table;
    /// 2 byte Master Record Signature
    uint8_t signature[2];
} __attribute__ ((__packed__)) mbr;

void getMBR(FILE* file,  mbr *masterBR);
void printPT(mbr *masterBR);


#endif //_MBR_H_









