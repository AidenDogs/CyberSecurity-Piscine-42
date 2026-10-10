Mandatory Part
In the language of your choice, you have to implement a program that allows you to store
an initial password in file, and that is capable of generating a new one time password
every time it is requested.
You can use any library that facilitates the implementation of the algorithm, as long
as it doesn’t do the dirty work, i.e. using a TOTP library is strictly prohibited. Of
course, you can and should use a library or function that allows you to access system
time.
• The executable must be named ft_otp
• Your program must take arguments.
◦ -g: The program receives as argument a hexadecimal key of at least 64 characters. The program stores this key safely in a file called ft_otp.key, which
is encrypted.
◦ -k: The program generates a new temporary password based on the key given
as argument and prints it on the standard output.
• Your program must use the HOTP algorithm (RFC 4226).
• The generated one-time password must be random and must always contain the
same format, i.e. 6 digits.
Below is an example of use:
$ echo -n "NEVER GONNA GIVE YOU UP" > key.txt
$ ./ft_otp -g key.txt
./ft_otp: error: key must be 64 hexadecimal characters.
$ [..]
$ cat key.hex | wc -c
64
$ ./ft_otp -g key.hex
Key was successfully saved in ft_otp.key.
$ ./ft_otp -k ft_otp.key
836492
$ sleep 60
$ ./ft_otp -k ft_otp.key
123518
4
Cybersecurity Piscine ft_otp
You can check if your program is working properly by comparing generated passwords
with Oathtool or any tool of your choice.
oathtool –totp $(cat key.hex)
Please note that the reference software you choose will be used
during your evaluation.
5
Chapter IV
Bonus Part
You can enhance your project with the following features:
• Creation of a QR code with seed generation.
• Creation of a graphic interface.

for the bonus part i wanna run it on a server.ts with an endpoint seed.ts which will execute the program ./ft_otp -s and read the key inside ft_otp.key which is encrypted in aes (the key is in a .env file). Another endpoint qrCode.ts to generate the totp qr code every 30 seconds using the program ./ft_otp -k ft_otp.key. Another endpoint otp.ts that will use the program ./ft_otp -k ft_otp.key and read the standard output to take the otp number and send it to frontend. If you have to modify the frontend files do it. If you don't know some params or names or macros and everything i already have write PLACEHOLDER_ before the name you ll chose. All this backend in node and pnpm

My workspace now
.
├── ft_otp
├── ft_otp.key
├── include
│   ├── common.hpp
│   ├── crypto.hpp
│   ├── env.hpp
│   ├── hex.hpp
│   ├── hotp.hpp
│   ├── key.hpp
│   └── seed.hpp
├── main.cpp
├── Makefile
├── otp.hpp
├── README.md
├── src
│   ├── crypto.cpp
│   ├── env.cpp
│   ├── hex.cpp
│   ├── hotp.cpp
│   ├── key.cpp
│   └── seed.cpp
└── web
    ├── public
    │   ├── app.js
    │   ├── index.html
    │   └── style.css
    └── src

5 directories, 22 files