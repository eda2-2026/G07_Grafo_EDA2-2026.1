CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
SRCDIR = src
TARGET = grafo_combustivel

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/csv.c \
       $(SRCDIR)/precos.c \
       $(SRCDIR)/grafo.c \
       $(SRCDIR)/analise.c \
       $(SRCDIR)/export.c

ifeq ($(OS),Windows_NT)
    TARGET = grafo_combustivel.exe
    RM = del /Q /F
else
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

run: $(TARGET)
	./$(TARGET)

clean:
	-$(RM) grafo_combustivel grafo_combustivel.exe

.PHONY: all run clean
