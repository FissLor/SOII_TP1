//
// Created by lfiss on 28/3/20.
//

#include <netinet/in.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/prctl.h>
#include <sys/signal.h>
#include <fcntl.h>

#include "../../../lib/MessageQueue/messageQueue.h"
#include "../../../lib/Util/util.h"
#include "../../../lib/File_DB/File_DB.h"

#include "fileService.h"



int sockfd;
int msqid;

volatile sig_atomic_t keep_going = 1;
void handle_sigint (int sig)
{
  printf ("File handler\n");
  keep_going = 0;
}

int main (int argc, char *argv[])
{
//  sleep (10);
  struct sigaction ctrl_c;
  memset (&ctrl_c, 0, sizeof (ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction (SIGINT, &ctrl_c, NULL);

  if (argc != 3)
    {
      fprintf (stderr, "%s: Cantidad incorrecta de argumentos", PROCES_NAME);
      exit (EXIT_FAILURE);
    }

  File_DB database = {NULL, NULL, NULL, NULL, NULL, 0};
  msqid = config (argv[1], argv[2], &database);

  printf ("\nProceso %d: FileService Iniciado\n", getpid ());
  mainloop (database);

  free(database.file_size);
  free(database.file_size_str);
  free(database.file_list);
  free(database.file_paths);

}

int config (char *filePath, char *sockfd_str, File_DB *database_ptr)
{
  sockfd = (int) strtol (sockfd_str, NULL, 10);
  prctl (PR_SET_PDEATHSIG, SIGINT);

  if (create_mq (&msqid, filePath) != 0)
    {
      fprintf (stderr, "Error al crear la cola de mensajes\n");
      return 1;
    }
  get_files_data ("../iso", database_ptr);

  return msqid;

}

void mainloop (File_DB database)
{
  while (keep_going)
    {
      int8_t status = handle_req (database);
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

int8_t handle_req (File_DB database)
{
  char request[MSG_SIZE];
  msgbuf msg;
//  char **file_list = list_files ("../iso");
  while (keep_going)
    {
      if (mq_recv (msqid, &msg, MSG_IN) != 0)
        {
          fprintf (stderr, "%s: Error recibiendo del usuario\n", PROCES_NAME);
          return -1;
        }
      snprintf (request, sizeof (request), "%s", msg.mtext);
      char **parsed = split_line (request);
      if (!strcmp (request, "exit"))
        {
          return 2;
        }
      else if (strcmp (parsed[1], FILE_LS) == 0)
        {
          file_ls (database);
        }

      else if (!strcmp (parsed[1], FILE_DOWN))
        {
          send_file (database, parsed[2]);

        }

      else
        {
          msg.mtype = MSG_OUT;
          snprintf (msg.mtext, sizeof (msg.mtext), "not-ok");
          mq_send (msg, msqid);
        }
    }

  return 0;
}

void file_ls (File_DB database)
{
//  printf("me pidieron lista de archivos");
  msgbuf msg;
  msg.mtype = MSG_OUT;
  snprintf (msg.mtext, sizeof (msg.mtext), "CODE\tFILE NAME\t\t\tSIZE\t\tHASH\n");

  for (u_int32_t i = 0; i < database.file_count; i++)
    {

      snprintf (msg.mtext + strlen (msg.mtext), 12, "%d\t", i);
      sprintf (msg.mtext + strlen (msg.mtext), "%-24s\t", database.file_list[i]);
      sprintf (msg.mtext + strlen (msg.mtext), "%s\t\t", (database.file_size_str[i]));
      for (int j = 0; j < 16; j++)
        sprintf (msg.mtext + strlen (msg.mtext), "%02x", database.hash[i][j]);

      strcat (msg.mtext, "\n");

    }

  mq_send (msg, msqid);
}



int send_file (File_DB database, char *filecode)
{
  msgbuf msg;
  msg.mtype = MSG_OUT;
  snprintf (msg.mtext, sizeof (msg.mtext), "Starting download...");
  mq_send (msg, msqid);
  int peer_socket;
  socklen_t sock_len;
  ssize_t len;
  struct sockaddr_in peer_addr;
  int fd;
  long sent_bytes;
  char file_size[256];
  off_t offset;
  long remain_data;
  char pathToFile[256];
  char buffer[BUFSIZ];
  long n;
  long code = strtol (filecode, NULL, 10);
  snprintf (pathToFile, sizeof (pathToFile), "../iso/%s", database.file_list[code]);

  fd = open (pathToFile, O_RDONLY);
  if (fd == -1)
    {
      fprintf (stderr, "Error opening file --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }

  fprintf (stdout, "File Size: \n%ld bytes\n", database.file_size[code]);

  sock_len = sizeof (struct sockaddr_in);


  /* Accepting incoming peers */
  peer_socket = accept (sockfd, (struct sockaddr *) &peer_addr, &sock_len);
  if (peer_socket == -1)
    {
      fprintf (stderr, "Error on accept --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }
  fprintf (stdout, "Accept peer --> %s\n", inet_ntoa (peer_addr.sin_addr));

  sprintf (file_size, "%ld", database.file_size[code]);



  /* Sending file size */
  len = send (peer_socket, file_size, sizeof (file_size), 0);
  if (len < 0)
    {
      fprintf (stderr, "Error on sending greetings --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }
  fprintf (stdout, "Server sent %zd bytes for the size\n", len);

  n = recv (peer_socket, buffer, BUFSIZ, 0);
  if (n < 0)
    {
      perror ("lectura de socket");
      exit (1);
    }
  else
    {
      buffer[n] = '\0';
    }
  if (strcmp (buffer, "ACK") != 0)
    {

    }

  len = send (peer_socket, database.hash[code], BUFSIZ, 0);
  if (len < 0)
    {
      fprintf (stderr, "Error on sending greetings --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }
  fprintf (stdout, "Server sent %zd bytes for the hash\n", len);

  n = recv (peer_socket, buffer, BUFSIZ, 0);
  if (n < 0)
    {
      perror ("lectura de socket");
      exit (1);
    }
  else
    {
      buffer[n] = '\0';
    }
  if (strcmp (buffer, "ACK") != 0)
    {

    }
  offset = 0;
  remain_data = database.file_size[code];
  /* Sending file data */
  while (((sent_bytes = sendfile (peer_socket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
      remain_data -= sent_bytes;
      printf ("\rProgreso %ld%%", (database.file_size[code] - remain_data) * 100 / database.file_size[code]);
      fflush (stdout);

    }

  printf ("\n");
  printf ("shutdown");
  shutdown (peer_socket, 2);
//  shutdown(server_socket,2);

  return 0;

}
