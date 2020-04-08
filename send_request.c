#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include "parse_url.h"
#include "send_request.h"


#define USER_AGENT "tq"
#define ACCEPT "html/text"
#define PORT 80
#define BUFMAX 10000
#define GETRE "GET /"
#define VERSION "HTTP/1.1"

char* send_request_and_get_response(url_t* url){
    int sockfd; 
    /* create the serv_addr */
    struct hostent *server; 
    server = convert_host_to_ip(url->hostname); 
    struct sockaddr_in serv_addr = construct_server_add(server);  
    /* create the client socket */
    sockfd = create_socket(); 
    /* establish the connection with the server */
    connect_to_server(sockfd, serv_addr); 
    /* generate the request */
    char *header = generate_the_header("GET /", "HTTP/1.1", USER_AGENT, ACCEPT, url); 
    //printf("%s\n", header); 
    /* send the request */
    send_request(sockfd, header); 
    /* accept response */
    char *response = (char *)malloc(BUFMAX*sizeof(char));
    if(!response){
        perror("ERROR allocate memory"); 
        exit(0); 
    }
    int length = receive(sockfd, response); 
    //printf("%s\n", response); 
    /*close the socket*/
    close(sockfd);
    return response; 
}
char *generate_the_header(char* request_info, char* version, char* user_agent, char* accept, url_t* url){
    char request[300];
    if(strcmp(url->filename, "")!=0){
        sprintf(request, "%s%s/%s %s\r\nHost: %s \r\nUser-Agent: %s\r\nAccpet: %s\r\n\r\n",request_info, url->dir, url->filename,version, url->hostname, user_agent, accept);
    }else{
        sprintf(request, "%s%s%s %s\r\nHost: %s \r\nUser-Agent: %s\r\nAccpet: %s\r\n\r\n",request_info, url->dir, url->filename,version, url->hostname, user_agent, accept);
    }
    //printf("%s\n", request); 
    char *strin =(char*)calloc(300,sizeof(char*)); 
    if(strin){
        strcpy(strin, request); 
    }
    return strin; 
}

struct hostent* convert_host_to_ip(char * url){
    struct hostent *server; 
    //printf("converting the domain name to the ip address\n"); 
    server=gethostbyname(url); 
    if(server==NULL){
        printf("can't get the web's ip"); 
        exit(1); 
    }
    //printf("converting success!\n"); 
    return server; 
}
struct sockaddr_in construct_server_add(struct hostent *server){
    struct sockaddr_in serv_addr; 
    bzero((char*)&serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family=AF_INET; 
    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length); 
    serv_addr.sin_port=htons(PORT);
    return serv_addr; 
}

int create_socket(){
    int sockfd; 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if(sockfd<0){
        perror("ERROR opening socket"); 
        exit(0); 
    }
    return sockfd; 
}

void connect_to_server(int sockfd, struct sockaddr_in serv_addr){
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        perror("ERROR connecting"); 
        exit(0); 
    }
}

void send_request(int sockfd, char* header){
    //printf("let's send the header to the server!\n"); 
    int n=write(sockfd, header, strlen(header)); 
    if(n<0){
        perror("ERROR writing from socket"); 
        exit(0); 
    }
}

int receive(int sockfd, char *response){
    char buffer[BUFMAX];
    int n; 
    bzero(buffer, BUFMAX-1); 
    n=read(sockfd, buffer, BUFMAX); 
    if(n<0){
        perror("ERROR reading from socket"); 
        exit(0);  
    }
    buffer[n]=0;
    if(response){
        strcpy(response, buffer); 
    }
    return n; 
}