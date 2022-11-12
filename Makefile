NAME	=	ircserv
CC		=	c++

SRCS	=	main.cpp Message.cpp Server.cpp Client.cpp ServerCMDS.cpp ServerBot.cpp Channel.cpp
OBJS	=	$(SRCS:%.cpp=%.o)

CFLAGS	=	-Wall -Wextra -g #-Werror -std=c++98

PORT		:=	4242
PASS		:=	Wolfsburg

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -fr $(OBJS)

fclean: clean
	rm -fr $(NAME)

re: fclean all

start:
	./$(NAME) $(PORT) $(PASS)

.PHONY: clean fclean re