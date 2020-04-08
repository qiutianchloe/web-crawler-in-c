
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
#include "deal_response.h"
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
    //printf("%s\n", p); 
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
    //find the start of the quote "
    int link_start = find_start_of_link(p, has_link); 
    //char* check = substring(p,link_start,);
    if(link_start==-1){
        return NULL; 
    }
    //check the length of the url
    int len = find_url_length(p, link_start); 
    if(len == -1){
        return NULL; 
    }
    //return the url 
    char* this_url = substring(p, link_start, len); 
    //printf("%s\n", this_url); 
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
    char* line = "href"; 
    char* linec = "HREF"; 
    for(i = start_a; i<(int)strlen(p)-start_a-4; i++){
        char* subst = substring(p,i,4);
        if(strcmp(line, subst) == 0 || strcmp(linec, subst) == 0){
            return i+4; 
        }
    }
    return -1; 
}
int find_start_of_link(char*p, int start_link){
    int i; 
    char* symble = "\""; 
    for(i=start_link;i<(int)strlen(p)-start_link; i++){
        char* subst = substring(p,i,1);
        if(strcmp(symble, subst) == 0){
            return i+1; 
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