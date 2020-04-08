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

#include "parse_url.h"
#include "send_request.h"
#include "deal_response.h"

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
        response = send_request_and_get_response(urls[is_checking]);
        //printf("%s\n", response); 
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