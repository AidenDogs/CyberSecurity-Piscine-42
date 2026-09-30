#include "../include/key.hpp"

static std::string isValidKey(std::ifstream& inFile)
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

static std::vector<uint8_t> normalizeKey(const std::vector<uint8_t>& key)
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
