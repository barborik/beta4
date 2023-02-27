CC = gcc

CFLAGS = -Ofast -std=c99 -Wall -Wextra -Wpedantic -funroll-loops -fopenmp -D_GLIBCXX_PARALLEL -Wno-unused-function
LFLAGS = -lm

INCL = -I lib/libzahl -I lib/glew -I lib/glfw
LIBS = -L lib/libzahl -L lib/glew -L lib/glfw

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
LFLAGS += -static -lzahl-mingw -l:glew32s.lib -l glfw3-mingw -lopengl32 -lgdi32 -lws2_32
else
LFLAGS += -lzahl -lGLEW -lglfw3 -lX11 -lGL
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
