//
// Created by Sebastian Tanase on 11/11/22.
//
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for atoi()
#define MAX 20
char indirizzoIp[MAX];
int porta;
int firstTime=1;
#define PORT 1234 // Numero di porta di default

char *leggiIndirizzoIp() {
    bzero(indirizzoIp, sizeof(indirizzoIp));
    printf("\n Inserisci indirizzo ip del server: ");
    scanf("%s", indirizzoIp);
    printf("indirizzo ip del server: %s ", indirizzoIp);
    return indirizzoIp;
}

int leggiPorta() {
    porta = 0;
    printf("\n Inserisci la porta d'accesso del server: ");
    scanf("%d", &porta);
    return porta;
}

void comunica();

void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}
void ClearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}


//funzione per leggere una stringa
void leggiStringa(int sockfd)
{
    char buff[MAX];
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("\nFrom Server : %s", buff);
    bzero(buff, sizeof(buff));
}
//funzione per scrivere una stringa e mandarla al server
void scriviStringa(int sockfd, char* string){
    write(sockfd, string, strlen(string));
}

int main(void){
#if defined WIN32
    WSADATA wsaData;
int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
if (iResult != 0) {
printf ("error at WSASturtup\n"); return -1;
}
#endif
//ricezione dei dati di comunicazione da parte del client
    leggiIndirizzoIp();
    leggiPorta();
    //avvio la funzione di comunicazione con il server
    comunica();
}

void comunica() {
#if defined WIN32
    WSADATA wsaData;
int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
if (iResult != 0) {
printf ("error at WSASturtup\n"); return -1;
}
#endif
// CREAZIONE DELLA SOCKET
    int Csocket;
    Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket < 0) {
        ErrorHandler("socket creation failed.\n");
        closesocket(Csocket);
        ClearWinSock();
    }

    // COSTRUZIONE DELL’INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    //memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(indirizzoIp); // IP del server
    sad.sin_port = htons(porta); // Server port
    if(firstTime) printf("\nCreazione del client");
    // CONNESSIONE AL SERVER
    if (connect(Csocket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
        ErrorHandler("Failed to connect.\n");
        closesocket(Csocket);
        ClearWinSock();
    } else {
        if(firstTime) printf("\nconnessione al server avvenuta con successo");
        leggiStringa(Csocket);
        char stringa1[MAX];
        printf("\nInserisci la prima stringa da inviare al client:");
        scanf("%s", stringa1);
        //printf("\nstring %s", stringa1);
        scriviStringa(Csocket, stringa1);
        printf("Inserisci la seconda stringa da inviare al client: ");
        bzero(stringa1, sizeof(stringa1)); // mi pulisco la variabile e la riutilizzo senza sprecarla
        scanf("%s", stringa1);
        //printf("\nstring %s", stringa1);
        scriviStringa(Csocket, stringa1);
        leggiStringa(Csocket);
        printf("\nvuoi continure S/N ?");
        bzero(stringa1, sizeof(stringa1)); // mi pulisco la variabile e la riutilizzo senza sprecarla
        scanf("%s", stringa1);
        if (strcmp(stringa1, "S") == 0 || strcmp(stringa1, "s") == 0) {
            close(Csocket);
            firstTime = 0;
            comunica();
        } else { // chiudo a prescindere la comunicazione anche se non ha scritto N o n
            printf("chiusura della connessione in corso...");
            // close the socket
            close(Csocket);
        }
    }
}