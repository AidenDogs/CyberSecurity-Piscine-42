#include "otp.hpp"

uint64_t getTotpTimeStep(uint64_t period = 1)
{
	uint64_t now = static_cast<uint64_t>(std::time(nullptr));
	return now / period;
}

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

uint8_t hexCharToByte(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	throw std::invalid_argument("Invalid hex char");
}

std::vector<uint8_t> hexStringToBytes(const std::string& hex)
{
	std::vector<uint8_t> bytes;
	bytes.reserve(hex.size() / 2);

	if (hex.size() % 2 != 0)
		throw std::invalid_argument("Hex string length must be even");
	
	for (size_t i = 0; i < hex.size(); i += 2)
	{
		uint8_t high = hexCharToByte(hex[i]);
		uint8_t low = hexCharToByte(hex[i + 1]);
		bytes.push_back((high << 4) | low);
	}
	return (bytes);
}

std::string getEnvValue(const std::string& name)
{
	std::ifstream file(".env");

	if (!file.is_open())
		return {};

	std::string line;

	while (std::getline(file, line))
	{
		auto pos = line.find('=');

		if (pos == std::string::npos)
			continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		value.erase(value.find_last_not_of(" \n\r\t") + 1);
		if (key == name)
			return value;
	}

	return {};
}

std::string isValidKey(std::ifstream& inFile)
{
	std::string line, full;
	while (std::getline(inFile, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		full += line;
	}

	for (char c : full)
	{
		if (!std::isxdigit(static_cast<unsigned char>(c)))
		{
			std::cerr << "Invalid key: contains non-hexadecimal characters\nAborting" << std::endl;
			inFile.close();
			exit(1);
		}
	}
	inFile.clear();
	inFile.seekg(0);

	if (full.length() < 64)
	{
		std::cerr << "./ft_otp: error: key must be at least 64 hexadecimal characters\nAborting" << std::endl;
		inFile.close();
		exit(1);
	}
	if (full.length() % 2 != 0)
	{
		std::cerr << "./ft_otp: error: key length must be even length\nAborting" << std::endl;
		inFile.close();
		exit(1);
	}

	return full;
}

std::vector<uint8_t> normalizeKey(const std::vector<uint8_t>& key)
{
	std::vector<uint8_t> block(64, 0);
	if (key.size() > 64)
	{
		unsigned char digest[SHA_DIGEST_LENGTH];
		SHA1(key.data(), key.size(), digest);
		std::copy(digest, digest + SHA_DIGEST_LENGTH, block.begin());
	}
	else
		std::copy(key.begin(), key.end(), block.begin());
	return block;
}

std::string bytesToHexString(const std::vector<uint8_t>& bytes)
{
	static const char hexDigits[] = "0123456789abcdef";
	std::string out;
	out.reserve(bytes.size() * 2);
	for (uint8_t b : bytes)
	{
		out.push_back(hexDigits[b >> 4]);
		out.push_back(hexDigits[b & 0x0F]);
	}
	return out;
}

void storeKey(std::ifstream& inFile)
{
	std::string hexContent = isValidKey(inFile);

	std::vector<uint8_t> rawKey = hexStringToBytes(hexContent);
	std::vector<uint8_t> normalizedKey = normalizeKey(rawKey);

	std::string secret = getEnvValue("AES_SECRET_KEY");
	if (secret.empty())
	{
		std::cerr << "AES_SECRET_KEY absent or empty in .env\nAborting" << std::endl;
		inFile.close();
		exit(1);
	}

	std::array<uint8_t, SALT_LEN> salt{};
	if (RAND_bytes(salt.data(), SALT_LEN) != 1)
	{
		std::cerr << "RAND_bytes failed for salt\nAborting" << std::endl;
		inFile.close();
		exit(1);
	}

	auto aesKey = deriveAesKey(secret, salt);
	std::array<uint8_t, GCM_IV_LEN> iv{};
	std::vector<uint8_t> encrypted;

	try
	{
		encrypted = aesGcmEncrypt(normalizedKey, aesKey, iv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Encryption error: " << e.what() << "\nAborting" << std::endl;
		inFile.close();
		exit(1);
	}

	std::string saltHex = bytesToHexString(std::vector<uint8_t>(salt.begin(), salt.end()));
	std::string ivHex = bytesToHexString(std::vector<uint8_t>(iv.begin(), iv.end()));
	std::string encryptedHex = saltHex + ivHex + bytesToHexString(encrypted);

	std::ofstream keyFile("ft_otp.key");
	if (!keyFile)
	{
		std::cerr << "Unable to open output file\nAborting" << std::endl;
		exit(1);
	}

	keyFile << encryptedHex;
	keyFile.close();

	if (chmod("ft_otp.key", S_IRUSR | S_IWUSR) != 0)
		std::cerr << "Warning: unable to restrict permissions on ft_otp.key" << std::endl;

	inFile.close();
	std::cout << "Key was successfully saved in ft_otp.key" << std::endl;
}

std::array<uint8_t, 8> toBigEndian64(uint64_t value)
{
	std::array<uint8_t, 8> out{};

	for (int i = 0; i < 8; i++)
	{
		out[7 - i] = static_cast<uint8_t>(value >> (i * 8));
	}
	return out;
}

void generateOTP(const std::string& keyFilePath)
{
	std::ifstream otpKey(keyFilePath);
	if (!otpKey)
	{
		std::cerr << "Unable to open file => " << keyFilePath << "\nAborting" << std::endl;
		exit(1);
	}

	std::string fileContent;
	std::getline(otpKey, fileContent);
	otpKey.close();

	const size_t saltHexLen = SALT_LEN * 2;
	const size_t ivHexLen = GCM_IV_LEN * 2;
	if (fileContent.size() < saltHexLen + ivHexLen)
	{
		std::cerr << "Corrupted key file\nAborting" << std::endl;
		exit(1);
	}

	std::vector<uint8_t> saltBytes = hexStringToBytes(fileContent.substr(0, saltHexLen));
	std::array<uint8_t, SALT_LEN> salt{};
	std::copy(saltBytes.begin(), saltBytes.end(), salt.begin());

	std::vector<uint8_t> ivBytes = hexStringToBytes(fileContent.substr(saltHexLen, ivHexLen));
	std::array<uint8_t, GCM_IV_LEN> iv{};
	std::copy(ivBytes.begin(), ivBytes.end(), iv.begin());

	std::vector<uint8_t> cipherBytes = hexStringToBytes(fileContent.substr(saltHexLen + ivHexLen));

	std::string secret = getEnvValue("AES_SECRET_KEY");
	if (secret.empty())
	{
		std::cerr << "AES_SECRET_KEY absent or empty in .env\nAborting" << std::endl;
		exit(1);
	}
	auto aesKey = deriveAesKey(secret, salt);

	std::vector<uint8_t> value;
	try
	{
		value = aesGcmDecrypt(cipherBytes, aesKey, iv);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\nAborting" << std::endl;
		exit(1);
	}

	if (value.size() != 64)
	{
		std::cerr << "Corrupted key file\nAborting" << std::endl;
		exit(1);
	}


	unsigned char ipad[64];
	for (int i = 0; i < 64; i++)
		ipad[i] = 0x36 ^ value[i];

	unsigned char opad[64];
	for (int i = 0; i < 64; i++)
		opad[i] = 0x5C ^ value[i];

	uint64_t timeStep = getTotpTimeStep();
	std::array<uint8_t, 8> bigEdian = toBigEndian64(timeStep);

	std::vector<uint8_t> innerInput;
	innerInput.reserve(64 + 8);
	innerInput.insert(innerInput.end(), ipad, ipad + 64);
	innerInput.insert(innerInput.end(), bigEdian.begin(), bigEdian.end());

	unsigned char hashing[SHA_DIGEST_LENGTH];
	unsigned char *inner_hash = SHA1(innerInput.data(), innerInput.size(), hashing);

	std::vector<uint8_t> outerInput;
	outerInput.reserve(64 + SHA_DIGEST_LENGTH);
	outerInput.insert(outerInput.end(), opad, opad + 64);
	outerInput.insert(outerInput.end(), inner_hash, inner_hash + SHA_DIGEST_LENGTH);

	unsigned char *hmac = SHA1(outerInput.data(), outerInput.size(), hashing);
	unsigned char offset = hmac[19] & 0x0F;

	uint32_t bin_code = (static_cast<uint32_t>(hmac[offset]) << 24)
					| (static_cast<uint32_t>(hmac[offset+1]) << 16)
					| (static_cast<uint32_t>(hmac[offset+2]) << 8)
					|	static_cast<uint32_t>(hmac[offset+3]);

	bin_code = bin_code & 0x7FFFFFFF;
	uint32_t otp = bin_code % 1000000;

	std::cout << std::setfill('0') << std::setw(6) << otp << std::endl;
}

void generateSeed()
{
	std::array<uint8_t, 32> buf{};

	if (RAND_bytes(buf.data(), 32) != 1)
		throw std::runtime_error("RAND_bytes failed");
	//TO-DO aggiungi creazione file temporaneo per store temporaneo del seed generato o soluzione alternativa
	storeKey(tmp);
}

void	exeProgram(char **argv)
{
	if (strcmp(argv[1], "-g") != 0 && strcmp(argv[1], "-k") != 0 && strcmp(argv[1], "-s") != 0)
	{
		std::cerr << "Invalid flag\nAborting" << std::endl;
		exit(1);
	}
	std::string flag = argv[1];
	if (flag == "-g")
	{
		std::ifstream inFile;

		inFile.open(argv[2]);
		if (!inFile)
		{
			std::cerr << "Unable to open file => " << argv[2] << std::endl;
			exit(1);
		}
		storeKey(inFile);
		inFile.close();
	}
	else if (flag == "-k")
		generateOTP(argv[2]);
	else
		generateSeed();
}

int	main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Invalid number of arguments" << std::endl;
		return (1);
	}
	exeProgram(argv);
	return (0);
}
