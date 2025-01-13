#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <stdbool.h>

#define DEFAULT_PORT 9999

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void capitalizeString(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char) str[i]);
    }
}

void split_at_colon(char *buffer, char **suffix, char* end) {
    for (; buffer < end; ++buffer) {
        if (*buffer == ':') {
            *buffer = '\0';
            *suffix = buffer + 1;
            return;
        }
    }
    *suffix = NULL;
}

char authenticate(char *user, char* password) {
    char stored_pass[10] = "PASSWORD";
    char capitalied_pass[10];
    char capitalied_user[10];

    strcpy(capitalied_user, user);
    strcpy(capitalied_pass, password);

    capitalizeString(capitalied_user);
    capitalizeString(capitalied_pass);

    printf("User: %s\n", capitalied_user);
    printf("Pass: %s\n", capitalied_pass);
    printf("Secr: %s\n", stored_pass);

    if (strcmp(capitalied_pass, stored_pass) == 0) {
        printf("Congratiolations %s user!\n\n The secret code is:", user);

        printf("TODO: add code that fetches secret...");

        printf("\n\n");
        return true;
    } else {
        printf("WRONG! Dear %s, this was totally wrong!\n", user);
        return false;
    }

}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno = DEFAULT_PORT;
    socklen_t clilen;
    char buffer[128];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    fd_set read_fds;
    int max_sd;
    struct timeval tv;
    int opt;

    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-p port]\n", argv[0]);
                printf("Synopsis: A simple TCP server that listens for incoming connections, receives strings of the form 'username:password', and prints a secret code if the user is worthy.\n The server will capitalize incoming user:password strings, because why not?\n\n");
                printf("Options:\n");
                printf("  -p <port>    Specify the port on which the server should listen. Default is %d.\n", DEFAULT_PORT);
                printf("  -h           Print this help message and exit.\n");
                exit(EXIT_SUCCESS);
            case 'p':
                portno = atoi(optarg);
                break;
            default: 
                fprintf(stderr, "Usage: %s [-p port] [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Listening on port %d... \n", portno);

    while (keep_running) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        max_sd = sockfd;

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if (select(max_sd + 1, &read_fds, NULL, NULL, &tv) < 0) {
            if (errno != EINTR) {
                error("ERROR in select");
            } else if (!keep_running) {
                printf("SIGINT received, shutting down.\n");
                break;
            }
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                if (keep_running) {
                    error("ERROR on accept");
                } else {
                    printf("SIGINT received, shutting down.\n");
                    break;
                }
            }

            bzero(buffer, sizeof(buffer));
            n = read(newsockfd, buffer, sizeof(buffer)-1);
            if (n < 0) error("ERROR reading from socket");

            char *username = buffer, *password = NULL;
            split_at_colon(buffer, &password, buffer + sizeof(buffer));
            if (password == NULL) {
                strcpy(buffer, "Malformed request! (send me a string like 'username:password')");
            } else if (authenticate(username, password)) {
                strcpy(buffer, "Authenticated! Check the server output to receive your prize.\n\n");
            } else {
                strcpy(buffer, "Access Denied!\n\n");
            }
            n = write(newsockfd, buffer, strlen(buffer));
            if (n < 0) error("ERROR writing to socket");

            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}