#include "utils.h"

void init_file_info(FileInfo* file_info)
{
	char name_buf[100];
	char part_buf[5];
	
	print("Enter file name:", WITH_LINE_BREAK);
	scan(name_buf);
	strcpy(file_info->name, name_buf);

	print("Enter partition number:", WITH_LINE_BREAK);
	scan(part_buf);
	file_info->partition = atoi(part_buf);
}

void get_data_server_port(FileInfo* file_info)
{
	print("Enter data server port:", WITH_LINE_BREAK);
	char tmp[7];
    scan(tmp);
    strcpy(file_info->port, tmp);
}

void encode_info_to(FileInfo file_info, char result[])
{
	char partition_s[4];
	strcpy(result, "FILE_INFO|");
	strcat(result, file_info.port);
	strcat(result, "|");
	strcat(result, file_info.name);
	strcat(result, "|");
	sprintf(partition_s, "%d", file_info.partition);
	strcat(result, partition_s);
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void send_info_to_main_server(FileInfo file_info)
{
	char result[200];
    memset(result, 0, strlen(result));
	encode_info_to(file_info, result);
    int sockfd, numbytes;  
    char buf[MAX_DATA_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char* output_message;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", MAIN_SERVER_PORT, &hints, &servinfo)) != 0) {
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
	if (send(sockfd, result, strlen(result), 0) == -1) {
        print_error("send");
    }		
    print(concat_strings("client: sent -> ", result), WITH_LINE_BREAK);

    close(sockfd);
}

void serve_as_data_server(FileInfo file_info)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    char file_buf[BUFFER_SIZE];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, file_info.port, &hints, &ai)) != 0) {
        print_error("selectserver: failed to getaddrinfo");
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        print_error("selectserver: failed to bind");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        print_error("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            print_error("select");
            exit(4);
        }
        memset(buf, 0, strlen(buf));

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        print_error("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        print(concat_strings("selectserver: new connection from ", 
                            inet_ntop(remoteaddr.ss_family, get_in_addr(
                                (struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN)),
                            WITHOUT_LINE_BREAK);
                        char tmp_msg[30];
                        sprintf(tmp_msg, " on socket %d", newfd);
                        print(tmp_msg, WITH_LINE_BREAK);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            char tmp_msg[200];
                            sprintf(tmp_msg, "selectserver: socket %d hung up", i);
                            print(tmp_msg, WITH_LINE_BREAK);
                        } else {
                            print_error("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        print(buf, WITH_LINE_BREAK);
                        char** parts = str_split(buf, '|');

                        if (strcmp(parts[0], "GET_FILE") == 0){
                            int file_ds = open(file_info.name, O_RDONLY );
                            if ( file_ds < 0 ){
                                print("Failed to open file", WITH_LINE_BREAK);
                                return;
                            }
                            print("file opened", WITH_LINE_BREAK);

                            ssize_t ret_in;
                            int file_size = get_file_size(file_ds);
                            int numbytes, sent_data = 0;
                            memset(file_buf, 0, strlen(file_buf));
                            while(file_size > sent_data) {
                                numbytes = read(file_ds, file_buf, BUFFER_SIZE);
                                file_buf[numbytes] = '\0';
                                print("file read => ", WITHOUT_LINE_BREAK);
                                print(file_buf, WITH_LINE_BREAK);
                                if (send(i, file_buf, strlen(file_buf), 0) == -1) {
                                    print_error("send");
                                }
                                sent_data += strlen(file_buf);
                            }
                            sleep(1);
                            if (send(i, END_OF_FILE, strlen(END_OF_FILE), 0) == -1) {
                                print_error("send");
                            }
                            print("file has been sent to client", WITH_LINE_BREAK);
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
}

int main(int argc, char const *argv[])
{
	FileInfo file_info;
	get_data_server_port(&file_info);
	init_file_info(&file_info);

	send_info_to_main_server(file_info);
    serve_as_data_server(file_info);

	return 0;
}