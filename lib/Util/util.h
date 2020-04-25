//
// Created by lfiss on 19/4/20.
//

#ifndef _UTIL_H_
#define _UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a\0"



char **split_line(char *line);

#endif //_UTIL_H_
