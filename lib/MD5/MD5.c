//
// Created by lfiss on 23/4/20.
//
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include "MD5.h"


unsigned char *md5 (char *filePath, long filesize)
{
  FILE *fh;
//  long filesize;
  unsigned char *buf;
  unsigned char *md5_result;

  fh = fopen (filePath, "r+");
  if (fh == NULL)
    {
      printf("fopen failed");
      return NULL;
    }
//  fseek (fh, 0L, SEEK_END);
//  filesize = ftell (fh);
//  printf("File size: %ld",filesize);
//  fseek (fh, 0L, SEEK_SET);
  buf = malloc ((unsigned long) filesize);
  fread (buf, (unsigned long) filesize, 1, fh);
  fclose (fh);
  md5_result = malloc (MD5_DIGEST_LENGTH);
  MD5 (buf, (size_t) filesize, md5_result);
  free (buf);
  return md5_result;
}

