#ifndef COMMON_HPP
# define COMMON_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <bits/stdc++.h>
# include <openssl/sha.h>
# include <bitset>
# include <cstdint>
# include <array>
# include <cctype>
# include <stdexcept>
# include <vector>
# include <ctime>
# include <sys/stat.h>
# include <iomanip>
# include <openssl/evp.h>
# include <openssl/rand.h>
# include <array>

constexpr int SALT_LEN = 16;
constexpr int PBKDF2_ITERATIONS = 600000;

constexpr int AES_KEY_LEN = 32;
constexpr int GCM_IV_LEN  = 12;
constexpr int GCM_TAG_LEN = 16;

#endif