//
// Created by lfiss on 21/4/20.
//

#include "hash.h"

#include <crypt.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>





char *hash(char *plain_password){
  unsigned char ubytes[16];
  char salt[20];
  const char *const saltchars =
      "./0123456789ABCDEFGHIJKLMNOPQRST"
      "UVWXYZabcdefghijklmnopqrstuvwxyz";
  char *hash;
  int i;

  /* Retrieve 16 unpredictable bytes from the operating system. */
  if (getentropy (ubytes, sizeof ubytes))
    {
      perror ("getentropy");
      return NULL;
    }

  /* Use them to fill in the salt string. */
  salt[0] = '$';
  salt[1] = '5'; /* SHA-256 */
  salt[2] = '$';
  for (i = 0; i < 16; i++)
    salt[3+i] = saltchars[ubytes[i] & 0x3f];
  salt[3+i] = '\0';

  /* Read in the user’s passphrase and hash it. */
//  hash = crypt(getpass ("Enter new passphrase: "), salt);
  hash = crypt(plain_password, salt);
  if (!hash || hash[0] == '*')
    {
      perror ("crypt");
      return NULL;
    }

  /* Print the results. */
  return hash;

}

int check_pass(char (*db)[64], char * checked, int usercount){
  for(int i = 0; i<usercount;i++){
      if(strcmp(db[i], crypt(checked,db[i])) == 0){
          return 0;
        }
    }

  return 1;
}

int check_user(char (*db)[64], char * checked, int usercount){
  for(int i = 0; i<usercount;i++){
      if(strcmp(db[i], checked) == 0){
          return 0;
        }
    }
  return 1;
}
