CC = gcc

CFLAGS = -O3 -std=c99 -Wall -Wextra -Wpedantic
LFLAGS = 

INCL = -I lib/libzahl
LIBS = -L lib/libzahl

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

TARGET_v1 = bin/beta4-v1.0
TARGET_v2 = bin/beta4-v2.0
TARGET_v3 = bin/beta4-v3.0
TARGET_v4 = bin/beta4-v4.0

ifeq ($(OS),Windows_NT)
TARGET_v1 := $(TARGET_v1).exe
TARGET_v2 := $(TARGET_v2).exe
TARGET_v3 := $(TARGET_v3).exe
TARGET_v4 := $(TARGET_v4).exe
LFLAGS += -l zahl-mingw
else
LFLAGS += -l zahl
endif

v1: obj/main1.o
	$(CC) $^ -o $(TARGET_v1) $(LIBS) $(LFLAGS)

v2: obj/main2.o
	$(CC) $^ -o $(TARGET_v2) $(LIBS) $(LFLAGS)

v3: obj/main3.o
	$(CC) $^ -o $(TARGET_v3) $(LIBS) $(LFLAGS)

v4: obj/main4.o
	$(CC) $^ -o $(TARGET_v4) $(LIBS) $(LFLAGS)

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(INCL) $(CFLAGS)
