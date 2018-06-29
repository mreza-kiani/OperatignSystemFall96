# defining variables:
CC=g++
CFLAGS=-c
HEADERS=network.h node.h hidden_node.h input_node.h output_node.h stats_node.h data.h global.h
OBJECTS=main.o network.o node.o hidden_node.o input_node.o stats_node.o output_node.o data.o

#defining rules:
all: network_out

network_out: $(OBJECTS) $(HEADERS)
	$(CC) $(OBJECTS) -pthread -o network.out

main.o: main.cpp network.h
	$(CC) $(CFLAGS) main.cpp

network.o: network.cpp $(HEADERS)
	$(CC) $(CFLAGS) network.cpp

node.o: node.cpp node.h global.h
	$(CC) $(CFLAGS) node.cpp

input_node.o: input_node.cpp input_node.h node.h data.h global.h
	$(CC) $(CFLAGS) input_node.cpp

hidden_node.o: hidden_node.cpp hidden_node.h node.h data.h global.h
	$(CC) $(CFLAGS) hidden_node.cpp

output_node.o: output_node.cpp output_node.h node.h global.h data.h
	$(CC) $(CFLAGS) output_node.cpp

stats_node.o: stats_node.cpp stats_node.h node.h global.h data.h
	$(CC) $(CFLAGS) stats_node.cpp

data.o: data.cpp data.h
	$(CC) $(CFLAGS) data.cpp

zip: clean
	tar cvf 810194388.zip *

clean:
	rm -rf *.o *.out