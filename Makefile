SRC = main.cpp ./parsing/Config.cpp server.cpp request.cpp client.cpp  \
		response.cpp ./response/get_method.cpp ./err/get_handle_errors.cpp \
		./response/get_send_response.cpp ./response/delete_mothod.cpp  ./response/post_method.cpp\
		./err/error_utlis.cpp cgi/cgi.cpp \

CFLAGS =  -std=c++98 -Wall -Wextra -Werror
CPP = c++
NAME = server
CLIENT = client

all: $(NAME)

$(NAME): $(SRC)
	@$(CPP) $(CFLAGS) $(SRC) -o $(NAME) 

clean:

fclean : clean
	@rm -rf $(NAME)
	@rm -rf $(CLIENT)


re : fclean all

client: client.cpp
	@$(CPP) $(CFLAGS) client.cpp -o client

run: 
	./server config/config_file.conf 
	

.PHONY: all clean fclean re