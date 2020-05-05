//
// Created by lfiss on 21/4/20.
//



#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "../MD5/MD5.h"

#include "File_DB.h"

int my_file_filter (const struct dirent *entry)
{

  if ((strcmp (entry->d_name, ".") == 0) || (strcmp (entry->d_name, "..") == 0))
    return 0; //No se guarda

  return 1;//Se guarda
}




int get_files_data (char *path, File_DB *database)
{

  struct dirent **namelist;
  int n;
  struct stat file_stat;

  n = scandir (path, &namelist, my_file_filter, alphasort);
  if (n == -1)
    {
      perror ("scandir");
      return 1;
    }

  database->file_list = malloc ((unsigned long) n * sizeof (char[SIZE_STR]));
  database->file_paths = malloc ((unsigned long) n * sizeof (char[SIZE_STR]));
  database->file_size_str = malloc ((unsigned long) n * sizeof (char[SIZE_STR]));
  database->file_size = malloc ((unsigned long) n * sizeof (unsigned long));
  database->hash = malloc ((unsigned long) n);

  for (int i = 0; i < n; i++)
    {
      snprintf (database->file_list[i], SIZE_STR, "%s", namelist[i]->d_name);

      sprintf (database->file_paths[i], "../iso/%s", database->file_list[i]);
      if (stat (database->file_paths[i], &file_stat) < 0)
        {
          fprintf (stderr, "Error fstat --> %s", strerror (errno));
          exit (EXIT_FAILURE);
        }

      database->file_size[i] = file_stat.st_size;
      if (database->file_size[i] > 1024)
        {
          if (database->file_size[i] > (1024 * 1024))
            sprintf (database->file_size_str[i], "%ld Mb", database->file_size[i] / 1024 / 1024);
          else
            sprintf (database->file_size_str[i], "%ld Kb", database->file_size[i] / 1024);
        }
      else
        sprintf (database->file_size_str[i], "%ld b", database->file_size[i]);
      database->hash[i] = md5 (database->file_paths[i], database->file_size[i]);
    }
  database->file_count = (u_int32_t) n;

  return 0;
}
