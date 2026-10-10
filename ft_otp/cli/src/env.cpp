#include "../include/env.hpp"

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