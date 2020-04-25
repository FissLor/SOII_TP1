#include "server.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/msg.h>
#include "../../../../lib/MessageQueue/messageQueue.h"

#define MQ_MSGLEN 256

int32_t sockfd;
u_int32_t clilen;

struct sockaddr_in serv_addr, cli_addr;
int msqid;

int config (int32_t argc, char *argv[]);
void mainloop ();
void launch_client ();
void attend_client__d ();
void attend_client (int32_t newsockfd);
void read_socket (char *buffer, int32_t newsockfd);

void address_conversion (int argc, char *argv[], __uint16_t *puerto);
int32_t log_in (char *userID, int32_t newsockfd);
void service_init (int8_t);

void blockUser (char *id);
char *errormsg;

volatile sig_atomic_t keep_going = 1;
volatile sig_atomic_t Auth_PID;
volatile sig_atomic_t File_PID;

void handle_sigint (int sig)
{

  kill(Auth_PID,sig);
  kill(File_PID,sig);

  shutdown (sockfd, 2);
  printf ("Se cerro el socket\n");
  if (msgctl (msqid, IPC_RMID, NULL) == -1)
    perror ("Eliminando cola : ");
  printf ("Se cerro la cola de mensajes\n");

//    sleep(1);
  keep_going = 0;
}

/*!
 *
 * @param argc
 * @param argv
 * @return
 */
int32_t main (int32_t argc, char *argv[])
{
  struct sigaction ctrl_c;
  memset (&ctrl_c, 0, sizeof (ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction (SIGINT, &ctrl_c, NULL);

  msqid = config (argc, argv);

  mainloop (msqid);

  return 0;
}

/*!
 *
 * @param argc
 * @param argv
 */
int config (int argc, char *argv[])
{

  printf ("\n**********Config**********\n");
  u_int16_t puerto;

  //#####################Socket################################


//  memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
  address_conversion (argc, argv, &puerto);

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      perror (" apertura de socket ");
      exit (1);
    }

  if (bind (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
    {
      fprintf (stderr, "Proceso %d: ", getpid ());
      perror ("ligadura");
      exit (1);
    }

  printf ("\nProceso: %d - socket disponible: %d\n", getpid (), ntohs (serv_addr.sin_port));

  listen (sockfd, 5);
  clilen = sizeof (cli_addr);

  key_t key;
  if (create_mq (&msqid, &key, "./Server") != 0)
    {
      fprintf (stderr, "Error al crear la cola de mensajes\n");
      return 1;
    }

//  auth_service_init();
//  file_service_init();

  service_init(AUTH_SID);
  service_init (FILE_SID);

  struct msgbuf msg;

  snprintf (msg.mtext, sizeof (msg.mtext), "TestForAuth");
  msg.mtype = MSG_AUTH_OUT;
  mq_send (msg, msqid);

  msg.mtype = MSG_FILE_OUT;
  snprintf (msg.mtext, sizeof (msg.mtext), "TestForFile");
  mq_send (msg, msqid);

  printf ("\nEndConfig\n\n");
  return msqid;
}

/*!
 *
 * @param argc
 * @param argv
 * @param puerto
 */

void address_conversion (int argc, char **argv, __uint16_t *puerto)
{
  int32_t s;
  serv_addr.sin_family = DEFAULT_DOMAIN;
  serv_addr.sin_addr.s_addr = DEFAULT_ADDR;

  struct addrinfo *result;    //to store results
  struct addrinfo hints;      //to indicate information we want

  hints.ai_family = AF_INET; //we only want IPv4 addresses

  if (argc > 3)
    {
      fprintf (stderr, "Too many arguments");
      exit (EXIT_FAILURE);
    }
  else if (argc == 3)
    {
      if ((s = getaddrinfo (argv[1], NULL, &hints, &result)) != 0)
        {
          fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
          exit (EXIT_FAILURE);

        }

      if (inet_aton (result->ai_addr->sa_data, &serv_addr.sin_addr))
        {
          fprintf (stderr, "Invalid address\n");
          exit (EXIT_FAILURE);
        }
      printf ("direccion %d \n", serv_addr.sin_addr.s_addr);
      *puerto = (u_int16_t) atoi (argv[2]);
    }
  else if (argc == 2)
    {
      serv_addr.sin_addr.s_addr = DEFAULT_ADDR;
      printf ("direccion %d \n", serv_addr.sin_addr.s_addr);
      *puerto = (u_int16_t) atoi (argv[1]);
    }
  else if (argc < 2)
    {
      serv_addr.sin_addr.s_addr = DEFAULT_ADDR;
      *puerto = DEFAULT_PORT;
    }

  serv_addr.sin_port = htons (*puerto);
}

/*!
 *
 * @param msqid
 */
void mainloop ()
{
  while (keep_going)
    {
      int32_t newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);//blocking
      if (newsockfd < 0)
        {
          perror ("accept");
          exit (1);
        }

      if (log_in ("3", newsockfd) == -1)
        {
          close (newsockfd);
          continue;
        }

      attend_client (newsockfd);
      close (newsockfd);
    }

}

void attend_client (int32_t newsockfd)
{
  char input[TAM];
  char cpy[TAM];
  char response[TAM];
  response[0] = '\0';
  char *prompt = "Ingrese comando: ";
  char *comm;

  while (keep_going)
    {
      struct msgbuf msg;
      read_socket (input, newsockfd);
      if (input[0] == '\0')
        {
          snprintf (response, TAM, "%s", prompt);
          long n = send (newsockfd, response, TAM, 0);
          if (n < 0)
            {
              perror ("escritura en socket");
              exit (1);
            }
          continue;
        }
      strcpy (cpy, input);
      comm = strtok (cpy, " ");

      if (strcmp (comm, "user") == 0)
        {
          msg.mtype = MSG_AUTH_OUT;
          snprintf (msg.mtext, sizeof (msg.mtext), "%s", input);
          mq_send (msg, msqid);

          if (mq_recv (msqid, &msg, MSG_AUTH_IN) != 0)
            {
              fprintf (stderr, "Error en comando\n");
              continue;
            }
          if (strcmp (msg.mtext, "not-ok") == 0)
            {
              printf ("%s comando fallida: ", PROCES_NAME);
              printf ("%s\n", msg.mtext);
              snprintf (response, TAM, "Comando fallido\n%s", prompt);
              long n = send (newsockfd, response, TAM, 0);
              if (n < 0)
                {
                  perror ("escritura en socket");
                  exit (1);
                }
              continue;
            }
          else
            {
              printf ("%s: comando exitoso\n", PROCES_NAME);
              snprintf (response, TAM, "%s\n%s", msg.mtext, prompt);
              long n = send (newsockfd, response, TAM, 0);
              if (n < 0)
                {
                  perror ("escritura en socket");
                  exit (1);
                }
              continue;
            }
        }
      else if (strcmp (comm, "file") == 0)
        {
          msg.mtype = MSG_FILE_OUT;
          snprintf (msg.mtext, sizeof (msg.mtext), "%s", input);
          mq_send (msg, msqid);

          if (mq_recv (msqid, &msg, MSG_FILE_IN) != 0)
            {
              fprintf (stderr, "Error en comando\n");
              continue;
            }

          if (strcmp (msg.mtext, "not-ok") == 0)
            {
              printf ("%s comando fallido: ", PROCES_NAME);
              printf ("%s\n", msg.mtext);
              snprintf (response, TAM, "Comando fallido\n%s", prompt);
              long n = send (newsockfd, response, TAM, 0);
              if (n < 0)
                {
                  perror ("escritura en socket");
                  exit (1);
                }
              continue;
            }
          else
            {
              printf ("%s: comando exitoso\n", PROCES_NAME);
              snprintf (response, TAM, "%s\n%s", msg.mtext, prompt);
              long n = send (newsockfd, response, TAM, 0);
              if (n < 0)
                {
                  perror ("escritura en socket");
                  exit (1);
                }
              continue;
            }

        }
        // Verificación de si hay que terminar
      else if (!strcmp ("exit", comm))
        {
          printf ("PROCESO %d. Como recibí 'exit', termino la conexion\n\n", getpid ());
          snprintf (msg.mtext, sizeof (msg.mtext), "%s", comm);
          msg.mtype = MSG_AUTH_OUT;
          mq_send (msg, msqid);
          break;
        }
      snprintf (response, TAM, "%s", prompt);
      long n = send (newsockfd, response, TAM, 0);
      if (n < 0)
        {
          perror ("escritura en socket");
          exit (1);
        }
    }
}

/*!
 *
 * @param buffer
 */
void read_socket (char *buffer, int32_t newsockfd)
{
  long n;
//    printf("%s", buffer);

  n = recv (newsockfd, buffer, TAM - 1, 0);

  if (n < 0)
    {
      perror ("lectura de socket");
      exit (1);
    }
  else
    {
      buffer[n] = '\0';
    }

  printf ("%s: se recibio peticion: %s\n", PROCES_NAME, buffer);

}

int32_t log_in (char *userID, int32_t newsockfd)
{

  char buffer[TAM];
  struct msgbuf msg;
  char user[100];
  char password[100];
  int32_t checked = 0;

  printf ("%s Nuevo cliente - Inicia el proceso de autenticacion\n", PROCES_NAME);

  for (int i = 0; i < 3; i++)
    {

      memset (buffer, 0, TAM);
      if (read (newsockfd, buffer, TAM - 1) < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      if (strcmp (buffer, "exit") == 0)
        {

        }

      snprintf (user, sizeof (user), "%s", buffer);
      printf ("%s: recibi usuario\n", PROCES_NAME);

      if (send (newsockfd, "ok", 18, 0) < 0)
        {
          perror ("escritura en socket");
          exit (1);
        }

      if (read (newsockfd, buffer, TAM - 1) < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      if (strcmp (buffer, "exit") == 0)
        {

        }

      snprintf (password, sizeof (password), "%s", buffer);
      printf ("%s: recibi contrasena\n", PROCES_NAME);

      if (send (newsockfd, "ok", 18, 0) < 0)
        {
          perror ("escritura en socket");
          exit (1);
        }

      msg.mtype = MSG_AUTH_OUT;
      snprintf (msg.mtext, sizeof (msg.mtext), "%s", user);
      mq_send (msg, msqid);
      snprintf (msg.mtext, sizeof (msg.mtext), "%s", password);
      mq_send (msg, msqid);
//  printf("%s: envie datos mtext: %s\n\n",PROCES_NAME, msg.mtext);


      if (mq_recv (msqid, &msg, MSG_AUTH_IN) != 0)
        {
          fprintf (stderr, "Error en Autenticacion\n");
          return -1;
        }

      if (strcmp (msg.mtext, "ok") != 0)
        {
          printf ("%s Autenticacion fallida: ", PROCES_NAME);
          printf ("%s\n", msg.mtext);
          continue;
        }
      else
        {
          printf ("%s: Autenticacion exitosa\n", PROCES_NAME);
          checked = 1;
          break;
        }

    }

  if (checked)
    {
      return 0;
    }
  blockUser (userID);
  return -1;
}

void blockUser (char *id)
{
  printf ("%s", id);
  return;
}

void service_init (int8_t serv)
{
  char *path;
  char *serv_str;
  char sockfd_str[32];
  sockfd_str[0] = '\0';

  if (serv == FILE_SID)
    {
      path = FILE_PATH;
      serv_str = "Archivos";
      snprintf(sockfd_str, sizeof(sockfd_str),"%d", sockfd);
    }
  else
    {
      path = AUTH_PATH;
      serv_str = "Autenticacion";
    }

  int pid = fork ();
  if (pid < 0)
    {
      perror ("fork : Service_init process creation");
      exit (1);
    }

  if (pid == 0)
    {  // Proceso hijo

//      if (serv == FILE_SID)
//        {
////          close();
//
//        }
//      else
//        {
//          path = AUTH_PATH;
//          serv_str = "Autenticacion";
//          snprintf(sockfd_str,sizeof(sockfd_str),'\0');
//        }


      char *argv[] = {path, "./Server",sockfd_str, (char *) NULL};
      if (execv (argv[0], argv) == -1)
        {
          perror ("execv: ");
          exit (1);
        }

    }
  else
    {
      if (serv == FILE_SID)
        {
          File_PID = pid;
        }
      else
        {
          Auth_PID = pid;
        }
      printf ("%s: Se inicio el Servicio de %s, proceso hijo:  %d\n", PROCES_NAME, serv_str, pid);
    }
}
