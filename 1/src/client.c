#include "utils.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void get_file_from_data_server(char* port, int file_fd)
{
    int sockfd, numbytes;  
    char buf[BUFFER_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", port, &hints, &servinfo)) != 0) {
        print(concat_strings("getaddrinfo: ", gai_strerror(rv)), WITH_LINE_BREAK);
        return;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            print("client: socket", WITH_LINE_BREAK);
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            print("client: connect", WITH_LINE_BREAK);
            continue;
        }
        break;
    }
    if (p == NULL) {
        print("client: failed to connect", WITH_LINE_BREAK);
        return;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    print(concat_strings("client: connecting to ", s), WITH_LINE_BREAK);

    freeaddrinfo(servinfo); // all done with this structure
    char* command = "GET_FILE";
    if (send(sockfd, command, strlen(command), 0) == -1) {
        print_error("send");
    }       
    print(concat_strings("client: sent -> ", command), WITH_LINE_BREAK);


    memset(buf, 0, strlen(buf));
    while (((numbytes = recv(sockfd, buf, BUFFER_SIZE-1, 0)) != -1)) {
        buf[numbytes] = '\0';
        print("client: received => ", WITHOUT_LINE_BREAK);
        print(buf, WITH_LINE_BREAK);

        if (strcmp(buf, END_OF_FILE) == 0)
            break;
        write(file_fd, buf, strlen(buf));
    }

    close(sockfd);
}

void get_output_name(char* name)
{
    char name_buf[100];
    
    print("Enter output name:", WITH_LINE_BREAK);
    scan(name_buf);
    strcpy(name, name_buf);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAX_DATA_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char output_name[30];

    get_output_name(output_name);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", MAIN_SERVER_PORT, &hints, &servinfo)) != 0) {
        print(concat_strings("getaddrinfo: ", gai_strerror(rv)), WITH_LINE_BREAK);
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            print("client: socket", WITH_LINE_BREAK);
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            print("client: connect", WITH_LINE_BREAK);
            continue;
        }
        break;
    }
    if (p == NULL) {
        print("client: failed to connect", WITH_LINE_BREAK);
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    print(concat_strings("client: connecting to ", s), WITH_LINE_BREAK);

    freeaddrinfo(servinfo); // all done with this structure
    char* command = "GET_FILE";
    if (send(sockfd, command, strlen(command), 0) == -1) {
        print_error("send");
    }       
    print(concat_strings("client: sent -> ", command), WITH_LINE_BREAK);


    if ((numbytes = recv(sockfd, buf, MAX_DATA_SIZE-1, 0)) == -1) {
        print_error("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    print("client: received ", WITHOUT_LINE_BREAK);
    print(buf, WITH_LINE_BREAK);

    char** parts = str_split(buf, '|');
    int file_fd = open(output_name, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR, S_IWUSR, S_IXUSR);
    for (int i = 1; i <= atoi(parts[0]); ++i)
        get_file_from_data_server(parts[i], file_fd);
    close(file_fd);


    close(sockfd);
    return 0;
}