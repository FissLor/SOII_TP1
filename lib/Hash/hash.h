//
// Created by lfiss on 12/4/20.
//

#ifndef SERVER_HASH_H
#define SERVER_HASH_H

//#define _GNU_SOURCE



char *hash(char *plain_password);


int check_pass(char (*db)[64], char * checked, int usercount);
int check_user(char (*db)[64], char * checked, int usercount);

#endif // SERVER_HASH_H
