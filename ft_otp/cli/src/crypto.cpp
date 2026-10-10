#include "../include/crypto.hpp"

std::array<uint8_t, AES_KEY_LEN> deriveAesKey(const std::string& secret, const std::array<uint8_t, SALT_LEN>& salt)
{
	std::array<uint8_t, AES_KEY_LEN> key{};
	if (PKCS5_PBKDF2_HMAC(secret.data(), static_cast<int>(secret.size()),
							salt.data(), static_cast<int>(salt.size()),
							600000, EVP_sha256(),
							AES_KEY_LEN, key.data()) != 1)
		throw std::runtime_error("PBKDF2 failed");
	return key;
}

std::vector<uint8_t> aesGcmEncrypt(const std::vector<uint8_t>& plaintext, const std::array<uint8_t, AES_KEY_LEN>& key, std::array<uint8_t, GCM_IV_LEN>& ivOut)
{
	if (RAND_bytes(ivOut.data(), GCM_IV_LEN) != 1)
		throw std::runtime_error("RAND_bytes failed");

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		throw std::runtime_error("EVP_CIPHER_CTX_new failed");

	std::vector<uint8_t> ciphertext(plaintext.size());
	int len = 0, ciphertextLen = 0;

	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
		EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, GCM_IV_LEN, nullptr) != 1 ||
		EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), ivOut.data()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM init failed");
	}

	if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), static_cast<int>(plaintext.size())) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM encrypt failed");
	}
	ciphertextLen = len;

	if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM finalize failed");
	}
	ciphertextLen += len;
	ciphertext.resize(ciphertextLen);

	std::array<uint8_t, GCM_TAG_LEN> tag{};
	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, GCM_TAG_LEN, tag.data()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM get tag failed");
	}
	EVP_CIPHER_CTX_free(ctx);

	ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());
	return ciphertext;
}

std::vector<uint8_t> aesGcmDecrypt(const std::vector<uint8_t>& ciphertextAndTag, const std::array<uint8_t, AES_KEY_LEN>& key, const std::array<uint8_t, GCM_IV_LEN>& iv)
{
	if (ciphertextAndTag.size() < GCM_TAG_LEN)
		throw std::invalid_argument("Corrupted key file: too short");

	size_t ciphertextLen = ciphertextAndTag.size() - GCM_TAG_LEN;
	const uint8_t* ciphertext = ciphertextAndTag.data();
	const uint8_t* tag = ciphertextAndTag.data() + ciphertextLen;

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		throw std::runtime_error("EVP_CIPHER_CTX_new failed");

	std::vector<uint8_t> plaintext(ciphertextLen);
	int len = 0, plaintextLen = 0;

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
		EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, GCM_IV_LEN, nullptr) != 1 ||
		EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM init failed");
	}

	if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext, static_cast<int>(ciphertextLen)) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM decrypt failed");
	}
	plaintextLen = len;

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, GCM_TAG_LEN,
							 const_cast<uint8_t*>(tag)) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("AES-GCM set tag failed");
	}

	int ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
	EVP_CIPHER_CTX_free(ctx);

	if (ret <= 0)
		throw std::runtime_error("AES-GCM authentication failed: key file tampered or wrong secret");

	plaintextLen += len;
	plaintext.resize(plaintextLen);
	return plaintext;
}