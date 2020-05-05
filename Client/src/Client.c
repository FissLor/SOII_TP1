#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>
#include <jmorecfg.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>
#include <dirent.h>
#include "../../lib/MD5/MD5.h"
#include "../../lib/MBR/MBR.h"
#include "Client.h"

#define TAM 2048
#define DEFAULT_PORT 2000
#define DEFAULT_ADDR "0.0.0.0"

//#define DEV_BY_ID  "/dev/disk/by-id"
#define DEV_BY_ID  "."

#define ACK "ACK"

int32_t sockfd = -1;

struct sockaddr_in serv_addr;

/*!// Handler for SIGINT, caused by
 * Ctrl-C at keyboard
 *
 * @param sig
 */
void handle_sigint (int sig)
{
  if (sockfd != -1)
    {
      long n = 0;
      char buffer[TAM];
      n = send (sockfd, "exit", 5, 0);
      if (n < 0)
        {
          perror ("Escritura de socket");
          exit (1);
        }
      n = recv (sockfd, buffer, TAM, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
    }

  exit (EXIT_SUCCESS);
}

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

int32_t config (int argc, char *argv[])
{
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
      puerto = (uint16_t) strtol (argv[2], NULL, 10);
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

void mainloop ()
{
  long n;
  boolean terminar = FALSE;
  char buffer[TAM];

  log_in (buffer);

  n = recv (sockfd, buffer, TAM, 0);
  if (n < 0)
    {
      perror ("lectura de socket");
      exit (1);
    }
  buffer[n] = '\0';
  printf ("%s", buffer);

  while (1)
    {
      fgets (buffer, TAM - 1, stdin);
      buffer[strcspn (buffer, "\n")] = '\0';

      n = send (sockfd, buffer, strlen (buffer), 0);
      if (n < 0)
        {
          perror ("escritura de socket");
          exit (1);
        }

      // Verificando si se escribió: exit
//      buffer[strlen (buffer)] = '\0';
      if (strcmp ("exit", buffer) == 0)
        {
          terminar = TRUE;
        }
      else
        {
          n = recv (sockfd, buffer, TAM, 0);
          if (n < 0)
            {
              perror ("lectura de socket");
              exit (1);
            }
          buffer[n] = '\0';
          printf ("\n%s", buffer);
          char *parsed = strtok (buffer, "\n");
          n = strcmp (parsed, "Starting download...");
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

int get_pass (char *password)
{
  struct termios oflags = {0, 0, 0, 0, 0, {0, 0, 0}, 0, 0};
  struct termios nflags = {0, 0, 0, 0, 0, {0, 0, 0}, 0, 0};;

  /* Desactivando Echo */
  tcgetattr (fileno (stdin), &oflags);
  nflags = oflags;
  nflags.c_lflag &= ~((unsigned int) ECHO);
  nflags.c_lflag |= (unsigned int) ECHONL;

  if (tcsetattr (fileno (stdin), TCSANOW, &nflags) != 0)
    {
      perror ("tcsetattr");
      return EXIT_FAILURE;
    }

  fgets (password, TAM, stdin);


  /* Se recupera la salida por terminal */
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

  while (1)
    {
      long n;
      printf ("Ingrese su nombre de usuario: ");
      fgets (buffer, TAM, stdin);
//      buffer[strcspn (buffer, "\n")] = '\0';

//    buffer[strcspn(buffer,"\n")] = '\0';
      usuario = calloc (strlen (buffer), sizeof (char));
      snprintf (usuario, strlen (buffer), "%s", buffer);

      printf ("Ingrese su contrasena: ");
      if (get_pass (buffer) != 0)
        printf ("Error recuperando la contrasena");

      password = calloc (strlen (buffer), sizeof (char));
      snprintf (password, strlen (buffer), "%s", buffer);

      n = send (sockfd, usuario, strlen (usuario), 0);
      if (n < 0)
        {
          perror ("escritura de socket");
          exit (1);
        }

      n = recv (sockfd, buffer, TAM, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      buffer[n] = '\0';
      if (strcmp (buffer, "ok") != 0)
        {
          fprintf (stderr, "Error en el envio de los datos para autenticacion intentelo nuevamente\n");
          continue;
        }
      n = send (sockfd, password, strlen (password), 0);
      if (n < 0)
        {
          perror ("escritura de socket");
          exit (1);
        }
      n = recv (sockfd, buffer, TAM, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      buffer[n] = '\0';
      if (strcmp (buffer, "ok") != 0)
        {
          fprintf (stderr, "Error en el envio de los datos para autenticacion intentelo nuevamente\n");
          continue;
        }
      n = recv (sockfd, buffer, TAM, 0);
      if (n < 0)
        {
          perror ("lectura de socket");
          exit (1);
        }
      buffer[n] = '\0';
      printf ("%s\n", buffer);
      send (sockfd, ACK, 3, 0);
      if (strcmp (buffer, "Autenticacion exitosa") == 0)
        {
          break;
        }

    }
  free (usuario);
  free (password);

}

void download ()
{
  int down_socket;
  ssize_t len;
  char buffer[BUFSIZ];
  long int file_size;
  FILE *received_file;
  long remain_data;
  unsigned char received_hash[21];
  unsigned char *calculated_hash;
  long index;
  char dev_path[512];

  printf ("Comenzando rutina de descarga\n");
  /* Create download socket */
  down_socket = socket (AF_INET, SOCK_STREAM, 0);

  printf ("\nLista de usb conectados al equipo:\n");
  printf ("CODIGO\tDISPOSITIVO\n");
  char **list = list_files (DEV_BY_ID);
  int i = 0;
  while (list[i] != NULL)
    {
      printf ("%d\t%s\n", i, list[i]);
      i++;
    }
  printf ("\nVerifique por favor que el dispositivo en el que desea grabar la imagen este en la lista\n"
          "Ingrese por favor el codigo del dispositivo en el que le gustaria grabar la imagen\n"
          "ADVERTENCIA: De haber algun contenido en el dispositivo seleccionado, este se perdera \n\n");

  while (1)
    {
      printf ("CODIGO: ");
      fgets (buffer, 10, stdin);
      index = strtol (buffer, NULL, 10);
      if (index >= i)
        printf ("El codigo ingresado es incorrecto");
      else
        break;

    }

  sprintf (dev_path, "%s/%s", DEV_BY_ID, list[index]);

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
  file_size = strtol (buffer, NULL, 10);
  if (file_size > 1024)
    {
      if (file_size > (1024 * 1024))
        fprintf (stdout, "\nFile size : %ld Mb\n", file_size / 1024 / 1024);
      else
        fprintf (stdout, "\nFile size : %ld Kb\n", file_size / 1024);
    }

  send (down_socket, ACK, strlen (ACK), 0);


  /* Receiving file hash */
  len = recv (down_socket, buffer, BUFSIZ, 0);
  snprintf ((char *) received_hash, (unsigned long) len, "%s", buffer);
//  fprintf (stdout, "\nFile hash : %hhu\n", (uint8_t)received_hash);

  send (down_socket, ACK, strlen (ACK), 0);

  received_file = fopen (dev_path, "w+");
  if (received_file == NULL)
    {
      fprintf (stderr, "Failed to open file foo --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }

  remain_data = file_size;

  while ((remain_data > 0) && ((len = recv (down_socket, buffer, BUFSIZ, 0)) > 0))
    {
      fwrite (buffer, sizeof (char), (unsigned long) len, received_file);
      remain_data -= (int) len;
      printf ("\rProgreso %ld%%", ((file_size - remain_data) * 100 / file_size));
      fflush (stdout);
    }
  printf ("\n\n");

  MBR file_mbr;

  fclose (received_file);

  received_file = fopen (dev_path, "r+");
  if (received_file == NULL)
    {
      fprintf (stderr, "Failed to open file foo --> %s\n", strerror (errno));

      exit (EXIT_FAILURE);
    }

  getMBR (received_file, &file_mbr);
  printf ("Tabla de Particiones de la unidad\n");
  printPT (&file_mbr);
  fclose (received_file);

  calculated_hash = md5 (dev_path, file_size);

  if (strncmp ((const char *) calculated_hash, (const char *) received_hash, 16) == 0)
    {
      printf ("Md5 checksum match\n");
    }

  close (down_socket);
  free (calculated_hash);
  free (list);

}

int myfilter (const struct dirent *entry)
{

  if ((strcmp (entry->d_name, ".") == 0) || (strcmp (entry->d_name, "..") == 0))
    return 0; //No se guarda
  if (strstr (entry->d_name, "part") != NULL)
    return 0;
  if (strstr (entry->d_name, "usb") == NULL)
    return 0;

  return 1;//Se guarda
}

char **list_files (char *path)
{

  struct dirent **namelist;
  int n;

  n = scandir (path, &namelist, myfilter, alphasort);
  if (n == -1)
    {
      perror ("scandir");
      return NULL;
    }
  if (n == 0)
    {
      printf ("\nnada\n");
    }

  char **name_list_str = (char **) malloc ((unsigned long) n * sizeof (char *));

  for (int i = 0; i < n; i++)
    {
      name_list_str[i] = malloc (sizeof (char) * 256);
    }
//  char nameListStrr[n][256];


  for (int i = 0; i < n; i++)
    {
      name_list_str[i] = namelist[i]->d_name;
    }
  name_list_str[n] = NULL;

  return name_list_str;
}

