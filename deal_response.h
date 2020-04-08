#include "parse_url.h"
int deal_with_resource(char* response,url_t* urls[], int num_of_url, char* hostname);
int check_status(char* p); 

int deal_with_200(char* p, url_t* urls[], int num_of_url, char* host); 

char* check_url_in_line(char* p); 
int find_the_start_content(char* p); 
int find_tag_a(char* p, int start_pos_count); 
int check_hyperlink(char* p, int start_a);
int find_url_length(char* p, int has_link);
int find_start_of_link(char* p, int start_link);
