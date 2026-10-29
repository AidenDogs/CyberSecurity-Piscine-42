#include <iostream>
#include <string>

#define PROGRAM_VERSION "1.0"
#define INFECTION_KEY "20ad1e9ba9eef6481f8cbaa9c3bc7fc45df21fb463491fc9f693ebaf09dcb41f"
#define WORK_DIR (std::string(std::getenv("HOME")) + "/infection")

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
	std::cout << "  \033[32m-h, --help\033[0m     Display this help message" << std::endl;
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

void cureInfection(std::string cure)
{

}

void infect(void)
{
	std::cout << WORK_DIR << std::endl;
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
		}
	}
	if (!key.empty())
		cureInfection(key);
	else
		infect();
}


int main (int argc, char **argv)
{
	parseArgs(argc, argv);
	return (0);
}