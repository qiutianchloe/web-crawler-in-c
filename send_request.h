/*functions to send the request to the host*/
char* send_request_and_get_response(url_t* url); 
struct hostent* convert_host_to_ip(char * url);
struct sockaddr_in construct_server_add(struct hostent *server); 
int create_socket();  
void connect_to_server(int sockfd, struct sockaddr_in serv_addr); 
char *generate_the_header(char* request_info, char* version, char* user_agent, char* accept, url_t* url); 
void send_request(int sockfd, char* header); 
int receive(int sockfd, char *response); 