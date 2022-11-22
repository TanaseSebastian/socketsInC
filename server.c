#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for atoi()
#define PORT 1234 // default protocol port number
#define MAX 20
#define QLEN 6 // size of request queue
char buff[MAX]; //mi dicharo un buffer globale che pulisco ogni volta che ne ho bisogno
void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}
void ClearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        char stringa1[strlen(buff)];
        int i;
        for ( i = 0; i < strlen(buff); i++) {
            stringa1[i] = buff[i];
        }
        //stringa1[i++]="\n";
        printf("stringa1: %s: ", stringa1);
        // print buffer which contains the client contents
        printf("From client: %s\t ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        //while ((buff[n++] = getchar()) != '\n')
        ;
        for ( i = 0; i < strlen(stringa1); i++) {
            buff[i] = stringa1[i];
        }
        // and send that buffer to client
        write(connfd, buff, sizeof(buff));
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


// Funzione che uso per salutare il client
void salutaClient(int connfd, char* string)
{
    /*int dimensione = strlen(string);
    char buff[strlen(dimensione)];
        bzero(buff, dimensione);
        int i;
        for ( i = 0; i < dimensione; i++) {
            buff[i] = string[i];
        }
        //  send that buffer to client
        */
    write(connfd, string, strlen(string));
    //bzero(buff, sizeof(buff));

}
char* LetturaStringa(int connfd){
    bzero(buff, sizeof(buff));
    read(connfd, buff, sizeof(buff));
    printf("From Client : %s", buff);
    //char* str;
    /*for (int i = 0; i < MAX; ++i) {
        str[i] = buff[i];
    }*/
    //str = buff;
    return buff;
}

char* concatena(char* string1, char* string2){
    int i, j = 0; // j per stringaCompleta
    char stringaCompleta[MAX*2];
    for ( i = 0; i < strlen(string1); i++) {
        stringaCompleta[j] = string1[i];
        j++;
    }
    for ( i = 0; i < strlen(string2); i++) {
        stringaCompleta[j] = string2[i];
    }
    return stringaCompleta;
}

char* LetturaAndConcatena(int connfd){
    char stringa[MAX];
    bzero(stringa, sizeof(stringa));
    read(connfd, stringa, sizeof(stringa));
    printf("From Client : %s", stringa);
    strcat(buff, stringa);
    printf("\nConcateno con il buffer stringa completa: %s", buff);
    return buff;
}

void scriviStringa(int sockfd, char* string){
    write(sockfd, string, strlen(string));
}


int main(int argc, char *argv[]) {
    printf("\ninizo del main");
    int port;
    port = PORT; // use default port number
    if (port < 0) {
        printf("\nbad port number %s \n", argv[1]); return 0;
    }
#if defined WIN32 // initialize Winsock
    WSADATA wsaData;
int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
if (iResult != 0) {
ErrorHandler("Error at WSAStartup()\n"); return 0;
}
#endif

// CREAZIONE DELLA SOCKET
    int MySocket;
    MySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (MySocket < 0) {
        ErrorHandler("socket creation failed.\n");
        ClearWinSock();
        return -1;
    } else printf("\nCreazione della socket avvenuta correttamente");
// ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET
    struct sockaddr_in sad;
    // memset(&sad, 0, sizeof(sad)); // ensures that extra bytes contain 0 sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons(port); /* converts values between the host and network byte order. Specifically, htons() converts 16-bit quantities from host byte order to network byte order. */
    if (bind(MySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0) { ErrorHandler("bind() failed.\n");
        closesocket(MySocket);
        ClearWinSock();
        return -1;
    }else printf("\nbind avvenuto correttamente");
// SETTAGGIO DELLA SOCKET ALL'ASCOLTO
    if (listen (MySocket, QLEN) < 0) {
        ErrorHandler("listen() failed.\n"); closesocket(MySocket); ClearWinSock();
        return -1;
    }
    else{
        printf("\nServer in ascolto");
    }
// ACCETTARE UNA NUOVA CONNESSIONE
    struct sockaddr_in cad; // structure for the client address
    int clientSocket; // socket descriptor for the client
    int clientLen; // the size of the client address
    printf("\nIn attesa che un client si connetta...");
    while (1) { /* oppure for (;;) */
        clientLen = sizeof(cad); // set the size of the client address
        if ((clientSocket = accept(MySocket, (struct sockaddr *)&cad, &clientLen)) < 0) {
            ErrorHandler("accept() failed.\n");
            // CHIUSURA DELLA CONNESSIONE
            closesocket(MySocket);
            ClearWinSock();
            return 0;}
        else{
            printf("\nConnessione accetta e avvenuta con successo, gestisco il client avente indirzzo ip:  %s\n", inet_ntoa(cad.sin_addr));
            salutaClient(clientSocket, "Benvenuto Client");
            char* stringa1 = LetturaStringa(clientSocket);
            printf("\nho memorizzato la prima stringa: %s\n", stringa1);
            char* stringaCompleta = LetturaAndConcatena(clientSocket);
            scriviStringa(clientSocket, stringaCompleta);
            printf("\n stringa completa: %s\n", stringaCompleta);

        }
    }
}
