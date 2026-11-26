#ifndef CRYPTO_HPP
# define CRYPTO_HPP

# include "common.hpp"
# include <openssl/evp.h>
# include <openssl/pem.h>
# include <openssl/rsa.h>
# include <openssl/err.h>

void encryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& key);
void decryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& key);

std::array<unsigned char, 32> generateEncryptionKey();
std::string decryptRSA(const std::vector<unsigned char>& encrypted, const std::string& privateKeyPath);
std::vector<unsigned char> encryptRSA(const std::string& message, const std::string& publicKeyPath);

#endif