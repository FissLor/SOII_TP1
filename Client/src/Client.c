#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>
#include <jmorecfg.h>
#include <signal.h>
#include <termios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../../lib/MD5/MD5.h"
#include "../../lib/MBR/MBR.h"

#define TAM 256
#define DEFAULT_PORT 2000
#define DEFAULT_ADDR "0.0.0.0"

int32_t sockfd = -1;

struct sockaddr_in serv_addr;

int32_t config (int argc, char *argv[]);
void mainloop ();
void receive (char *buffer);
void sendToSock (char *buffer);
void log_in (char *buffer);
int get_pass (char *password);
void download ();

/*!// Handler for SIGINT, caused by
 * Ctrl-C at keyboard
 *
 * @param sig
 */
void handle_sigint (int sig)
{
  if (sockfd != -1)
    {
      char nop[17];
      send (sockfd, "exit", 5, 0);
      receive (nop);
    }

//    sleep(1);
  exit (EXIT_SUCCESS);
}
/*!
 *
 * @param argc
 * @param argv
 * @return
 */
int32_t main (int argc, char *argv[])
{
  struct sigaction Ctrl_C;
  memset (&Ctrl_C, 0, sizeof (Ctrl_C));
  Ctrl_C.sa_handler = handle_sigint;
  Ctrl_C.sa_flags = 0;
  sigaction (SIGINT, &Ctrl_C, NULL);

  sockfd = config (argc, argv);

  mainloop (sockfd);

  return 0;
}

/*!
 *
 * @param argc
 * @param argv
 * @return
 */
int32_t config (int argc, char *argv[])
{
  mbr file_mbr;
  printf("Tamano de mbr struct: %lu",sizeof(file_mbr));
  char *host;
  uint16_t puerto;

  struct hostent *server;

  if (argc < 3)
    {
      puerto = DEFAULT_PORT;
      host = DEFAULT_ADDR;
    }
  else
    {
      puerto = (uint16_t) atoi (argv[2]);
      host = argv[1];
    }

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      perror ("ERROR apertura de socket");
      exit (1);
    }

  server = gethostbyname (host);
  if (server == NULL)
    {
      fprintf (stderr, "Error, no existe el host\n");
      exit (0);
    }

  memset ((char *) &serv_addr, '0', sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy ((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (unsigned long) server->h_length);
  serv_addr.sin_port = htons (puerto);

  if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
    {
      perror ("conexion");
      exit (1);
    }
  return sockfd;
}

/*!
 *
 */
void mainloop ()
{
  boolean terminar = FALSE;
  char buffer[TAM];

  log_in (buffer);

  while (1)
    {

//        memset( buffer, 0, TAM );
      fgets (buffer, TAM - 1, stdin);
      buffer[strcspn (buffer, "\n")] = '\0';
//        printf("buffer quedo : %s\n",buffer);
//        if(buffer[0]=='\0')
//          continue;

      sendToSock (buffer);

      // Verificando si se escribió: exit
      buffer[strlen (buffer)] = '\0';
      if (strcmp ("exit", buffer) == 0)
        {
          terminar = TRUE;
        }
      else
        {
          receive (buffer);
          printf ("%s", buffer);
          char *parsed = strtok (buffer, "\n");
          int n = strcmp (parsed, "Starting download...");
          if (n == 0)
            {
              download ();
            }

        }

      if (terminar)
        {
          printf ("Finalizando ejecución\n");
          exit (0);
        }

    }
}

/*!
 *
 * @param buffer
 */
void sendToSock (char *buffer)
{
  long n;
//    printf("entre");
  n = send (sockfd, buffer, strlen (buffer), 0);
  if (n < 0)
    {
      perror ("escritura de socket");
      exit (1);
    }
//    printf("sali");
}

/*!
 *
 * @param buffer
 * @return
 */
void receive (char *buffer)
{
  long n;

  n = recv (sockfd, buffer, TAM, 0);
  if (n < 0)
    {
      perror ("lectura de socket");
      exit (1);
    }
  buffer[n] = '\0';
//    printf( "Respuesta: %s\n", buffer );

}

int get_pass (char *password)
{
  struct termios oflags, nflags;
//    char password[64];

  /* disabling echo */
  tcgetattr (fileno (stdin), &oflags);
  nflags = oflags;
  nflags.c_lflag &= (unsigned int) ~ECHO;
  nflags.c_lflag |= ECHONL;

  if (tcsetattr (fileno (stdin), TCSANOW, &nflags) != 0)
    {
      perror ("tcsetattr");
      return EXIT_FAILURE;
    }

  fgets (password, sizeof (password), stdin);


  /* restore terminal */
  if (tcsetattr (fileno (stdin), TCSANOW, &oflags) != 0)
    {
      perror ("tcsetattr");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

void log_in (char *buffer)
{
  char *usuario, *password;

  printf ("////////// Descarga de imagenes booteables //////////\n");
  printf ("Procesos de autenticacion\n");
  printf ("Ingrese su nombre de usuario: ");
  memset (buffer, 0, TAM);
  fgets (buffer, TAM - 1, stdin);

//    buffer[strcspn(buffer,"\n")] = '\0';
  usuario = calloc (strlen (buffer), sizeof (char));
  snprintf (usuario, strlen (buffer), "%s", buffer);

  printf ("Ingrese su contrasena: ");
  memset (buffer, 0, TAM);
  if (get_pass (buffer) != 0)
    printf ("Error recuperando la contrasena");

//    buffer[strcspn(buffer,"\n")] = '\0';
  password = calloc (strlen (buffer), sizeof (char));
  snprintf (password, strlen (buffer), "%s", buffer);

  printf ("User: %s Password: %s", usuario, password);
  sendToSock (usuario);
  receive (buffer);
  sendToSock (password);
  receive (buffer);
  free (usuario);
  free (password);

}

#define FILENAME        "./file"

void download ()
{
  int down_socket;
  ssize_t len;
  char buffer[BUFSIZ];
  int file_size;
  FILE *received_file;
  int remain_data;
  char *received_data;
  unsigned char received_hash[21];
  unsigned char *calculated_hash;

  printf ("Comenzando rutina de descarga\n");
  /* Create download socket */
  down_socket = socket (AF_INET, SOCK_STREAM, 0);
  if (down_socket == -1)
    {
      fprintf (stderr, "Error creating socket --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }

  printf ("Socket para descarga creado\n");
  /* Connect to the server */
  if (connect (down_socket, (struct sockaddr *) &serv_addr, sizeof (struct sockaddr)) == -1)
    {
      fprintf (stderr, "Error on connect --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }

  printf ("Conexion establecida\n");

  /* Receiving file size */
  len = recv (down_socket, buffer, BUFSIZ, 0);
  file_size = atoi (buffer);
  fprintf (stdout, "\nFile size : %d\n", file_size);

  send(down_socket,"ACK",BUFSIZ,0);

  received_data = malloc((unsigned long) file_size);
  received_data[0] = '\0';

  /* Receiving file hash */
  len = recv (down_socket, buffer, BUFSIZ, 0);
  snprintf((char*)received_hash, (unsigned long) len, "%s", buffer);
  fprintf (stdout, "\nFile hash : %s\n", received_hash);
  send(down_socket,"ACK",BUFSIZ,0);
//0x565538a67010 "oY\002\254#p$\275\320\301v˓\006", <incomplete sequence \304>

  received_file = fopen (FILENAME, "r+");
  if (received_file == NULL)
    {
      fprintf (stderr, "Failed to open file foo --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }

  remain_data = file_size;
  int i = 0;
  while ((remain_data > 0) && ((len = recv (down_socket, buffer, BUFSIZ, 0)) > 0))
    {
      fwrite (buffer, sizeof (char), (unsigned long) len, received_file);
      remain_data -= len;
      fprintf (stdout, "Receive %zd bytes and we hope :- %d bytes\n", len, remain_data);
    }

  mbr file_mbr;


  fclose (received_file);

//  received_file = fopen (FILENAME, "r+");
//  if (received_file == NULL)
//    {
//      fprintf (stderr, "Failed to open file foo --> %s\n", strerror (errno));
//
//      exit (EXIT_FAILURE);
//    }
//
//
//  getMBR(received_file,&file_mbr);
//  printPT(&file_mbr);
//  calculated_hash = md5(FILENAME);
//
//  if(strncmp((const char*)calculated_hash,(const char*)received_hash,16)==0){
//    printf("Md5 checksum match");
//  }



  close (down_socket);
  free(received_data);
//  free(calculated_hash);

  return;
}
//
//void synchro(){
//
//  long n;
//  boolean terminar = 0;
//  char buffer[TAM];
//
//  // Verificando si se escribió: fin
//  buffer[strlen(buffer)-1] = '\0';
//  if( !strcmp( "exit", buffer ) ) {
//    terminar = TRUE;
//  }
//
//  memset( buffer, 0, TAM );
//  n = recv( sockfd, buffer, TAM,0);
//  if ( n < 0 ) {
//    perror( "lectura de socket" );
//    exit( 1 );
//  }
////    printf( "Respuesta: %s\n", buffer );
//
////  return terminar;
//
//}
