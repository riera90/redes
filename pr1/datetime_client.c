#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char const *argv[]) {
    
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1){
        fprintf(stderr, "ERROR: unable to open socket");
        return 1;
    }
    
    struct sockaddr_in server;
    
    server.sin_family = AF_INET;
    server.sin_port = htons(2000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    socklen_t server_length = sizeof(server);
    
    char payload[255];
    
    strcpy(payload, argv[1]);
    
    printf("payload <%s>\n", payload);
    
    int payload_status = sendto(
        client_fd,
        payload,
        sizeof(payload),
        0,
        (struct sockaddr*) &server,
        server_length
    );
    
    char response[255];
    
    int server_response_status = recvfrom(
        client_fd,
        response,
        sizeof(response),
        0,
        (struct sockaddr*) &server,
        &server_length
    );
    
    printf("response <%s>\n", response);
    
    
    close(client_fd);
    
    return 0;
}