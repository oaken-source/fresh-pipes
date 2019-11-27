
%{

/* Dies ist der Lexer, der Teil des Parsers, der die Eingabe in
 * vordefinierte Token zerlegt, und denen gegebenenfalls einen Wert
 * zuweist. */

#include "parser.h"
#include "y.tab.h"
#include "dispatcher.h"

int
parse (char *str, struct sh_program *p)
{
  YY_BUFFER_STATE state = yy_scan_string(str);
  yyparse(p);
  yy_delete_buffer(state);

  return 0;
}

%}

%option noyywrap

%%

\n        return NEWLINE;
>         return GREAT;
\<        return LESS;
>>        return DGREAT;
\|        return PIPE;

[a-zA-Z0-9_.,-]+  { yylval.str = strdup(yytext); return TOKEN; }

[ \t]

%%
