//
// Created by lfiss on 21/4/20.
//

#include "messageQueue.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>

int create_mq(int *msqid , key_t *key, char *filePath){

  if ((*key = ftok(filePath, 'B')) == -1) {
      perror("ftok");
      return EXIT_FAILURE;
    }

  if ((*msqid = msgget(*key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      return EXIT_FAILURE;
    }
//  printf("%s: message queue: ready to send messages.\n",PROCES_NAME_MQ);
  return EXIT_SUCCESS;

}

int mq_send(struct msgbuf msg, int msqid){

  unsigned long len = strlen(msg.mtext);
  /* remove newline at end, if it exists */
  if (msg.mtext[len-1] == '\n')
    msg.mtext[len-1] = '\0';

//  printf("Send:\n Message: %s Length: %lu, Type: %ld\n\n",msg.mtext, len,msg.mtype);
  if (msgsnd(msqid, &msg, len, 0) == -1) { /* +1 for '\0' */
      perror("msgsnd");
      fprintf(stderr,"%s",msg.mtext);
      return 1;
    }
  return 0;
}

int remove_mq(int msqid){
  if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


int mq_recv(int msqid, struct msgbuf *msg, long mtype) { //blocking
  ssize_t bytes_written = msgrcv(msqid, msg, sizeof(msg->mtext), mtype, 0);
  if ( bytes_written== -1) {
      perror("msgrcv");
      return 1;
    }
  msg->mtext[bytes_written] = '\0';
  return 0;
}
