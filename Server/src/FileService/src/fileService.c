//
// Created by lfiss on 28/3/20.
//




#include "fileService.h"
//#include "../src/lib/MessageQueue/messageQueue.h"
#include "../../../../lib/MessageQueue/messageQueue.h"
#include "../../../../lib/Util/util.h"
#include "../../../../lib/Directory/dir.h"
#include "../../../../lib/Hash/hash.h"
#include "../../../../lib/MD5/MD5.h"

#include <netinet/in.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/prctl.h>
#include <sys/signal.h>
#include <fcntl.h>


#define MSG_IN 20
#define MSG_OUT 40

int config (char *filePath, char *sockfd_str);
void file_ls (char **file_list);
int send_file (char **file_List, char *filecode);
int8_t handle_req ();
void mainloop ();

int sockfd;
int msqid;

volatile sig_atomic_t keep_going = 1;
void handle_sigint(int sig)
{
  printf("File handler\n");
  keep_going = 0;
}

int main (int argc, char *argv[])
{
  struct sigaction ctrl_c;
  memset(&ctrl_c,0,sizeof(ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction(SIGINT, &ctrl_c, NULL);

  if (argc != 3)
    {
      fprintf (stderr, "%s: Cantidad incorrecta de argumentos", PROCES_NAME);
      exit (EXIT_FAILURE);
    }
  printf ("\nProceso %d: FileService Iniciado\n", getpid ());

  msqid = config (argv[1], argv[2]);

  mainloop (msqid);

}

int config (char *filePath, char *sockfd_str)
{
  sockfd = (int) strtol (sockfd_str, NULL, 10);
  prctl (PR_SET_PDEATHSIG, SIGINT);
  key_t key;
  if (create_mq (&msqid, &key, filePath) != 0)
    {
      fprintf (stderr, "Error al crear la cola de mensajes\n");
      return 1;
    }

  struct msgbuf msg;

  mq_recv(msqid, &msg, MSG_IN);

  printf ("%s : recibi %s\n", PROCES_NAME, msg.mtext);

  return msqid;

}

void mainloop ()
{
  while (keep_going)
    {
      int8_t status = handle_req ();
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

int8_t handle_req ()
{
  char request[200];
  struct msgbuf msg;
  char **file_list = list_files ("../iso");
  while (keep_going)
    {
      if (mq_recv (msqid, &msg, MSG_IN) != 0)
        {
          fprintf (stderr, "%s: Error recibiendo del usuario\n", PROCES_NAME);
          return -1;
        }
      snprintf (request, sizeof (request), "%s", msg.mtext);
      char **parsed = split_line (request);

      if (strcmp (parsed[1], FILE_LS) == 0)
        {
          file_ls (file_list);
        }

      if (!strcmp (parsed[1], FILE_DOWN))
        {
          send_file (file_list, parsed[2]);

        }

      if (!strcmp (request, "exit"))
        {
          return 2;
        }
    }

  return 0;
}

void file_ls (char **file_list)
{
//  printf("me pidieron lista de archivos");
  struct msgbuf msg;
  char code[12];
  msg.mtype = MSG_OUT;
  snprintf (msg.mtext, sizeof (msg.mtext), "CODE\tFILE NAME\n");
  int i = 0;
  while (1)
    {
      if (file_list[i] == NULL)
        break;
      else
        {
          snprintf (code, 12, "%d\t", i);
          strcat (msg.mtext, code);
          strcat (msg.mtext, file_list[i]);
          strcat (msg.mtext, "\n");
        }
      i++;
    }
  mq_send (msg, msqid);
}

#define PORT_NUMBER     5000
#define SERVER_ADDRESS  INADDR_ANY
#define FILE_TO_SEND    "hello.c"

int send_file (char **file_List, char *filecode)
{
  unsigned char *hash;
  struct msgbuf msg;
  msg.mtype = MSG_OUT;
  snprintf (msg.mtext, sizeof (msg.mtext), "Starting download...");
  mq_send (msg, msqid);
  int peer_socket;
  socklen_t sock_len;
  ssize_t len;
  struct sockaddr_in peer_addr;
  int fd;
  long sent_bytes = 0;
  char file_size[256];
  struct stat file_stat;
  off_t offset;
  long remain_data;
  char pathToFile[256];
  char buffer [BUFSIZ];
  long n;
  snprintf (pathToFile, sizeof (pathToFile), "../iso/%s", file_List[strtol (filecode, NULL, 10)]);

  fd = open (pathToFile, O_RDONLY);
  if (fd == -1)
    {
      fprintf (stderr, "Error opening file --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }

  hash = md5(pathToFile);

  /* Get file stats */
  if (fstat (fd, &file_stat) < 0)
    {
      fprintf (stderr, "Error fstat --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }

  fprintf (stdout, "File Size: \n%ld bytes\n", file_stat.st_size);


  sock_len = sizeof (struct sockaddr_in);




  /* Accepting incoming peers */
  peer_socket = accept (sockfd, (struct sockaddr *) &peer_addr, &sock_len);
  if (peer_socket == -1)
    {
      fprintf (stderr, "Error on accept --> %s", strerror (errno));

      exit (EXIT_FAILURE);
    }
  fprintf (stdout, "Accept peer --> %s\n", inet_ntoa (peer_addr.sin_addr));

  sprintf (file_size, "%ld", file_stat.st_size);



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
    if(!strcmp(buffer,"ACK")==0){

    }

  len = send (peer_socket, hash, BUFSIZ, 0);
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
  if(!strcmp(buffer,"ACK")==0){

    }
  offset = 0;
  remain_data = file_stat.st_size;
  /* Sending file data */
  while (((sent_bytes = sendfile (peer_socket, fd,  &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
      fprintf (stdout, "1. FileService sent %ld bytes from file's data, offset is now : %ld and remaining data = %ld\n", sent_bytes, offset, remain_data);
      remain_data -= sent_bytes;
      fprintf (stdout, "2. FileService sent %ld bytes from file's data, offset is now : %ld and remaining data = %ld\n", sent_bytes, offset, remain_data);
    }


  printf("shutdown");
  shutdown (peer_socket, 2);
//  shutdown(server_socket,2);

  return 0;

}
