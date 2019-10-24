CC=gcc
FLAGS=-Wall -g 
LIBS=-lpthread -ltermbox
OUT=art
INCLUDE=-I headers/
OBJS=

$(OUT): ./out $(OBJS) ./out/main.o
	$(CC) $(FLAGS) $(LIBS) $(INCLUDE) -o $(OUT) ./out/main.o $(OBJS) 

./out:
	mkdir ./out

./out/main.o: main.c headers/main.h  $(OBJS)
	$(CC) $(FLAGS) $(INCLUDE) -o ./out/main.o -c main.c 
	
.Phony: run

run:
	./$(OUT)

clean:
	rm $(OUT) out/*
