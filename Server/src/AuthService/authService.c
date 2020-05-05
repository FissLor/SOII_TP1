//
// Created by lfiss on 28/3/20.
//
#include <stdio.h>
#include <zconf.h>
#include <sys/prctl.h>
#include <sys/signal.h>
#include <time.h>

#include "../../../lib/Auth_DB/Auth_DB.h"
#include "authService.h"
#include "../../../lib/MessageQueue/messageQueue.h"

#include "../../../lib/Util/util.h"

volatile sig_atomic_t keep_going = 1;
void handle_sigint (int sig)
{
  printf ("Auth handler\n");
  keep_going = 0;
}

int main (int argc, char *argv[])
{


  struct sigaction ctrl_c;
  memset (&ctrl_c, 0, sizeof (ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction (SIGINT, &ctrl_c, NULL);

  if (argc != 3)
    {
      fprintf (stderr, "%s Cantidad incorrecta de argumentos", PROCES_NAME);
      exit (EXIT_FAILURE);
    }

  printf ("\nProceso %d: AuthService Iniciado\n", getpid ());

  int msqid;

  Auth_DB database = {NULL, NULL, 0, NULL, NULL, -1, NULL};

  msqid = config (argv[1], &database);
  mainloop (msqid, &database);

  free (database.passwords);
  free (database.users);
  free (database.strikes);
  free (database.blocked);

  return 0;
}

int config (char *filePath, Auth_DB *database)
{

  prctl (PR_SET_PDEATHSIG, SIGINT);
  int msqid;
  if (create_mq (&msqid, filePath) != 0)
    {
      fprintf (stderr, "Error al crear la cola de mensajes\n");
      return 1;
    }

  data_fetch (database);

  return msqid;
}

void mainloop (int msqid, Auth_DB *database)//, char users[100][64], char passwords[100][64], int usercount)
{

  while (keep_going)
    {
      int status;
      status = log_in (msqid, database);

      if (status == 1)
        {
          continue;
        }

      status = handle_req (msqid, database);
      if (status == 0)
        {
          continue;
        }
      else if (status == 2)
        {
          exit (EXIT_SUCCESS);
        }
    }
}

__int8_t handle_req (int msqid, Auth_DB *database_ptr)
{
  char request[MSG_SIZE];
  msgbuf msg;

  while (keep_going)
    {

      if (mq_recv (msqid, &msg, MSG_IN) != 0)
        {
          fprintf (stderr, "%s: Error recibiendo request\n", PROCES_NAME);
          return -1;
        }

      snprintf (request, sizeof (request), "%s", msg.mtext);

      char **parsed = split_line (request);

      if (strcmp (parsed[0], "exit") == 0)
        {
          fprintf (stderr, "%s deje de recibir solicitudes", PROCES_NAME);
          return 0;
        }

      else if (strcmp (parsed[1], USER_LS) == 0)
        {

          list_users (msqid, *database_ptr);
          continue;
        }

      else if (strcmp (parsed[1], USER_PASSWD) == 0)
        {
          change_password (parsed[2], msqid, database_ptr);
          continue;
        }

      else
        {
          msg.mtype = MSG_OUT;
          snprintf (msg.mtext, sizeof (msg.mtext), "not-ok");
          mq_send (msg, msqid);
        }
      int i = 0;
      while (parsed[i] != NULL)
        {
          free (parsed[i]);
          i++;
        }
      free (parsed);
    }
  return 0;

}

int change_password (char *new_password, int msqid, Auth_DB *database)
{
//  char new_password [200];
  msgbuf msg;
  char * hashed_new_password = hash (new_password);

  fprintf (stderr, "%s\n", database->passwords[database->active]);
  snprintf (database->passwords[database->active], sizeof (database->passwords[database->active]), "%s", hashed_new_password);
  fprintf (stderr, "%s", database->passwords[database->active]);
  snprintf (msg.mtext, sizeof (msg.mtext), "Password changed");
  msg.mtype = MSG_OUT;
  mq_send (msg, msqid);

  data_save(*database);
  return 0;
}

int list_users (int msqid, Auth_DB database)
{
  msgbuf msg;
  msg.mtype = MSG_OUT;
  char * bloqueado;
  char last_time[100];
  snprintf (msg.mtext, sizeof (msg.mtext), "USUARIOS    ESTADO    ULTIMA CONEXION\n");
  for (u_int32_t i = 0; i < database.user_count; i++)
    {
      if(database.blocked[i]==1)
        bloqueado = "     BLOQUEADO";
      else
        bloqueado = "       Activo ";

//      last_time = ctime(&database.last_conection[i]);
      if(database.last_conection[i]!= -1)
        sprintf(last_time,"    %s",ctime(&database.last_conection[i]));
      else
        sprintf(last_time,"    No se registra conexion\n");
      strcat (msg.mtext, database.users[i]);
      strcat (msg.mtext, bloqueado);
      strcat (msg.mtext, last_time);
      strcat (msg.mtext, "\n");
    }

  mq_send (msg, msqid);

  return 0;
}

int log_in (int msqid, Auth_DB *database_ptr)
{
  msgbuf msg;

  char user[MSG_SIZE];
  char pass[MSG_SIZE];
  fprintf (stderr, "%s : Im logging ", PROCES_NAME);

//  memset(msg.mtext,0,sizeof(msg.mtext));

  if (mq_recv (msqid, &msg, MSG_IN) != 0)
    {
      fprintf (stderr, "%s Error recibiendo el usuario\n", PROCES_NAME);
      return -1;
    }

  snprintf (user, sizeof (user), "%s", msg.mtext);
  fprintf (stderr, "%srecibi usuario\n", PROCES_NAME);



  if (mq_recv (msqid, &msg, MSG_IN) != 0)
    {
      fprintf (stderr, "%s: Error recibiendo el usuario\n", PROCES_NAME);
      return -1;
    }

  snprintf (pass, sizeof (pass), "%s", msg.mtext);
  fprintf (stderr, "%s: recibi contraseña\n", PROCES_NAME);
  int8_t user_ID;
  user_ID = check_user (*database_ptr, user);
  if (user_ID == -1)
    {
      fprintf (stderr, "%s: Error en usuario o contrasena (1)\n", PROCES_NAME);
      snprintf (msg.mtext, sizeof (msg.mtext), "WRONG USER OR PASSWORD");
      msg.mtype = MSG_OUT;
      mq_send (msg, msqid);
      return 1;
    }
  if (database_ptr->blocked[user_ID] == 1)
    {
      fprintf (stderr, "%s: Usuario bloqueado\n", PROCES_NAME);
      snprintf (msg.mtext, sizeof (msg.mtext), "Usuario bloqueado");
      msg.mtype = MSG_OUT;
      mq_send (msg, msqid);
      return 1;
    }
  if (check_pass (*database_ptr, pass)==1)
    {
      database_ptr->strikes[user_ID]++;
      if (database_ptr->strikes[user_ID] == 3)
        {
          database_ptr->blocked[user_ID] = 1;
          fprintf (stderr, "%s: Usuario ha sido bloqueado\n", PROCES_NAME);
          snprintf (msg.mtext, sizeof (msg.mtext), "USUARIO O CONTRASENA ERRONEOS: El usuario ha sido bloqueado");
          msg.mtype = MSG_OUT;
          mq_send (msg, msqid);
        }
      else
        {
          fprintf (stderr, "%s: Error en usuario o contrasena (2)\n", PROCES_NAME);
          snprintf (msg.mtext, sizeof (msg.mtext), "USUARIO O CONTRASENA ERRONEOS");
          msg.mtype = MSG_OUT;
          mq_send (msg, msqid);
        }

      return 1;
    }

  database_ptr->active = user_ID;
  database_ptr->last_conection[user_ID] = time(NULL);
  snprintf (msg.mtext, sizeof (msg.mtext), "ok");
  msg.mtype = MSG_OUT;
  mq_send (msg, msqid);
  return 0;
}


