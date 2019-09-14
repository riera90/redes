#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


const char* date_request_handler()
{
    time_t now;
    now = time(NULL);
    struct tm* now_tm = localtime(&now);
    char buffer [64];
    strftime(buffer, sizeof(buffer), "%A, %d de %B", now_tm);
    char* buffer_dma = (char*) calloc(sizeof(char), 64);
    strcpy(buffer_dma, buffer);
    return buffer_dma;
}


const char* time_request_handler()
{
    time_t now;
    now = time(NULL);
    struct tm* now_tm = localtime(&now);
    char buffer [64];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", now_tm);
    char* buffer_dma = (char*) calloc(sizeof(char), 64);
    strcpy(buffer_dma, buffer);
    return buffer_dma;
}


const char* datetime_request_handler()
{
    time_t now;
    now = time(NULL);
    struct tm* now_tm = localtime(&now);
    char buffer [64];
    strftime(buffer, sizeof(buffer), "%A, %d de %B; %H:%M:%S", now_tm);
    char* buffer_dma = (char*) calloc(sizeof(char), 64);
    strcpy(buffer_dma, buffer);
    return buffer_dma;
}


const char* request_handler(const char* request)
{
    if (!strcmp((const char*)request, "time"))
        return time_request_handler();
        
    if (!strcmp((const char*)request, "date"))
        return date_request_handler();
        
    if (!strcmp((const char*)request, "datetime"))
        return datetime_request_handler();
        
    if (!strcmp((const char*)request, "shutdown")) 
        return "shuting down the microservice";
    
    return "request not found";
}


int main()
{
    
    // screates a UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (server_fd == -1) {
        fprintf(stderr, "ERROR: unable to open socket");
        return 1;
    }
    
    // configures the port as ipv4 localhost and port 2000
    struct sockaddr_in server;
    
    server.sin_family = AF_INET; // ipv4
    server.sin_port = htons(2000);
    server.sin_addr.s_addr = htonl(INADDR_ANY); // is htonl necesary?
    
    // tries to bind the fd to the address and port
    {
        int bind_payload = bind(
            server_fd,
            (struct sockaddr*) &server,
            sizeof(server)
        );
        
        if (bind_payload == -1){
            fprintf(stderr, "ERROR: unable to bind socket");
            close(server_fd);
            return 2;
        }
    }
    
    
    // creates variables to store the client information
    struct sockaddr_in client;
    socklen_t client_length = sizeof(client);
    char request[255];
    
    u_short exit_condition = 0;
    
    
    // while the exit request is not performed, the server will run
    while (!exit_condition)
    {
        // get the request form the client
        int received = recvfrom(
            server_fd,
            request,
            sizeof(request),
            0,
            (struct sockaddr*) &client,
            &client_length
        );
        
        if (received <= 0){
            fprintf(stderr, "error in request, ignoring\n");
            continue;
        }
        
        printf("request  <%s>\n", request);
        
        // process the request, this is be made with handlers
        char payload[255];
        strcpy(payload, request_handler(request));
        printf("payload <%s>\n", payload);
        
        // interprets the shutdown request
        if (!strcmp((const char *)request, "shutdown")) {
            exit_condition = 1;
            continue;
        }
        
        // sends the payload back to the client
        int send_payload_status = sendto(
            server_fd,
            payload,
            sizeof(payload),
            0,
            (struct sockaddr*) &client,
            client_length
        );
    }
    
    close(server_fd);
    return 0;
}