#ifndef CRYPTO_HPP
# define CRYPTO_HPP

# include "common.hpp"

const std::string INFECTION_KEY = "20ad1e9ba9eef6481f8cbaa9c3bc7fc45df21fb463491fc9f693ebaf09dcb41f";

void encryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& key);
void decryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& key);

#endif