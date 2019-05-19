/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>
#include <sys/queue.h>
#include <signal.h>

int sockfd;
int newsockfd;
int port_number;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
int running = 1;
int cooked_meals = 0;
TAILQ_HEAD(tailhead, entry) head;

struct entry {
    char c;
    TAILQ_ENTRY(entry) entries;
};

void add_to_queue(char ch) {
    struct entry *elem;
    elem = malloc(sizeof(struct entry));
    if (elem) {
        elem->c = ch;
    }
    TAILQ_INSERT_HEAD(&head, elem, entries);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


void sig_hand(int signum)
{
    running = 0;
    close(newsockfd);
    close(sockfd);
    exit(0);
}

void safe_stop(void)
{
    for(int i =0;i<32;i++)
    {
        signal(i, sig_hand);
    }
}

int main(int argc, char *argv[])
{
    safe_stop();
    socklen_t client_length;
    // Max message size
    char buffer[256];
    char * endp;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    printf("Cozinheiro esperando alguém com fome\n");
    // Domain: AF_INET
    // Type: SOCK_STREAM
    // Protocol: 0 = Unspecified
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Sucesso ela retorna um inteiro não negativo
    if (sockfd < 0)
        error("ERROR opening socket");
    // Enche server_address de \0
    bzero((char *) &server_address, sizeof(server_address));
    port_number = strtol(argv[1],&endp,10);
    server_address.sin_family = AF_INET;
    // Binda para todas as interfaces disponíveis
    // server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Converte port_number para network number
    server_address.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &server_address,
             sizeof(server_address)) < 0)
        error("ERROR on binding");
    while(running) {
        listen(sockfd, 255);
        client_length = sizeof(client_address);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &client_address,
                           &client_length);
        if (newsockfd < 0)
            error("ERROR on accept");
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) error("ERROR reading from socket");
        /*----------------- Client Message --------------------*/

        if(!strcmp(buffer,"I need food!"))
        {
            n = write(newsockfd, "Here the marmitex", strlen("Here the marmitex"));
            if (n < 0) error("ERROR writing to socket");
            cooked_meals++;
            printf("Cooked meals:%d\n", cooked_meals);

        }
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
