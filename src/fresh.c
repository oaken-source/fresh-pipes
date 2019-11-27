
#include <stdio.h>
#include <errno.h>

#include "parser.h"
#include "dispatcher.h"
#include "builtin.h"

#include <readline/readline.h>
#include <readline/history.h>

/* Der "Programmname", wie in der main Funktion in argv[0] übergeben */
char *program_name = NULL;

int
main (int argc, char *argv[])
{
  program_name = argv[0];

  char *input = NULL;
  while (1)
    {
      /* readline übernimmt für uns das Anlegen eines Input Buffers */
      input = readline("$> ");

      /* NULL als Rückgabewert bedeutet, dass die Eingabe geschlossen wurde */
      if (input == NULL)
        {
          puts("exit");
          break;
        }

      /* readline übernimmt für uns auch die history */
      add_history(input);

      /* Konvertiere die Eingabe in eine Maschinenlesbare Struktur */
      struct sh_program program;
      parse(input, &program);

      /* Führe die ausgelesenen Kommandos aus */
      dispatch(program);

      free(program.pipes);
      free(input);
    }

  return 0;
}
