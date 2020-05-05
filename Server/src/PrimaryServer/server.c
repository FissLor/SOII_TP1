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

#include "server.h"

#include "../../../lib/MessageQueue/messageQueue.h"

int32_t sockfd;
u_int32_t clilen;

struct sockaddr_in serv_addr, cli_addr;///< Estructuras para representar las direcciones de Cliente y Servidor
int msqid;///< Identificador de la cola de mensajes



void respond (int newsockfd, char *service_msg);
void send_help (int newsockfd);


volatile sig_atomic_t keep_going = 1;
volatile sig_atomic_t Auth_PID;
volatile sig_atomic_t File_PID;

void handle_sigint (int sig)
{

  kill (Auth_PID, sig);
  kill (File_PID, sig);

  shutdown (sockfd, 2);
  printf ("Se cerro el socket\n");
  if (msgctl (msqid, IPC_RMID, NULL) == -1)
    perror ("Eliminando cola : ");
  printf ("Se cerro la cola de mensajes\n");

//    sleep(1);
  keep_going = 0;
}

int32_t main (int32_t argc, char *argv[])
{
  struct sigaction ctrl_c; ///< Estructura para hacer un manejo de las senales SIGINT
  memset (&ctrl_c, 0, sizeof (ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction (SIGINT, &ctrl_c, NULL);

  msqid = config (argc, argv);

  mainloop (msqid);

  return 0;
}

int config (int argc, char *argv[])
{

  printf ("\n**********Config**********\n");
  u_int16_t puerto;

  //#####################Socket################################

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

  if (create_mq (&msqid, "./Server") != 0)
    {
      fprintf (stderr, "Error al crear la cola de mensajes\n");
      return 1;
    }

  service_init (AUTH_SID);
  service_init (FILE_SID);

  printf ("\nEndConfig\n\n");
  return msqid;
}

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
      *puerto = (u_int16_t) strtol (argv[2],NULL,10);
    }
  else if (argc == 2)
    {
      serv_addr.sin_addr.s_addr = DEFAULT_ADDR;
      printf ("direccion %d \n", serv_addr.sin_addr.s_addr);
      *puerto = (u_int16_t) strtol (argv[1],NULL, 10);
    }
  else if (argc < 2)
    {
      serv_addr.sin_addr.s_addr = DEFAULT_ADDR;
      *puerto = DEFAULT_PORT;
    }

  serv_addr.sin_port = htons (*puerto);
}

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

      if (log_in (newsockfd) == -1)
        {
          close (newsockfd);
          continue;
        }
      send_help (newsockfd);
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
      msgbuf msg;
      long n = recv (newsockfd, input, TAM - 1, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      else
          input[n] = '\0';


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
      strncpy (cpy, input, sizeof (input));
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
          respond (newsockfd, msg.mtext);
          continue;
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
          respond (newsockfd, msg.mtext);
          continue;
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

int32_t log_in (int32_t newsockfd)
{
  char buffer[TAM];
  msgbuf msg;
  char user[100];
  char password[100];
  int32_t checked = 0;

  printf ("%s Nuevo cliente - Inicia el proceso de autenticacion\n", PROCES_NAME);

  for (int i = 0; i < 3; i++)
    {

      long n;
      n = recv (newsockfd, buffer, TAM - 1, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      buffer[n] = '\0';
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
      n = recv (newsockfd, buffer, TAM - 1, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
        buffer[n] = '\0';
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
          if (send (newsockfd, msg.mtext, sizeof (msg.mtext), 0) < 0)
            {
              perror ("escritura en socket");
              exit (1);
            }
          recv (newsockfd, buffer, TAM, 0);
          continue;
        }
      else
        {
          printf ("%s: Autenticacion exitosa\n", PROCES_NAME);
          checked = 1;
          if (send (newsockfd, "Autenticacion exitosa", TAM, 0) < 0)
            {
              perror ("escritura en socket");
              exit (1);
            }
          recv (newsockfd, buffer, TAM, 0);

          break;
        }

    }

  if (checked)
    {
      return 0;
    }
  return -1;
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
      snprintf (sockfd_str, sizeof (sockfd_str), "%d", sockfd);
    }
  else
    {
      path = AUTH_PATH;
      serv_str = "Autenticacion";
      sockfd_str[0] = '\0';
    }

  int pid = fork ();
  if (pid < 0)
    {
      perror ("fork : Service_init process creation");
      exit (1);
    }

  if (pid == 0)
    {  // Proceso hijo

      if (serv == FILE_SID)
        {
        }
      else
        {
          close (sockfd);
        }

      char *argv[] = {path, "./Server", sockfd_str, (char *) NULL};
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

void respond (int newsockfd, char *service_msg)
{
  char response[TAM];
  char *prompt = "Ingrese comando: ";

  if (strcmp (service_msg, "not-ok") == 0)
    {
      printf ("%s comando fallida: ", PROCES_NAME);
      printf ("%s\n", service_msg);
      snprintf (response, TAM, "Comando fallido\n%s", prompt);
      long n = send (newsockfd, response, TAM, 0);
      if (n < 0)
        {
          perror ("escritura en socket");
          exit (1);
        }
    }
  else
    {
      printf ("%s: comando exitoso\n", PROCES_NAME);
      snprintf (response, TAM, "%s\n%s", service_msg, prompt);
      long n = send (newsockfd, response, TAM, 0);
      if (n < 0)
        {
          perror ("escritura en socket");
          exit (1);
        }
    }
}

void send_help (int newsockfd)
{
  char help[2048];
  sprintf (help, "Comandos:\n"
                 "user ls                   Lista los usuarios, su estado y ultima conexion\n"
                 "user passwd [new_pass]    Cambia la contrase;a actual por new_pass\n"
                 "file ls                   Lista las imagenes booteables disponibles para descarga junto a su codigo\n"
                 "file down [code]          Descarga la imagen especificada por code\n");
  long n = send (newsockfd, help, strlen (help), 0);
  if (n < 0)
    {
      perror ("escritura en socket");
      exit (1);
    }
}
