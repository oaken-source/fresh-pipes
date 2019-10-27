
#include "builtin.h"

#include <errno.h>
#include <string.h>

static int
builtin_cd (int argc, char *argv[])
{
  /* Die Fehlernummer ENOSYS beschreibt den Fehler, dass die Funktion nicht
   * implementiert ist. Entfernen Sie die Fehlerrückgabe, wenn Sie Ihre
   * Implementierung hier eintragen. */
  errno = ENOSYS;
  return 1;
}

int
builtin_exit (int argc, char *argv[])
{
  /* Die Fehlernummer ENOSYS beschreibt den Fehler, dass die Funktion nicht
   * implementiert ist. Entfernen Sie die Fehlerrückgabe, wenn Sie Ihre
   * Implementierung hier eintragen. */
  errno = ENOSYS;
  return 1;
}

struct builtin_t
{
  const char *command;
  builtin_func_t function;
};

struct builtin_t builtins[] = {
  { "cd", &builtin_cd },
  { "exit", &builtin_exit },

  /* Nach dem obigen Schema können weitere builtins hier hinzugefügt werden.
   * Diese werden automatisch von der Funktion get_builtin als solche erkannt.
   *
   * Der folgende Eintrag markiert das Ende der Liste, und muss am Ende der
   * Aufzählung stehen. */

  { 0 }
};

builtin_func_t
get_builtin (const char *command)
{
  /* Diese Funktion stellt fest, ob es sich bei einem gegeben Kommando um ein
   * builtin handelt, also um eine Funktion, die direkt von der shell
   * implementiert ist.
   *
   * Der Rückgabewert ist ein Zeiger auf eine Funktion, falls eine Überein-
   * stimmung gefunden wurde, und ansonsten NULL. */

  struct builtin_t *p = builtins;
  while (p->command != NULL)
    {
      if (!strcmp(command, p->command))
        return p->function;
      p++;
    }

  return NULL;
}

