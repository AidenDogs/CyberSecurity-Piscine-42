#include "../include/hex.hpp"

static uint8_t hexCharToByte(char c)
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