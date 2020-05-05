//
// Created by lfiss on 4/4/20.
//

#define _GNU_SOURCE


#include <stdio.h>
#include <unistd.h>
#include  <sys/random.h>
#include <crypt.h>
#include <string.h>


/* ‘GNU's Not Unix’ hashed using SHA-256, MD5, and DES. */
static const char hash_sha[] =
    "$5$DQ2z5NHf1jNJnChB$kV3ZTR0aUaosujPhLzR84Llo3BsspNSe4/tsp7VoEn6";
static const char hash_md5[] = "$1$A3TxDv41$rtXVTUXl2LkeSV0UU5xxs1";
static const char hash_des[] = "FgkTuF98w5DaI";

char *hash(char *plain_password);
int check_password();

int
main(void)
{
  FILE *dat = fopen("../database/dat","w");
  char * users[] = {"lfiss", "Tbone", "admin"};
  char * passwords[] = {"lfiss", "Tbone", "admin"};
  int n = sizeof(users)/sizeof(users[0]);
  for(int i=0; i < n; i++){
//    users[i]=hash(users[i]);
      passwords[i]=hash(passwords[i]);
      fprintf(dat, "%s %s %s\n", users[i],passwords[i],"-1");
    }



  fclose(dat);
  return 0;
}

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

int check_password(){
  char *phrase;
  int status = 0;

  /* Prompt for a passphrase. */
  phrase = getpass ("Enter passphrase: ");

  /* Compare against the stored hashes.  Any input that begins with
     ‘GNU's No’ will match the DES hash, but the other two will
     only match ‘GNU's Not Unix’. */

  if (strcmp (crypt (phrase, hash_sha), hash_sha))
    {
      puts ("SHA: not ok");
      status = 1;
    }
  else
    puts ("SHA: ok");

  if (strcmp (crypt (phrase, hash_md5), hash_md5))
    {
      puts ("MD5: not ok");
      status = 1;
    }
  else
    puts ("MD5: ok");

  if (strcmp (crypt (phrase, hash_des), hash_des))
    {
      puts ("DES: not ok");
      status = 1;
    }
  else
    puts ("DES: ok");

  return status;
}
