//
// Created by lfiss on 25/4/20.
//


#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include "MBR.h"

int main(){
  FILE * file;

  file = fopen ("/home/lfiss/CLionProjects/TP1/SOI-TP1/Client/bin", "r+");
  if (file == NULL)
    {
      fprintf (stderr, "Failed to open file foo --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }
  mbr file_mbr;
  getMBR(file,&file_mbr);
  printPT(&file_mbr);
  fclose(file);
  return 0;

}


void getMBR(FILE* file,  mbr *masterBR)
{



  unsigned long n = fread(&masterBR,1,sizeof(mbr),file);
  if(n==0){
    perror("No se pudo obtener MBR");
  }

}

void printPT(mbr *masterBR)
{
  partition table_entries[4];
  table_entries[0] = masterBR->partition_table.table_entry1;
  printf("BOOT\tCHS_START_VALUES\tTYPE\tCHS_END_VALUES\tSTART\tSIZE");
  for(int i=0;i<4;i++)
    {
      printf("%2x\t",table_entries[i].boot);
      for(int j = 0; j<3;j++)
        printf("%2x ",table_entries[i].chs_start[j]);
      printf("\t%2x\t",table_entries[i].type);
      for(int j = 0; j<3;j++)
        printf("%2x ",table_entries[i].chs_end[j]);
      printf("\t");
      for(int j = 0; j<4;j++)
        printf("%2x ",table_entries[i].start[j]);
      printf("\t");
      for(int j = 0; j<4;j++)
        printf("%2x ",table_entries[i].size[j]);

      printf("\n");

    }
}

