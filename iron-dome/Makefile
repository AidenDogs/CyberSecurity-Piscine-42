NAME = ironDome

SRC = 	main.c \
		src/*.c

CC = cc -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME)

fclean:
	rm -rf $(NAME)

re: fclean all

.PHONY: all bonus fclean re