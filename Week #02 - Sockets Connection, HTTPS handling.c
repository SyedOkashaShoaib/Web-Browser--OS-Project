#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    //hello zafeer
    // 1- URL input (can be from terminal too)
    char url[256];
    printf("Enter a string: ");
    fgets(url, sizeof(url), stdin);
    url[strcspn(url, "\n")] = 0; 
    

    
    struct hostent *host = gethostbyname(url);
    if (host == NULL) 
    {
        fprintf(stderr, "ERROR : Host Name Not Found.\n");
        return 1;
    }

    // Convert to IP string
    char *ip = inet_ntoa(*((struct in_addr *)host->h_addr_list[0]));

    printf("IP address of %s is: %s\n", url, ip);


    // 2- Creating socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("Socket creation failed");
        return 1;
    }

    // 3- Setup server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);  // I connected on port 80
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // 4- Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to %s on port 80\n", ip);


    // 5- Send HTTP request
    char request[1024];
    sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url);
    send(sockfd, request, strlen(request), 0);
    printf("HTTP request sent!\n");

    // 6- Receive and print the response
    char buffer[4096];
    int bytes_received;
    FILE *file = fopen("History/output.html", "w");

    if (file == NULL) 
    {
        perror("File opening failed");
        close(sockfd);
        return 1;
    }

    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) 
    {
        buffer[bytes_received] = '\0'; // Null-terminate
        fwrite(buffer, 1, bytes_received, file); // Save to file
    }


    printf("\n\nResponse received. Closing socket.\n");
    


    close(sockfd);

    return 0;
}
