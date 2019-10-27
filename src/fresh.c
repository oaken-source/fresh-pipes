
#include <stdio.h>
#include <readline/readline.h>
#include <errno.h>

#include "parser.h"
#include "builtin.h"

/* Der "Programmname", wie in der main Funktion in argv[0] übergeben */
char *program_name = NULL;

int
do_fork_wait (int argc, char *argv[])
{
  /****************************************************************************
   * fork / exec                                                              *
   * -----------                                                              *
   *                                                                          *
   *            ...                    Der fork Systemaufruf erzeugt in UNIX  *
   *             |                     einen neuen Prozess, indem er eine     *
   *             v                     Kopie des laufenden Prozesses erzeugt. *
   *      fork() * ---- x              Beide Kopien setzen ihre Ausführung    *
   *             |      |              unmittelbar nach dem fork Aufruf fort. *
   *    pid != 0 |      | pid == 0                                            *
   *             v      v              Im neu erstellten Kindprozess ist der  *
   *      wait() *      * exec(...)    Rückgabewert von fork 0, während im    *
   *            ---     |              Elternprozess die Prozess ID des neuen *
   *                   ...             Prozesses zurückgegeben wird.          *
   *                    |                                                     *
   *                    v              Führt man im Kindprozess den System-   *
   *                    * exit()       aufruf exec* aus, ersetzt man das      *
   *            ---    ---             Programm des laufenden Prozesses durch *
   *             |                     ein neu geladenes Programm. Durch die  *
   *             v                     Kombination von fork und exec kann die *
   *            ...                    shell neue Programme starten.          *
   *                                                                          *
   *  Der Systemaufruf wait im Elternprozess bewirkt, dass die shell das Ende *
   *  des neu gestarteten Prozesses abwartet, bevor die Ausführung fort-      *
   *  gesetzt wird. wait wird darüber hinaus dem Betriebssystem vermitteln,   *
   *  dass die Informationen des beendeten Kindprozesses aufgeräumt, und      *
   *  dessen Prozess ID wiederverwendet werden kann. Lesen Sie dazu auch die  *
   *  Manual-Seite `man 2 wait`.                                              *
   ****************************************************************************/

  errno = ENOSYS;
  return 1;
}

int
main (int argc, char *argv[])
{
  program_name = argv[0];

  char *input = NULL;
  size_t nwords;
  char **words = NULL;
  while (1)
    {
      input = readline("$> ");

      if (input == NULL)
        {
          puts("exit");
          break;
        }

      words = parse(input, &nwords);

      if (nwords == 0)
        continue;

      builtin_func_t builtin = get_builtin(words[0]);
      if (builtin != NULL)
        {
          builtin(nwords, words);
        }
      else
        {
          do_fork_wait(nwords, words);
        }

      free(words);
      free(input);
    }

  return 0;
}
