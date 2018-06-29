#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAIN_SERVER_PORT "9090"
#define MAX_DATA_SIZE 100
#define MAX_FILE_NUMBER 20
#define BUFFER_SIZE 256

#define WITH_LINE_BREAK 1
#define WITHOUT_LINE_BREAK 0

#define NEW_LINE '\n'
#define END_OF_FILE "EOF"

typedef struct FileInfo
{
	char name[100];
    char port[7];
	int partition;
} FileInfo;

typedef struct Node
{
	struct Node* next;
	FileInfo file_info;
} Node;

void print(const char* message, int with_line_break);
void print_error(const char* message);
void scan(char result[]);
char* concat_strings(const char* a, const char* b);
char** str_split(char* a_str, const char a_delim);

// link list functions
Node* create_dummy_head();
Node* create_node(char* name, char* port, int partition);
void insert_node(Node* head, Node* new_node);
long get_file_size(int file_fd);
