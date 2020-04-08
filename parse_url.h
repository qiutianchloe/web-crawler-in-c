

#ifndef URL_H
#define URL_H
typedef struct url url_t;
struct url{ 
    char* hostname; 
    char* dir; 
    char* filename; 
    int state; 
};

/*other function*/
char* substring(char* ch,int pos,int length);

url_t *make_new_url(void);
url_t *convert_string_to_struct(char* strin, char* original_host);

char *convert_struct_to_string(url_t* url); 
char* find_the_host(char* url_start_with_host); 
void separate_dir_and_filename(char* url_without_host, url_t* url); 
int check_filename_in_url(char* url_without_host); 
void print_url_struct(url_t* url); 
int check_is_filename(char* last_part);
int find_last_part(char* url_without_host);
/*deal with url*/

int check_with_prev_url(url_t* this_line_url, url_t* urls[], int num_of_url); 
int tow_url_are_same(url_t* url1, url_t* url2); 

#endif