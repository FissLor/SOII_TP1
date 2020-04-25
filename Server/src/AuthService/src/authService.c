//
// Created by lfiss on 28/3/20.
//
#include "authService.h"
#include "../../../../lib/MessageQueue/messageQueue.h"
#include "../../../../lib/Hash/hash.h"
#include "../../../../lib/Util/util.h"

#include <sys/prctl.h>
#include <sys/signal.h>


#define USER_LS "ls"
#define USER_PASSWD "passwd"
#define MSG_IN 10
#define MSG_OUT 30

int data_fetch(char (*users)[64], char (*passw)[64]);
int config(char* filePath, char users[100][64], char passw[100][64], int *usercount);
int log_in(int msqid, char users[100][64],char passwords[100][64], int usercount);
__int8_t handle_req(int msqid,char (*users)[64],char (*passwords)[64], int usercount);
void mainloop(int msqid, char users[100][64], char passwords[100][64],int usercount);


int list_users(int msqid,char (*users)[64], int usercount) ;
int change_password(char*new_password, int msqid, char (*passwords)[64],int userID) ;

volatile sig_atomic_t keep_going = 1;
void handle_sigint(int sig)
{
  printf("Auth handler\n");
  keep_going = 0;
}


int main(int argc, char* argv[] ){
  struct sigaction ctrl_c;
  memset(&ctrl_c,0,sizeof(ctrl_c));
  ctrl_c.sa_handler = handle_sigint;
  ctrl_c.sa_flags = 0;
  sigaction(SIGINT, &ctrl_c, NULL);

//  if (argc!=2){
//      fprintf(stderr,"%s Cantidad incorrecta de argumentos", PROCES_NAME);
////      exit(EXIT_FAILURE);
//  }

  printf("\nProceso %d: AuthService Iniciado\n", getpid());

  char users[100][64];
  char passw[100][64];
  int usercount = 0;
  int msqid;
  if (argc==2)
    {
      msqid = config (argv[1], users, passw, &usercount);
    }
  else
    msqid = config ("./Server", users, passw, &usercount);
  mainloop(msqid,users,passw,usercount);
  return 0;
}


int config(char* filePath,char users[100][64], char passw[100][64], int *usercount){

  prctl(PR_SET_PDEATHSIG, SIGINT);
  int msqid;
  key_t key;
  if (create_mq(&msqid, &key, filePath)!=0){
    fprintf(stderr,"Error al crear la cola de mensajes\n");
    return 1;
  }

  struct msgbuf msg;

  memset(msg.mtext,0,sizeof(msg.mtext));
  mq_recv(msqid, &msg, MSG_IN);

  printf("%s : recibi %s\n",PROCES_NAME, msg.mtext);



  *usercount = data_fetch(users, passw);

  return msqid;
}
/*!
 *
 * @param msqid
 * @param users
 * @param passwords
 * @param usercount
 */


void mainloop(int msqid, char users[100][64], char passwords[100][64],int usercount){
  struct msgbuf msg;

  while(keep_going){
    int status;
    status =log_in(msqid,users, passwords, usercount);
    if(status==0){
      snprintf(msg.mtext,sizeof(msg.mtext), "ok");
      msg.mtype = MSG_OUT;
      mq_send(msg, msqid);
    }
    else if(status ==1){
      snprintf(msg.mtext,sizeof(msg.mtext), "WRONG USER OR PASSWORD");
      msg.mtype = MSG_OUT;
      mq_send(msg, msqid);
      continue;
    }

    status = handle_req (msqid, users, passwords, usercount);
    if (status==0){
      continue;
    }
    else if(status==2){
      exit(EXIT_SUCCESS);
    }
  }
}

__int8_t handle_req(int msqid,char (*users)[64],char (*passwords)[64], int usercount) {
  char request [200];
  struct msgbuf msg;

  while(keep_going){

    if(mq_recv(msqid,&msg,MSG_IN)!=0){
      fprintf(stderr,"%s: Error recibiendo request\n", PROCES_NAME);
      return -1;
    }

    snprintf(request,sizeof(request),"%s", msg.mtext);

    char **parsed = split_line(request);

      if(strcmp(parsed[0], "exit")==0){
          fprintf(stderr, "%s deje de recibir solicitudes", PROCES_NAME);
          return 0;
        }

    if(strcmp(parsed[1], USER_LS)==0){

      list_users(msqid,users,usercount);
        continue;
    }

    if(strcmp(parsed[1], USER_PASSWD)==0){
      change_password(parsed[2],msqid,passwords,0);
        continue;
    }


    else{
        msg.mtype = MSG_OUT;
        snprintf(msg.mtext,sizeof(msg.mtext),"not-ok");
        mq_send(msg,msqid);
    }
    int i = 0;
    while(parsed[i]!=NULL)
      {
        free (parsed[i]);
        i++;
      }
      free(parsed);
  }
  return 0;

}


int change_password(char*new_password, int msqid, char (*passwords)[64],int userID) {
//  char new_password [200];
  struct msgbuf msg;

//  memset(msg.mtext,0,sizeof(msg.mtext));
//  if(mq_recv(msqid,&msg,MSG_IN)!=0){
//    fprintf(stderr,"%s: Error recibiendo el usuario\n", PROCES_NAME);
//    return -1;
//  }
//  snprintf(new_password,sizeof(new_password),"%s",msg.mtext);

  fprintf(stderr,"%s", passwords[userID]);
  snprintf(passwords[userID],sizeof(passwords[userID]),"%s",hash(new_password));
  fprintf(stderr,"%s", passwords[userID]);
  snprintf(msg.mtext,sizeof(msg.mtext), "Password changed");
  msg.mtype = MSG_OUT;
  mq_send(msg, msqid);
  return 0;
}

int list_users(int msqid,char (*users)[64], int usercount) {
  struct msgbuf msg;
  msg.mtype = MSG_OUT;
  snprintf(msg.mtext,sizeof(msg.mtext),"Usuarios:\n");
  for (int i = 0; i<usercount;i++){
    strcat(msg.mtext,users[i]);
    strcat(msg.mtext,"\n");
  }

  mq_send(msg, msqid);

  return 0;
}

int data_fetch(char (*users)[64], char (*passw)[64]){
  FILE *dat = fopen("./dat", "r");
  if(dat == NULL){
    fprintf(stderr,"No existe archivo dat");
    return -1;
  }


  int32_t i =0;
  while(fscanf(dat, "%63s %63s",users[i], passw[i])>0){

    i = i+1;

  }
  fclose(dat);
  return i;

}



int log_in(int msqid, char users[100][64],char passwords[100][64], int usercount){
  struct msgbuf msg;

  char user[200];
  char pass[200];
  fprintf(stderr,"%s : Im logging ",PROCES_NAME);

//  memset(msg.mtext,0,sizeof(msg.mtext));

  if(mq_recv(msqid,&msg,MSG_IN)!=0){
    fprintf(stderr,"%s Error recibiendo el usuario\n", PROCES_NAME);
    return -1;
  }

  snprintf(user,sizeof(user),"%s",msg.mtext );
  fprintf(stderr,"%srecibi el usuario: %s\n",PROCES_NAME, user);

  memset(msg.mtext,0,sizeof(msg.mtext));

  if(mq_recv(msqid,&msg,MSG_IN)!=0){
    fprintf(stderr,"%s: Error recibiendo el usuario\n", PROCES_NAME);
    return -1;
  }

  snprintf(pass,sizeof(pass),"%s",msg.mtext );
  fprintf(stderr,"%s: recibi la contrasena: %s\n",PROCES_NAME, pass);

  if(check_user(users, user, usercount)){
    fprintf(stderr,"%s: Error en usuario o contrasena (1)\n", PROCES_NAME);
    return 1;
  }
  if(check_pass(passwords, pass, usercount)){
    fprintf(stderr,"%s: Error en usuario o contrasena (2)\n", PROCES_NAME);
    return 1;
  }

  return 0;
}


