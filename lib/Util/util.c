//
// Created by lfiss on 21/4/20.
//
#include "util.h"

char **split_line (char *line)
{
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc ((unsigned long) bufsize * sizeof (char *));
  char *token;

  if (!tokens)
    {
      fprintf (stderr, "lsh: allocation error\n");
      exit (EXIT_FAILURE);
    }

  token = strtok (line, TOK_DELIM);
  while (token != NULL)
    {
      tokens[position] = token;
      position++;

      if (position >= bufsize)
        {
          bufsize += TOK_BUFSIZE;
          tokens = realloc (tokens, (unsigned long) bufsize * sizeof (char *));
          if (!tokens)
            {
              fprintf (stderr, "lsh: allocation error\n");
              exit (EXIT_FAILURE);
            }
        }

      token = strtok (NULL, TOK_DELIM);
    }
  tokens[position] = NULL;
  return tokens;
}
