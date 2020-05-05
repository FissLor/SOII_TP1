//
// Created by lfiss on 25/4/20.
//


#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include "MBR.h"

void getMBR (FILE *file, MBR *masterBR)
{
  unsigned long n = fread (masterBR, 1, MBR_SIZE, file);
  if (n == 0)
    {
      perror ("No se pudo obtener MBR");
    }
}

void printPT (MBR *masterBR)
{
  PARTITION *table_entries = masterBR->partition_table.table_entries;

  printf ("BOOT\tCHS_START_VALUES\tTYPE\tCHS_END_VALUES\tSTART\t\tSIZE\n");
  for (int i = 0; i < 4; i++)
    {
      printf ("%02x\t\t", table_entries[i].boot);
      for (int j = 0; j < 3; j++)
        printf ("%02x ", table_entries[i].chs_start[j]);
      printf ("\t %02x\t      ", table_entries[i].type);
      for (int j = 0; j < 3; j++)
        printf ("%02x ", table_entries[i].chs_end[j]);

      printf ("%6u", (uint32_t) table_entries[i].start);
      printf ("\t");

      printf ("%12u ", (uint32_t) table_entries[i].size);

      printf ("\n");

    }
}
