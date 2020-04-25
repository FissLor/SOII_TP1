//
// Created by lfiss on 21/4/20.
//

#include "dir.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


int myfilter(const struct dirent *entry)
{

  if((strcmp(entry->d_name,".")==0)||(strcmp(entry->d_name,"..")==0))
    return 0; //No se guarda

  return 1;//Se guarda
}


char ** list_files (char *path)
{

  struct dirent **namelist;
  int n;

  n = scandir(path, &namelist, myfilter, alphasort);
  if (n == -1) {
      perror("scandir");
      return NULL;
  }

  char **nameListStr =(char**) malloc((unsigned long) n*sizeof(char*));

  for(int i = 0;i<n;i++){
    nameListStr[i] = malloc(sizeof(char)*256);
  }
//  char nameListStrr[n][256];


  for(int i=0; i<n;i++){
    nameListStr[i] = namelist[i]->d_name;
  }
  nameListStr[n]=NULL;

return nameListStr;
}

