NAME = ironDome

SRC = 	main.c \
		src/*.c

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME)

fclean:
	rm -rf $(NAME)

re: fclean all

.PHONY: all bonus fclean re