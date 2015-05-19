/******************************/
/*                            */
/*      Chat Room Client      */
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

#define port 7777
#define size 128

struct sock_Info{
    int used;
    char Name[20];
    int sockfd;
    struct sockaddr_in sock_addr;
};

typedef struct sock_Info s_Info;

void show_Init(char serverIP[]);
void send_recv(void);
void *thread_init(void *ptr);
void *thread_send(void *ptr);
void *thread_recv(void *ptr);

s_Info server, user;

/* ================================================ */

int main(void){

    /* Initializaton */

    char buffer[size];
    char serverIP[20];

    printf("Please Enter the server IP : ");
    scanf("%s", serverIP);
    system("clear");

    bzero(&user, sizeof(user));                    // Initialize user
    bzero(&server, sizeof(server));                // Initialize Server Info.
    server.sock_addr.sin_family = PF_INET;
    server.sock_addr.sin_port = htons(port);            // Socket Port
    inet_aton(serverIP, &server.sock_addr.sin_addr);    // Server IP

    /* socket() - Create an endpoint for communication. */

    if((user.sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
       printf("socket() is ERROR!!");
       exit(0);
    }

    /* connect() - Initiate a connection on a socket */

    if(connect(user.sockfd, (struct sockaddr*)&server.sock_addr, sizeof(server.sock_addr)) == -1){
        printf("Can not connect to Server (%s)... \n", serverIP);
        exit(0);
    } // connect to the server.
    else{
        show_Init(serverIP);       // show welcome message
    }

    /* recv() & send() - Receive and Send messages */

    send_recv();

    /* close() - Close the connection */

    close(user.sockfd);

    return 0;
}

/* ================================================ */

/* Show startting Message */

void show_Init(char serverIP[]){

    system("clear");

    printf("\t\t*****************************************\n"
           "\t\t*                                       *\n"
           "\t\t*            Chat Room Client           *\n"
           "\t\t*                                       *\n"
           "\t\t*****************************************\n");

    printf("\n\n============= Connectting to server successfully (%s) =============\n\n", serverIP);
    printf("\n=============      Welcome to 702b Chat Room          =============\n\n");
    return;

    return ;
}

/* ================================================ */

/* Send() and Recv() */

void send_recv(void){

    pthread_t tid_init, tid_send, tid_recv;

    pthread_create(&tid_init, NULL, thread_init, NULL);

    /* Create thread */

    pthread_join(tid_init, NULL);

    pthread_create(&tid_recv, NULL, thread_recv, NULL);
    pthread_create(&tid_send, NULL, thread_send, NULL);

    pthread_join(tid_recv, NULL);
    pthread_join(tid_send, NULL);

    return ;
}

/* ================================================ */

/* Login - Enter ID  */

void *thread_init(void *ptr){

    char str1[] = "\n";
    char buffer[size];

    /* Enter the Name */

    bzero(buffer, sizeof(buffer));
    recv(user.sockfd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
    scanf("%s", user.Name);
    send(user.sockfd, user.Name, strlen(user.Name), 0);

    system("clear");
    printf("\n=============    Hello, %s. Welcome to 702b Chatroom     =============\n\n\n\n > ", user.Name);


    return ;
}

/* ================================================ */

/* Send() used by pthread */

void *thread_send(void *ptr){

    char sendBuffer[size + 24];
    char str1[] = "\n";

    bzero(sendBuffer, sizeof(sendBuffer));

    while(1){

        gets(sendBuffer);
        strcat(sendBuffer, str1);
        send(user.sockfd, sendBuffer, strlen(sendBuffer), 0);

        bzero(sendBuffer, sizeof(sendBuffer));
        fflush(stdin);
    }

    return ;
}

/* ================================================ */

/* Send() used by pthread */

void *thread_recv(void *ptr){

    char recvBuffer[size];

    /* Initialization */

    bzero(recvBuffer, sizeof(recvBuffer));

    while(1){

        if(recv(user.sockfd, recvBuffer, sizeof(recvBuffer), 0) > 0){
            printf("%s", recvBuffer);
        }

        bzero(recvBuffer, sizeof(recvBuffer));
        fflush(stdout);
    }

    return ;
}
