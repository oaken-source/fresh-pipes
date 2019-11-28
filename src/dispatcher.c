
#include "dispatcher.h"

#include "fresh.h"
#include "util.h"
#include "builtin.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static int
dispatch_perform_redirects (struct sh_command c)
{
  size_t i;
  for (i = 0; i < c.nredirects; ++i)
    {
      struct sh_redirect r = c.redirects[i];

      /* TODO:
       *   Hier müssen die Umleitungen von Standardein- und Ausgabe auf Dateien implementiert
       *   werden. Das Bedeutet:
       *
       *    - Datei öffnen (sh_redirect.filename)
       *    - im korrekten Modus (O_RDONLY / O_WRONLY / O_CREAT / O_TRUNC / O_APPEND / ...)
       *      siehe auch `man 2 open`.
       *        '<': sh_redirect.type == IN
       *        '>': sh_redirect.type == OUT
       *        '>>': sh_redirect.type == APPOUT
       *    - File Descriptor auf korrekten Standard Deskriptor duplizieren (dup / dup2)
       *    - überschüssige File Deskriptoren schließen
       */
    }

  return 0;
}

static int
dispatch_builtin_unforked (builtin_func_t builtin, struct sh_command c)
{
  /* Das Builtin wird nicht in einem Kindprozess ausgeführt, wir müssen also
   * im Falle von Redirects die originalen File Deskriptoren sichern und nach
   * Ende des Builtins wiederherstellen.
   */

  dup2(0, 10);
  dup2(1, 11);

  int res = dispatch_perform_redirects(c);
  if (res != 0)
    res = builtin(c.nwords, c.words);

  dup2(10, 0);
  dup2(11, 1);

  close(10);
  close(11);

  return res;
}

static void
dispatch_builtin_or_exec (struct sh_command c)
{
  builtin_func_t builtin = get_builtin(c.words[0]);
  if (builtin != NULL)
    {
      int res = builtin(c.nwords, c.words);
      exit(res);
    }

  execvp(c.words[0], c.words);

  fprintf(stderr, "%s: %s: ", program_name, c.words[0]);
  perror("");
  exit(2);
}

static void
dispatch_pipe_fork (struct sh_pipe p)
{
  struct sh_command c = p.commands[p.ncommands - 1];
  p.ncommands--;

  int res = dispatch_perform_redirects(c);

  /* Solange in der Pipeline weitere Prozesse folgen, erzeugen wir rekursiv mit
   * pipe() ein Paar File Deskriptoren, die wir mit dup() oder dup2() auf die
   * Standardein- bzw. -Ausgabe umleiten. Überschüssige File Deskriptoren können
   * mit close() geschlossen werden.
   */

  if (p.ncommands > 0)
    {
      int fd[2];
      pipe(fd);

      int pid = fork();
      if (pid == 0)
        {
          /* TODO: im Kindprozess muss das schreibende Ende der Pipe mit der Standardausgabe
           *       verbunden werden:  fd[1] --> 1
           */

          dispatch_pipe_fork(p);
        }

     /* TODO: im Elternprozess muss das lesende Ende der Pipe mit der Standardeingabe
      *       verbunden werden:  fd[0] --> 0
      */
    }

  if (res != 0)
    exit(res);

  dispatch_builtin_or_exec(c);
}

static int
dispatch_pipeline (struct sh_pipe p)
{
  /* Spezialfall: Falls der Ausdruck nur ein Kommando enthält,
   * UND wir ein builtin ausführen, wird nicht geforkt.
   */

  if (p.ncommands == 1)
    {
      struct sh_command c = p.commands[0];
      builtin_func_t builtin = get_builtin(c.words[0]);

      if (builtin != NULL)
        return dispatch_builtin_unforked(builtin, c);
    }

  /* Andernfalls bauen wir die Pipeline von HINTEN nach VORNE mit
   * Kindprozessen und Pipes auf, und warten bis der Prozess am
   * Ende der Pipe terminiert.
   */

  int pid = fork();
  if (pid == 0)
    dispatch_pipe_fork(p);

  waitpid(pid, NULL, 0);

  return 0;
}

void
dispatch (struct sh_program p)
{
  /* Arbeite nacheinander alle Audrücke in der geparsten Eingabe ab.
   * Aufgrunddessen, dass readline genau eine Zeile Text liefert, sollte
   * diese Liste höchstens einen Eintrag haben.
   */

  size_t i;
  for (i = 0; i < p.npipes; ++i)
    dispatch_pipeline(p.pipes[i]);
}
