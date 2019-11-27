
/* Die folgende Grammatik wurde angepasst und vereinfacht von:
 *   https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_10
 * der offiziellen Referenzpublikation der POSIX Standardisierungsgruppe. */
%{

#include <stdio.h>
#include "parser.h"
#include "dispatcher.h"

/* Diese Macros sind furchtbar schlechter Stil, und mangelhaft in
 * Sachen fehlerbehandlung, aber sie verkürzen den folgenden Code erheblich. */

#define ADD_TO_LIST(W, N, L) do { (N)++; (L) = realloc((L), sizeof(*(L)) * (N)); (L)[(N) - 1] = (W); } while(0)
#define ADD_TO_WORDS(W, N, L) do { (N)++; (L) = realloc((L), sizeof(*(L)) * ((N) + 1)); (L)[(N) - 1] = (W); (L)[(N)] = NULL; } while(0)
#define MERGE_TO_LIST(N1, L1, N2, L2) do { (L2) = realloc((L2), sizeof(*(L2)) * ((N1) + (N2))); size_t i; for (i = 0; i < (N1); ++i) (L2)[(N2) + i] = (L1)[i]; (N2) += (N1); free((L1)); } while(0)
#define MERGE_TO_WORDS(N1, L1, N2, L2) do { (L2) = realloc((L2), sizeof(*(L2)) * ((N1) + (N2) + 1)); size_t i; for (i = 0; i < (N1); ++i) (L2)[(N2) + i] = (L1)[i]; (N2) += (N1); free((L1)); (L2)[(N2)] = NULL; } while(0)
#define COMMAND_INIT(C) do { (C).words = NULL; (C).nwords = 0; (C).redirects = NULL, (C).nredirects = 0; } while(0)
#define PIPE_INIT(P) do { (P).commands = NULL; (P).ncommands = 0; } while(0)
#define PROGRAM_INIT(C) { (C).pipes = NULL; (C).npipes = 0; } while(0)

%}

%union {
    char *str;
    struct sh_redirect redirect;
    struct sh_command command;
    struct sh_pipe pipe;
    struct sh_program program;
}

%parse-param { struct sh_program *program }

%token  TOKEN
%token  NEWLINE

%token  GREAT LESS DGREAT PIPE

%type<str> TOKEN
%type<redirect> io_redirect
%type<command> command cmd_name cmd_prefix cmd_word cmd_suffix
%type<pipe> complete_command
%type<program> complete_commands

/* Beginne die Grammatik in EBNF
 *
 * Die folgenden Zeilen sind etwas schwarze Magie, wenn man nicht mit
 * Compilerbau vertraut ist. lex und yacc sind sehr mächtige Werkzeuge,
 * die es sich zu beherrschen lohnt, zum Beispiel zum Parsen von
 * Konfigurationsdateien, oder Domänenspezifischen Sprachen.
 *
 * Einen Überblick über die Feinheiten von ley und yacc finden Sie zum
 * Beispiel hier: http://dinosaur.compilertools.net/ */

%start program

%%

program          : linebreak complete_commands linebreak
                 {
                   *program = $2;
                 }
                 | linebreak
                 {
                   program->pipes = NULL;
                   program->npipes = 0;
                 }
                 ;
complete_commands: complete_commands newline_list complete_command
                 {
                   $$ = $1;
                   ADD_TO_LIST($3, $$.npipes, $$.pipes);
                 }
                 |                                complete_command
                 {
                   PROGRAM_INIT($$);
                   ADD_TO_LIST($1, $$.npipes, $$.pipes);
                 }
                 ;
complete_command :                                 command
                 {
                   PIPE_INIT($$);
                   ADD_TO_LIST($1, $$.ncommands, $$.commands);
                 }
                 | complete_command PIPE linebreak command
                 {
                   $$ = $1;
                   ADD_TO_LIST($4, $$.ncommands, $$.commands);
                 }
                 ;
command          : cmd_prefix cmd_word cmd_suffix
                 {
                   MERGE_TO_WORDS($2.nwords, $2.words, $$.nwords, $$.words);
                   MERGE_TO_LIST($2.nredirects, $2.redirects, $$.nredirects, $$.redirects);
                   MERGE_TO_WORDS($3.nwords, $3.words, $$.nwords, $$.words);
                   MERGE_TO_LIST($3.nredirects, $3.redirects, $$.nredirects, $$.redirects);
                   $$ = $1;
                 }
                 | cmd_prefix cmd_word
                 {
                   $$ = $1;
                   ADD_TO_WORDS($1.words[0], $$.nwords, $$.words);
                 }
                 | cmd_prefix
                 {
                   $$ = $1;
                 }
                 | cmd_name cmd_suffix
                 {
                   $$ = $1;
                   MERGE_TO_WORDS($2.nwords, $2.words, $$.nwords, $$.words);
                   MERGE_TO_LIST($2.nredirects, $2.redirects, $$.nredirects, $$.redirects);
                 }
                 | cmd_name
                 {
                   $$ = $1;
                 }
                 ;
cmd_name         : TOKEN                   /* Apply rule 7a */
                 {
                   COMMAND_INIT($$);
                   ADD_TO_WORDS($1, $$.nwords, $$.words);
                 }
                 ;
cmd_word         : TOKEN                   /* Apply rule 7b */
                 {
                   COMMAND_INIT($$);
                   ADD_TO_WORDS($1, $$.nwords, $$.words);
                 }
                 ;
cmd_prefix       :            io_redirect
                 {
                   COMMAND_INIT($$);
                   ADD_TO_LIST($1, $$.nredirects, $$.redirects);
                 }
                 | cmd_prefix io_redirect
                 {
                   $$ = $1;
                   ADD_TO_LIST($2, $$.nredirects, $$.redirects);
                 }
                 ;
cmd_suffix       :            io_redirect
                 {
                   COMMAND_INIT($$);
                   ADD_TO_LIST($1, $$.nredirects, $$.redirects);
                 }
                 | cmd_suffix io_redirect
                 {
                   $$ = $1;
                   ADD_TO_LIST($2, $$.nredirects, $$.redirects);
                 }
                 |            TOKEN
                 {
                   COMMAND_INIT($$);
                   ADD_TO_WORDS($1, $$.nwords, $$.words);
                 }
                 | cmd_suffix TOKEN
                 {
                   $$ = $1;
                   ADD_TO_WORDS($2, $$.nwords, $$.words);
                 }
                 ;
io_redirect      : GREAT     TOKEN
                 {
                   $$.filename = $2;
                   $$.type = OUT;
                 }
                 | LESS      TOKEN
                 {
                   $$.filename = $2;
                   $$.type = IN;
                 }
                 | DGREAT    TOKEN
                 {
                   $$.filename = $2;
                   $$.type = APPOUT;
                 }
                 ;
newline_list     :              NEWLINE
                 | newline_list NEWLINE
                 ;
linebreak        : newline_list
                 | /* empty */
                 ;
