/******************************/
/*                            */
/*      Chat Room Server      */
/*          ver.1             */
/*        by  Simon           */
/*                            */
/******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define port 7777
#define size 128
#define userNumber 10

struct sock_Info{
    int used;
    char Name[20];
    int sockfd;
    struct sockaddr_in sock_addr;
};

typedef struct sock_Info s_Info;

void show_Init(void);
void show_IP(s_Info user, int option);
void *thread_client(void *ptr);
int chooseUserNumber(s_Info *client);

s_Info client[userNumber];


/* ================================================ */

int main(void){

    /* Initialization */

    s_Info server;
    int i, id;

    bzero(&server, sizeof(server));                     // set host to zero.
    server.sock_addr.sin_family = PF_INET;
    server.sock_addr.sin_port = htons(port);            // set socket port
    server.sock_addr.sin_addr.s_addr = INADDR_ANY;


    int addrlen = sizeof(struct sockaddr_in);
    pthread_t tid[userNumber];

    for(i = 0; i < userNumber; i++){
        client[i].used = 0;
    }

    /* socket() - Create an endpoint for communication. */

    if((server.sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket() is ERROR");
        exit(0);
    }

    /* bind() - Bind a name to a socket */

    int optval = 1;

    setsockopt(server.sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));    // set address can be reused.
    signal(SIGPIPE,SIG_IGN);            // ignore signal pipe

    if(bind(server.sockfd, (struct sockaddr*)&server.sock_addr, sizeof(server.sock_addr)) == -1){
        perror("bind() is ERROR");
        exit(0);
    }

    /* listen() - Listen for connections on a socket */

    if(listen(server.sockfd, userNumber) == -1){        // listen to socket with max 10 connections.
        perror("listen() is ERROR");
        exit(0);
    }

    /* Ready for providing service */

    show_Init();               // show welcome message

    while(1){

        /* accept() - Accept a connection on a socket */

        if((id = chooseUserNumber(client)) == userNumber){
            printf("\n\nFull Connection...\n\n");
            exit(0);
        }

        //printf("\n\nID = %d\n\n", id);

        if((client[id].sockfd = accept(server.sockfd, (struct sockaddr*)&client[id].sock_addr, &addrlen)) == -1){
            perror("accept() is ERROR");
            exit(0);
        }

        /* Service thread for client (Receive) */

        pthread_create(&tid[id], NULL, thread_client, &client[id]);

        //show_IP(client[id], 1);

        /* Service thread (Send messages to clients) */

    }


    /* Close the server */

    close(server.sockfd);

    return 0;
}

/* ================================================ */

/* Show Start Information */

void show_Init(void){

    system("clear");

    printf("*****************************************\n"
           "*                                       *\n"
           "*            Chat Room Server           *\n"
           "*                                       *\n"
           "*****************************************\n");

    printf("\n\nServer Socket Port : %d\n\n", port);
    printf("\nServer is running............\n\n\n");
    return;
}

/* ================================================ */

/* Show Client's IP */

void show_IP(s_Info user, int option){

    char *user_IP;

    user_IP = inet_ntoa(user.sock_addr.sin_addr);

    switch(option){

        case 1 :
            printf("Receive the connection from : %s / Name : %s\n", user_IP, user.Name);
            break;
        case 2 :
            printf("Close the connection from : %s / Name : %s\n", user_IP, user.Name);
            break;
        default :
            break;
    }


    return ;
}

/* ================================================ */

/* Choose an empty ID */

int chooseUserNumber(s_Info *client){

    int i;

    for(i = 0; (client[i].used != 0) && i < userNumber; i++){
        /* Do Nothing */
        //printf("client[i].used = %d\n\n", i, client[i].used);
    }

    client[i].used = 1;

    return i;
}

/* ================================================ */

/* Service thread for client (Receive) */

void *thread_client(void *ptr){

    char recvBuffer[size];
    char sendBuffer[size + 24];
    s_Info *user = (s_Info *)ptr;
    int i;
    char str1[] = "Please Enter Your Name : ";

    /* Initialization */

    bzero(recvBuffer, sizeof(recvBuffer));
    bzero(sendBuffer, sizeof(sendBuffer));

    /* request for user Name */

    send(user->sockfd, str1, strlen(str1), 0);
    recv(user->sockfd, user->Name, sizeof(user->Name), 0);

    show_IP(*user, 1);

    while(1){

        if(recv(user->sockfd, recvBuffer, sizeof(recvBuffer), 0) > 0){

            sprintf(sendBuffer, "# %s > ", user->Name);
            strcat(sendBuffer, recvBuffer);

            for(i = 0; i < userNumber; i++){
                if((client[i].used != 0) && (client[i].sockfd != user->sockfd)){
                    send(client[i].sockfd, sendBuffer, strlen(sendBuffer), 0);
                }
            }

            bzero(sendBuffer, sizeof(sendBuffer));
            bzero(recvBuffer, sizeof(recvBuffer));
        }
    }

    user->used = 0;           // set to not used
    close(user->sockfd);
    return;
}
