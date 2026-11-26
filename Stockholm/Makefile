NAME = stockholm

SRC = 	main.cpp \
		src/*.cpp

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto

keys:
	openssl genrsa -out private.pem 2048
	openssl rsa -in private.pem -pubout -out public.pem

fclean:
	rm -rf $(NAME) private.pem public.pem key-used-for-infection-DO-NOT-DELETE.key

re: fclean all

.PHONY: all keys fclean re