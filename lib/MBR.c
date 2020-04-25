//
// Created by lfiss on 25/4/20.
//

#include "MBR.h"


typedef struct _sbmp_ftype_data {
    /// A 2 character string value in ASCII to specify a DIB file type.
    /// It must be 'BM' or '0x42 0x4D' in hexadecimals for modern compatibility reasons.
    uint16_t file_type; // (Endianness) 'BM' - '0x42 0x4D' (Endianness)
    /// Entire file size in bytes. This value is basically the number of bytes in a BMP image file.
    uint32_t file_size;
    /// It should be initialized to '0' integer (unsigned) value.
    uint32_t reserved;
    ///  the offset of actual pixel data in bytes.
    /// In nutshell:- it is the number of bytes between start of the file (0) and the first byte of the pixel data.
    uint32_t data_offset;
} __attribute__ ((__packed__)) sbmp_ftype_data;