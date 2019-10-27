
#include "parser.h"

#include "util.h"

#include <string.h>
#include <stdlib.h>

char**
parse (char *str, size_t *nwords)
{
  /* Diese Funktion zerlegt eine eingegebene Zeichenkette in Worte, indem
   * sie sie an Whitespace Zeichen trennt. Dies ist ein sehr einfacher lexer,
   * der allerdings nicht ausreichend ist, um komplexere Eingaben zu erkennen.
   *
   * Für den Anfang ist er allerdings ausreichend. */

  static const char *whitespace = " \f\n\r\t\v";

  debug("parsing input '%s'", str);

  *nwords = 0;
  char **words = NULL;

  char *token = NULL;
  char *start = str;
  while ((token = strsep(&start, whitespace)))
    {
      if (*token == '\0')
        continue;

      words = realloc(words, sizeof(*words) * (*nwords + 2));
      words[*nwords] = token;
      (*nwords)++;
      words[*nwords] = NULL;
    }

  debug(" | nwords: %zu", *nwords);
  size_t i;
  for (i = 0; i < *nwords; ++i)
    debug(" |   '%s'", words[i]);

  return words;
}
