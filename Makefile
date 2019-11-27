
BIN = fresh
OBJ = src/fresh.o \
      src/builtin.o \
      src/dispatcher.o \
      src/parser/y.tab.o \
      src/parser/lex.yy.o \
      src/util.o

EXTRA_CLEAN = src/parser/y.tab.c \
              src/parser/y.tab.h \
              src/parser/lex.yy.c

CFLAGS = -Wall -Wextra -Isrc
LDADD = -lreadline -ly

.PHONY: all clean

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(EXTRA_CLEAN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDADD)

%.o: %.c
	$(CC) -o $@ $< -c $(CFLAGS)

src/parser/lex.yy.c: src/parser/fresh.l src/parser/y.tab.h
	lex -o $@ $<

src/parser/y.tab.c: src/parser/fresh.y
	yacc -o $@ $<

src/parser/y.tab.h: src/parser/fresh.y
	yacc --defines=$@ -o /dev/null $<
