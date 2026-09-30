#include "../include/seed.hpp"

void generateSeed()
{
	std::array<uint8_t, 32> seed{};

	if (RAND_bytes(seed.data(), seed.size()) != 1)
		throw std::runtime_error("RAND_bytes failed");

	const auto tmpPath = std::filesystem::temp_directory_path() / "seed.tmp";
	{
		std::ofstream out(tmpPath, std::ios::binary | std::ios::trunc);

		if (!out)
			throw std::runtime_error("Cannot create temporary seed file");

		for (uint8_t byte : seed)
		{
			out << std::hex
				<< std::setw(2)
				<< std::setfill('0')
				<< static_cast<int>(byte);
		}

		if (!out)
			throw std::runtime_error("Cannot write seed");

		out.close();
	}

	{
		std::ifstream inFile(tmpPath, std::ios::binary);

		if (!inFile)
			throw std::runtime_error("Cannot open temporary seed file");

		storeKey(inFile);
		inFile.close();
	}

	std::filesystem::remove(tmpPath);
}