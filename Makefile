CC=gcc
FLAGS=-Wall -g 
LIBS=-ltermbox
OUT=art
INCLUDE=-I headers/
OBJS=./out/pixel.o

$(OUT): ./out $(OBJS) ./out/main.o
	$(CC) $(FLAGS) $(LIBS) $(INCLUDE) -o $(OUT) ./out/main.o $(OBJS) 

./out:
	mkdir ./out

./out/main.o: main.c headers/main.h $(OBJS)
	$(CC) $(FLAGS) $(INCLUDE) -o ./out/main.o -c main.c 

./out/pixel.o: pixel.c headers/pixel.h
	$(CC) $(FLAGS) $(INCLUDE) -o ./out/pixel.o -c pixel.c
	
.Phony: run

run:
	./$(OUT)

clean:
	rm $(OUT) out/*
