#ifndef COMMON_HPP
# define COMMON_HPP

# include <iostream>
# include <string>
# include <filesystem>
# include <algorithm>
# include <cstring>
# include <fstream>
# include <string_view>
# include <vector>
# include <cstdint>
# include <stdexcept>
# include <iterator>
# include <openssl/evp.h>
# include <openssl/rand.h>

const std::string PROGRAM_VERSION = "1.0";
const std::string INFECTION_DIR = (std::string(std::getenv("HOME")) + "/infection/");

#endif