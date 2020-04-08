/* A simple client program for HTTP request
 */

/*head file*/
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
typedef struct url url_t;

struct url{ 
    char* hostname; 
    char* dir; 
    char* filename; 
    int state; 
};

#define USER_AGENT "joeb"
#define ACCEPT "html/text"
#define PORT 80
#define BUFMAX 10000
#define GETRE "GET /"
#define VERSION "HTTP/1.1"

/*functions to send the request to the host*/
char* the_whole_process2(url_t* url); 
struct hostent* convert_host_to_ip(char * url);
struct sockaddr_in construct_server_add(struct hostent *server); 
int create_socket();  
void connect_to_server(int sockfd, struct sockaddr_in serv_addr); 
char *generate_the_header2(char* request_info, char* version, char* user_agent, char* accept, url_t* url); 
void send_request(int sockfd, char* header); 
int receive(int sockfd, char *response); 

/*functions to deal with the response*/ 
int deal_with_resource(char* response,url_t* urls[], int num_of_url, char* hostname);
int check_status(char* p); 
int deal_with_200(char* p, url_t* urls[], int num_of_url, char* host); 
char* check_url_in_line(char* p); 
int find_the_start_content(char* p); 
int find_tag_a(char* p, int start_pos_count); 
int check_hyperlink(char* p, int start_a);
int find_url_length(char* p, int has_link);
int find_last_part(char* url_without_host);


/*deal with url*/
url_t *make_new_url(void); 
int check_with_prev_url(url_t* this_line_url, url_t* urls[], int num_of_url); 
url_t *convert_string_to_struct(char* strin, char* original_host);
char *convert_struct_to_string(url_t* url); 
char* find_the_host(char* url_start_with_host); 
void separate_dir_and_filename(char* url_without_host, url_t* url); 
int check_filename_in_url(char* url_without_host); 
void print_url_struct(url_t* url); 
int tow_url_are_same(url_t* url1, url_t* url2); 
int check_is_filename(char* last_part);


/*other function*/
char* substring(char* ch,int pos,int length);



int main(int argc, char **argv){
    //get the input
    if(argc!=2){
        printf("input the domain address!\n");
        return 0; 
    }
    char* domain_add = argv[1];
    url_t *domain_url  = convert_string_to_struct(domain_add, "web1.comp30023"); 
    int num_of_url=1; 
    // char* urls[120]; 
    url_t *urls[120]; 
    // urls[0] = domain_add; 
    urls[0] = domain_url; 
    char* response; 


    int checked = 0; 
    int unchecked = 1; 
    int is_checking = 0; //the current url the socket is sending the request to 
    while(unchecked>0 && num_of_url<100){
        response = the_whole_process2(urls[is_checking]);
        int new_web=deal_with_resource(response, urls, num_of_url,urls[is_checking]->hostname);
        is_checking+=1; 
        num_of_url = num_of_url+new_web;
        unchecked = unchecked+new_web-1;
        checked = checked+1;  
    }

    int counter=0; 
    for(counter=0; counter<num_of_url; counter++){
        char * url_now = convert_struct_to_string(urls[counter]); 
        //print_url_struct(urls[counter]);
        printf("%s\n", url_now); 
        //printf("\n"); 
    }
    free(response); 
	return 0;
}

url_t *convert_string_to_struct(char* strin, char* original_host){
    url_t *url; 
    url = make_new_url(); 
    char* protocal = "http://";
    char* check_prot =substring(strin,0,7); 
    char* start_with_host = "//"; 
    char* check_host =substring(strin,0,2); 

    if(strcmp(protocal, check_prot)==0){
        // started with http://www.example.com
        char* host_without_http = substring(strin,5,(int)strlen(strin)-5);
        // now started with //www.example.com
        url->hostname = find_the_host(host_without_http);

        int length_of_host = (int)strlen(url->hostname); 
        int length_of_url_without_http = (int)strlen(host_without_http); 
        char* url_without_host = substring(host_without_http,length_of_host+2,length_of_url_without_http-length_of_host-2); 
        separate_dir_and_filename(url_without_host, url); 
    }else if(strcmp(start_with_host, check_host)==0){ 
        url->hostname = find_the_host(strin); 
        int length_of_host = (int)strlen(url->hostname); 
        int length_of_url_without_http = (int)strlen(strin); 
        char* url_without_host = substring(strin,length_of_host+2,length_of_url_without_http-length_of_host-2); 
        separate_dir_and_filename(url_without_host, url); 
    }else{
        url->hostname = original_host; 
        separate_dir_and_filename(strin, url); 
    }
    //we need to find the host 

    return url; 
} 

char* find_the_host(char* url_start_with_host){
    char* hostname; 
    hostname=substring(url_start_with_host,2,(int)strlen(url_start_with_host)-2);
    int counter; 
    for(counter=0; counter<(int)strlen(hostname); counter++){
        if(hostname[counter]=='/'){
            //printf("%s\n", substring(hostname,0,counter)); 
            return substring(hostname,0,counter); 
        }
    }
    return hostname; 
}
void separate_dir_and_filename(char* url_without_host, url_t* url){
    int length_without_host = (int)strlen(url_without_host); 
    if(length_without_host==0){
        url->dir = ""; 
        url->filename = ""; 
    }else if(length_without_host==1){
        url->dir = ""; 
        url->filename = ""; 
    }else{
        if(strcmp(substring(url_without_host,0,2), "//")==0){
            url->dir = ""; 
            if(strcmp(substring(url_without_host,length_without_host-1,1), "/")!=0||length_without_host==2){
                url->filename = substring(url_without_host,2,length_without_host-2);     
            }else{
                url->filename = substring(url_without_host,2,length_without_host-3); 
            }
        }else{
            int has_filename = check_filename_in_url(url_without_host); 
            //开头只有一条杠
            if(has_filename!=-1){
                //最后一部分是以两条杠结尾的
                url->dir = substring(url_without_host,1,has_filename-1);
                if(strcmp(substring(url_without_host,length_without_host-1,1), "/")!=0||length_without_host==2){
                    url->filename = substring(url_without_host,has_filename+2,length_without_host-has_filename-2);     
                }else{
                    url->filename = substring(url_without_host,has_filename+2,length_without_host-has_filename-3);
                } 
            }else{
                int start_of_last_part = find_last_part(url_without_host); 
                int length_of_lastpart = length_without_host-start_of_last_part-1;
                char* last_part = substring(url_without_host,start_of_last_part+1, length_of_lastpart); 
                int is_filename = check_is_filename(last_part); 
                if(is_filename==-1){
                    url->filename = "";  
                    if(strcmp(substring(url_without_host,length_without_host-1,1), "/")!=0){
                        url->dir = substring(url_without_host,1,length_without_host-1);  
                    }else{
                        url->dir = substring(url_without_host,1,length_without_host-2); 
                    }
                }else{
                    if(strcmp(substring(last_part,length_of_lastpart-1,1), "/")!=0){
                        url->filename = substring(last_part,0,length_of_lastpart);  
                    }else{
                        url->filename = substring(last_part,0,length_of_lastpart-1); 
                    }
                    if(start_of_last_part==0){
                        url->dir = ""; 
                    }else{
                        url->dir = substring(url_without_host, 1, start_of_last_part-1); 
                    }
                }
            }
        }
    }
}
int check_filename_in_url(char* url_without_host){
    int i; 
    char* file_symble = "//"; 
    for(i = 0; i<=(int)strlen(url_without_host)-2; i++){
        char* subst = substring(url_without_host,i,2);
        if(strcmp(file_symble, subst) == 0){
            return i; 
        }
    }
    return -1; 
}

int check_is_filename(char* last_part){
    int i; 
    char* file_symble = ".";
    for(i = 0; i<(int)strlen(last_part); i++){
        char* subst = substring(last_part,i,1);
        if(strcmp(file_symble, subst) == 0){
            return i; 
        }
    }
    return -1; 
}
int find_last_part(char* url_without_host){
    int i; 
    char* symble= "/"; 
    for(i=(int)strlen(url_without_host)-2; i>=0; i--){
        char* subst = substring(url_without_host,i,1);
        if(strcmp(symble, subst) == 0){
            return i; 
        }
    }
    return -1; 
}
void print_url_struct(url_t* url){
    printf("hostname: %s ", url->hostname); 
    printf("dir: %s ", url->dir);
    printf("filename:%s\n", url->filename);  
}
char *convert_struct_to_string(url_t* url){
    char str[500];
    //char *header = generate_the_header("GET /","", "HTTP/1.1", domain_add, "tq", "html/text"); 
    strcpy (str,"http://");
    strcat(str,url->hostname);
    if(strcmp(url->dir, "")!=0){
        strcat(str,"/");
    }
    strcat (str,url->dir);
    if(strcmp(url->filename, "")!=0){
        strcat (str,"//");
    }
    strcat (str,url->filename);
    
    //printf("%s\n", request); 
    char *strin =(char*)calloc(500,sizeof(char*)); 
    if(strin){
        strcpy(strin,str); 
    }
    return strin; 
}
url_t
*make_new_url(void){
    /* initialise the graph */
    
    url_t *url;
    url = (url_t*)malloc(sizeof(*url));
    assert(url!=NULL);
    url->dir=NULL; 
    url->filename=NULL; 
    url->hostname = NULL; 
    url->state = 0; 
    
    return url;
}
/********************************************************************************************************************/
/*functions to send the request to the host and receive the response*/


char* the_whole_process2(url_t* url){
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
    char *header = generate_the_header2("GET /", "HTTP/1.1", USER_AGENT, ACCEPT, url); 
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
char *generate_the_header2(char* request_info, char* version, char* user_agent, char* accept, url_t* url){
    char request[300];
    //char *header = generate_the_header("GET /","", "HTTP/1.1", domain_add, "tq", "html/text"); 
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

char* substring(char* ch,int pos,int length){  
    //copy the function from https://www.cnblogs.com/guanglili/p/9489268.html
    char* pch=ch;  
    char* subch=(char*)calloc(sizeof(char),length+1);  
    int i;  
    pch=pch+pos;   
    for(i=0;i<length;i++)  {  
        subch[i]=*(pch++);  
    }   
    return subch;
}

/********************************************************************************************************************/
//these are the function to deal with the response
int deal_with_resource(char* response,url_t* urls[], int num_of_url, char* host){
    char *p;
    int new_web=0; 

    //提取状态码
    p=strtok(response,"\r\n"); 
    int status_code = check_status(p); 

    //判断状态码
    if(status_code==200){
        new_web = deal_with_200(p, urls, num_of_url, host); 
    }else{
        //printf("we didnt get the web successfully\n"); 
        //so far, do nothing
    }
    return new_web; 
}

int deal_with_200(char* p, url_t* urls[], int num_of_url,char* host){
    int new_url_num = 0; 
    while(p!= NULL) {
        char* this_line_url = check_url_in_line(p); 
        if(this_line_url!=NULL){
            url_t* this_url_stru=convert_string_to_struct(this_line_url, host);
            int not_same_page = check_with_prev_url(this_url_stru, urls, num_of_url); 
            if(not_same_page==1){
                //check this url and the previous urls instore before
                //printf("%s\n", this_line_url); 
                urls[num_of_url] = this_url_stru;
                num_of_url = num_of_url+1; 
                new_url_num++; 
            }
        }
        p = strtok(NULL, "\r\n");
    }
    //printf("we get website page successfully\n");
    return new_url_num; 
}

char* check_url_in_line(char* p){
    //clear all the empty space
    int start_pos = find_the_start_content(p);
    if(start_pos==-1){
        return NULL;
    }
    //find tag a
    int has_taga =  find_tag_a(p, start_pos); 
    if(has_taga==-1){
        return NULL; 
    }
    //find hyperlink
    int has_link = check_hyperlink(p, has_taga); 
    if(has_link==-1){
        return NULL; 
    }
    //check the length of the url
    int len = find_url_length(p, has_link); 
    //return the url 
    char* this_url = substring(p, has_link, len); 
    return this_url; 
}

/*functions to deal with the response*/ 
int find_the_start_content(char* p){
    int i = 0; 
    for(i=0; i<(int)strlen(p); i++){
        if(p[i]!=' '){
            return i; 
        } 
    }
    return -1; 
}

int check_status(char* p){
    char *status = substring(p,9,3); 
    int status_code = atoi(status) ; 
    return status_code; 
}
int find_tag_a(char* p, int start_pos_count){
    int i; 
    char* taga = "<a"; 
    char* tagca = "<A"; 
    for (i= start_pos_count; i<(int)strlen(p)-start_pos_count-2; i++){
        char* subst = substring(p,i,2); 
        if(strcmp(taga, subst)==0 || strcmp(tagca, subst)==0){
            return i+2; 
        }
    }
    return -1; 
}
int check_hyperlink(char* p, int start_a){
    int i; 
    char* line = "href="; 
    char* linec = "HREF="; 
    for(i = start_a; i<(int)strlen(p)-start_a-5; i++){
        char* subst = substring(p,i,5);
        if(strcmp(line, subst) == 0 || strcmp(linec, subst) == 0){
            return i+6; 
        }
    }
    return -1; 
}

int find_url_length(char* p, int has_link){
    int i; 
    for(i=has_link; i<(int)strlen(p); i++){
        if(p[i]=='\"'){
            return i-has_link; 
        }
    }
    return -1; 
}
/************************************************************************************************************/
//deal with the url 
int check_with_prev_url(url_t* this_line_url, url_t* urls[], int num_of_url){
    int counter; 
    for(counter=0; counter<num_of_url; counter++){
        if(tow_url_are_same(this_line_url, urls[counter])==1){
            return -1; 
        }
    }
    return 1; 
}

int tow_url_are_same(url_t* url1, url_t* url2){
    if(strcmp(url1->hostname, url2->hostname)!=0){
        return -1; 
    }
    if(strcmp(url1->dir, url2->dir)!=0){
        return -1; 
    }
    if(strcmp(url1->filename, url2->filename)!=0){
        return -1; 
    }
    return 1; 
}