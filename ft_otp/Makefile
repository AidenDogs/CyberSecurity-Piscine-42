
NAME = ft_otp

SRC = 	main.cpp \


CC = c++
# FLAGS = -std=c++98 -lcurl -lpsl -lzstd -lrtmp -lidn2 -lnghttp2 -lngtcp2 -lnghttp3 -lssl -lcrypto -lbrotli -lz 

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $^ -o $(NAME) -lcrypto

fclean:
	rm -rf $(NAME)
	rm -rf ft_otp.key

re: fclean all
