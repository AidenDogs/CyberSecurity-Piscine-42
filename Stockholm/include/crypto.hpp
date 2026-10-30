#ifndef CRYPTO_HPP
# define CRYPTO_HPP

# include "common.hpp"

const std::string INFECTION_KEY = "20ad1e9ba9eef6481f8cbaa9c3bc7fc45df21fb463491fc9f693ebaf09dcb41f";

constexpr int SALT_LEN = 16;
constexpr int AES_KEY_LEN = 32;
constexpr int GCM_IV_LEN  = 12;
constexpr int GCM_TAG_LEN = 16;

std::array<uint8_t, AES_KEY_LEN> deriveAesKey(const std::string& secret, const std::array<uint8_t, SALT_LEN>& salt);
std::vector<uint8_t> aesGcmEncrypt(const std::vector<uint8_t>& plaintext, const std::array<uint8_t, AES_KEY_LEN>& key, std::array<uint8_t, GCM_IV_LEN>& ivOut);
std::vector<uint8_t> aesGcmDecrypt(const std::vector<uint8_t>& ciphertextAndTag, const std::array<uint8_t, AES_KEY_LEN>& key, const std::array<uint8_t, GCM_IV_LEN>& iv);

#endif