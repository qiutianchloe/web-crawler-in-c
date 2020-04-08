# specifying the C Compiler for make to use
CC     = gcc

# exe name and a list of object files that make up the program
EXE    = crawler
OBJ    = crawler.o deal_response.o send_request.o parse_url.o

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ)

# object files for the modules needed by our program
parse_url.o: parse_url.h
send_request.o: send_request.h parse_url.h
deal_response.o: parse_url.h send_request.h deal_response.h

clean:
	rm -f $(OBJ) $(EXE)
