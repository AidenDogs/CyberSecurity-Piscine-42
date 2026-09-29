#ifndef HEX_HPP
# define HEX_HPP

# include "common.hpp"

std::vector<uint8_t> hexStringToBytes(const std::string& hex);
std::string bytesToHexString(const std::vector<uint8_t>& bytes);


#endif