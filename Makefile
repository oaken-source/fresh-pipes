
BIN = fresh
OBJ = src/fresh.o \
      src/builtin.o \
      src/parser.o \
      src/util.o

CFLAGS = -Wall -Wextra
LDADD = -lreadline

.PHONY: all clean

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDADD)

%.o: %.c
	$(CC) -o $@ $< -c $(CFLAGS)
