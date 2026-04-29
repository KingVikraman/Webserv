#include "Config.hpp"
#include "ConfigParse.hpp"

LocationConfig::LocationConfig() : autoindex(false), has_return(false), return_code(302) {}

ServerConfig::ServerConfig() : port(-1), client_max_body_size(0) {}

std::string readFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: Cannot open file " << filename << "\n";
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::vector<std::string> tokenize(const std::string &text)
{
	std::vector<std::string> tokens;
	std::istringstream stream(text);
	std::string token;

	while (stream >> token)
	{
		// Handle special characters as separate tokens
		std::string current;
		for (size_t i = 0; i < token.length(); i++)
		{
			char c = token[i];
			if (c == '{' || c == '}' || c == ';')
			{
				if (!current.empty())
				{
					tokens.push_back(current);
					current.clear();
				}
				tokens.push_back(std::string(1, c));
			}
			else
			{
				current += c;
			}
		}
		if (!current.empty())
			tokens.push_back(current);
	}
	return tokens;
}

void printConfig(const std::vector<ServerConfig> &servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "Server " << i << ":\n";
		std::cout << "  Port: " << servers[i].port << "\n";
		std::cout << "  Root: " << servers[i].root << "\n";
		std::cout << "  Client Max Body Size: " << servers[i].client_max_body_size << "\n";
		for (size_t j = 0; j < servers[i].locations.size(); j++)
		{
			std::cout << "  Location " << servers[i].locations[j].path << ":\n";
			std::cout << "    Root: " << servers[i].locations[j].root << "\n";
		}
	}
}

Config::Config() {}

Config::~Config() {}

bool Config::parse(const std::string& config_file)
{
	std::string file_content = readFile(config_file);
	if (file_content.empty())
	{
		std::cerr << "Failed to read config file: " << config_file << "\n";
		return false;
	}

	try
	{
		std::vector<std::string> tokens = tokenize(file_content);
		configParse parser(tokens);
		parser.parse();
		servers = parser.getServers();
		return true;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error parsing config: " << e.what() << "\n";
		return false;
	}
}

const std::vector<ServerConfig>& Config::getServers() const
{
	return servers;
}
