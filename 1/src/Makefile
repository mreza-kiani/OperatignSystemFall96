# defining variables:
CC=gcc
CFLAGS=-c
headers=utils.h
main_server_objects=main_server.o utils.o 
data_server_objects=data_server.o utils.o 
client_objects=client.o utils.o 

#defining rules:
all: main_server_out data_server_out client_out

main_server_out: $(main_server_objects) $(headers)
	$(CC) $(main_server_objects) -o main_server_out

data_server_out: $(data_server_objects) $(headers)
	$(CC) $(data_server_objects) -o data_server_out

client_out: $(client_objects) $(headers)
	$(CC) $(client_objects) -o client_out

main_server.o: main_server.c $(headers)
	$(CC) $(CFLAGS) main_server.c

data_server.o: data_server.c $(headers)
	$(CC) $(CFLAGS) data_server.c

client.o: client.c $(headers)
	$(CC) $(CFLAGS) client.c

utils.o: utils.c $(headers)
	$(CC) $(CFLAGS) utils.c

clean:
	rm -rf *o *_out	