#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

/* Die folgenden Strukturen beschreiben den Syntax Baum, welcher vom
 * Parser aus der Eingabe erstellt wird */

enum sh_redirect_type {
  IN,
  OUT,
  APPOUT
};

struct sh_redirect {
  enum sh_redirect_type type;
  char *filename;
};

struct sh_command {
  struct sh_redirect *redirects;
  size_t nredirects;

  char **words;
  size_t nwords;
};

struct sh_pipe {
  struct sh_command *commands;
  size_t ncommands;
};

struct sh_program {
  struct sh_pipe *pipes;
  size_t npipes;
};

int parse (char *str, struct sh_program *p);

#endif
