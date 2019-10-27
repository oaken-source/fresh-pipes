
#include "util.h"

#include <stdio.h>
#include <stdarg.h>

int
debug (const char *format, ...)
{
  /* Diese Funktion stellt eine Senke für debug Nachrichten bereit, die in ein
   * logfile in dem Verzeichnis geschrieben werden, in dem die shell gestartet
   * wurde. */

  static FILE *logfile = NULL;
  if (logfile == 0)
    logfile = fopen(".fresh.log", "a");

  int res = 0;

  va_list ap;
  va_start(ap, format);
  res += vfprintf(logfile, format, ap);
  va_end(ap);

  res += fprintf(logfile, "\n");

  return res;
}

