#ifndef CRYPTO_HPP
# define CRYPTO_HPP

# include "common.hpp"

std::array<uint8_t, AES_KEY_LEN> deriveAesKey(const std::string& secret, const std::array<uint8_t, SALT_LEN>& salt);
std::vector<uint8_t> aesGcmEncrypt(const std::vector<uint8_t>& plaintext, const std::array<uint8_t, AES_KEY_LEN>& key, std::array<uint8_t, GCM_IV_LEN>& ivOut);
std::vector<uint8_t> aesGcmDecrypt(const std::vector<uint8_t>& ciphertextAndTag, const std::array<uint8_t, AES_KEY_LEN>& key, const std::array<uint8_t, GCM_IV_LEN>& iv);



#endif