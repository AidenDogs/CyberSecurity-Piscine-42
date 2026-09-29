#include "include/hotp.hpp"

static uint64_t getTotpTimeStep(uint64_t period = 1)
{
	uint64_t now = static_cast<uint64_t>(std::time(nullptr));
	return now / period;
}

static std::array<uint8_t, 8> toBigEndian64(uint64_t value)
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