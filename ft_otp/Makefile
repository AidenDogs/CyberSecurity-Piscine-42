
NAME = ft_otp

SRC = 	cli/main.cpp \
		cli/src/*.cpp

CC = c++

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto

bonus:
	@echo "Starting ft_otp bonus server..."
	cd web && pnpm run dev

both: $(NAME)
	@echo "Starting ft_otp bonus server..."
	cd web && pnpm run dev

fclean:
	rm -rf $(NAME)
	rm -rf ft_otp.key

re: fclean all

.PHONY: all bonus fclean re