//
// Created by lfiss on 4/4/20.
//

#ifndef SERVER_MESSAGEQUEUE_H
#define SERVER_MESSAGEQUEUE_H

#include <zconf.h>
#ifdef PROCES_NAME
#define PROCES_NAME_MQ PROCES_NAME
#endif
#define PERMS 0644

struct msgbuf {
  long mtype;
  char mtext[200];
};


int create_mq(int *msqid , key_t *key, char *filePath);

int mq_send(struct msgbuf msg, int msqid);

int remove_mq(int msqid);

int mq_recv(int msqid, struct msgbuf *msg, long mtype); //blocking

#endif // SERVER_MESSAGEQUEUE_H
