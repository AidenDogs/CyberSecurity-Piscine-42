
NAME = ft_otp

SRC = 	main.cpp \
		src/*.cpp

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto

bonus: $(NAME)
	@echo "Starting ft_otp bonus server..."
	cd bonus && pnpm run dev	

fclean:
	rm -rf $(NAME)
	rm -rf ft_otp.key

re: fclean all

.PHONY: all bonus fclean re