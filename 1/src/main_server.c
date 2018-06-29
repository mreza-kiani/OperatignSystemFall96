#include "utils.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char* get_data_servers_info(Node* head){
    Node* curr;
    int num_of_data_servers = 0;
    char num_of_data_servers_s[5];
    char* result = (char*)malloc(300 * sizeof(char));

    for (curr = head->next; curr != NULL; curr = curr->next)
        num_of_data_servers += 1;

    sprintf(num_of_data_servers_s, "%d|", num_of_data_servers);
    strcpy(result, num_of_data_servers_s);

    for (curr = head->next; curr != NULL; curr = curr->next) {
        strcat(result, curr->file_info.port);
        if (curr->next != NULL)
            strcat(result, "|");
    }
    return result;
}

int main(void)
{
    Node* head = create_dummy_head();

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
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
    if ((rv = getaddrinfo(NULL, MAIN_SERVER_PORT, &hints, &ai)) != 0) {
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

                        if (strcmp(parts[0], "FILE_INFO") == 0){
                            Node* new_node = create_node(parts[2], parts[1], atoi(parts[3]));
                            insert_node(head, new_node);

                        }
                        else if (strcmp(parts[0], "GET_FILE") == 0){
                            char* result = get_data_servers_info(head);
                            // send to the listener!
                            if (send(i, result, strlen(result), 0) == -1) {
                                print_error("send");
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}