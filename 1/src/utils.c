#include "utils.h"


void print(const char* message, int with_line_break)
{
	write(STDOUT_FILENO, message, strlen(message));
	if (with_line_break == 1)
		write(STDOUT_FILENO, "\n", 1);;
}

void print_error(const char* message)
{
    write(STDERR_FILENO, message, strlen(message));
    write(STDERR_FILENO, "\n", 1);;
}

void scan(char result[])
{
	read(STDIN_FILENO, result, 100);
	for (int i = 0; i < strlen(result); ++i)
		if (result[i] == NEW_LINE)
			result[i] = '\0';
}

char* concat_strings(const char* a, const char* b)
{
	char* result = (char*)malloc(200 * sizeof(char));
	strcpy(result, a);
	strcat(result, b);
	return result;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

Node* create_dummy_head()
{
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    return head;
}

Node* create_node(char* name, char* port, int partition)
{
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->next = NULL;
    strcpy(new_node->file_info.name, name);
    strcpy(new_node->file_info.port, port);
    new_node->file_info.partition = partition;
    return new_node;
}

void insert_node(Node* head, Node* new_node)
{
    Node* curr;
    for (curr = head; curr->next != NULL; curr = curr->next)
    {
        if (curr->next->file_info.partition > new_node->file_info.partition)
        {
            new_node->next =  curr->next;
            curr->next = new_node;
            return;          
        }
    }
    curr->next = new_node;
}

long get_file_size(int file_fd) {
    struct stat buf;
    fstat(file_fd, &buf);
    return buf.st_size;
}