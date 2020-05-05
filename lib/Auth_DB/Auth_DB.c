//
// Created by lfiss on 21/4/20.
//
#include <crypt.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "Auth_DB.h"

char *hash (char *plain_password)
{
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
    salt[3 + i] = saltchars[ubytes[i] & 0x3f];
  salt[3 + i] = '\0';



  /* Read in the user’s passphrase and hash it. */
//  hash = crypt(getpass ("Enter new passphrase: "), salt);
  hash = crypt (plain_password, salt);
  if (!hash || hash[0] == '*')
    {
      perror ("crypt");
      return NULL;
    }

  /* Print the results. */
  return hash;

}

int data_fetch (Auth_DB *database)
{
  FILE *dat = fopen ("./dat", "r");
  if (dat == NULL)
    {
      fprintf (stderr, "No existe archivo dat");
      return -1;
    }
  char user_buff[64];
  char pass_buff[64];

  database->users = malloc (0 * sizeof (char[64]));
  database->passwords = malloc (0 * sizeof (char[64]));

  u_int32_t i = 0;
  while (fscanf (dat, "%63s %63s", user_buff, pass_buff) > 0)
    {
      database->users = realloc (database->users, (unsigned long) (i + 1) * sizeof (char[64]));
      database->passwords = realloc (database->passwords, (unsigned long) (i + 1) * sizeof (char[64]));
      strcpy (database->users[i], user_buff);
      strcpy (database->passwords[i], pass_buff);
      i = i + 1;
    }
  fclose (dat);
  database->user_count = i;
  database->active = -1;
  database->strikes = malloc (i * sizeof (*database->strikes));
  database->blocked = malloc (i * sizeof (*database->blocked));
  database->last_conection = malloc (i * sizeof (*database->last_conection));

  for (i = 0; i < database->user_count; i++)
    {
      database->strikes[i] = 0;
      database->blocked[i] = 0;
      database->last_conection[i] = (time_t) -1;
    }

  return 0;
}

int check_pass (Auth_DB database, char *password_to_check)
{
  for (u_int32_t i = 0; i < database.user_count; i++)
    {
      if (strcmp (database.passwords[i], crypt (password_to_check, database.passwords[i])) == 0)
        {
          return 0;
        }
    }

  return 1;
}

int8_t check_user (Auth_DB database, char *user_to_check)
{
  for (u_int32_t i = 0; i < database.user_count; i++)
    {
      if (strcmp (database.users[i], user_to_check) == 0)
        {
          return (int8_t) i;
        }
    }
  return -1;
}
