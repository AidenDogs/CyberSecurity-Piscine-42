
NAME = ft_otp

SRC = 	main.cpp \

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto

fclean:
	rm -rf $(NAME)
	rm -rf ft_otp.key

re: fclean all
