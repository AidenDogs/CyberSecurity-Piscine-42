NAME = stockholm

SRC = 	main.cpp \
		src/*.cpp

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto


fclean:
	rm -rf $(NAME)

re: fclean all

.PHONY: all bonus fclean re