
#include "parse_url.h"

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
        strcat (str,"/");
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