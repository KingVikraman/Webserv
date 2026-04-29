#include "ConfigParse.hpp"
#include "Config.hpp"

configParse::configParse(const std::vector<std::string> &tokens) : tokens(tokens), pos(0) {}

std::string configParse::peek() const
{
	if (pos >= tokens.size())
		throw std::runtime_error("Unexpected end of configuration");
	return tokens[pos];
}

std::string configParse::next()
{
	if (pos >= tokens.size())
		throw std::runtime_error("Unexpected end of configuration");
	return tokens[pos++];
}

void configParse::expect(const std::string &expected)
{
	if (peek() != expected)
		throw std::runtime_error("Expected '" + expected + "' but got: '" + peek() + "'");
	next();
}

void configParse::parseErrorPage(ServerConfig &server)
{
	next();
	std::vector<int> codes;

	while (peek() != ";")
	{
		std::string tok = next();
		if (peek() == ";")
		{
			if (codes.empty())
				throw std::runtime_error("error_page requires at least one status code");
			for (size_t i = 0; i < codes.size(); ++i)
				server.error_pages[codes[i]] = tok;
			expect(";");
			return;
		}

		for (size_t i = 0; i < tok.size(); ++i)
		{
			if (tok[i] < '0' || tok[i] > '9')
				throw std::runtime_error("Invalid status code in error_page: " + tok);
		}
		codes.push_back(std::atoi(tok.c_str()));
	}

	throw std::runtime_error("error_page missing target URI");
}

void configParse::parseReturn(LocationConfig &location)
{
	next();
	if (peek() == ";")
		throw std::runtime_error("return requires URL or code URL");

	std::string first = next();
	if (peek() == ";")
	{
		location.return_code = 302;
		location.return_url = first;
		location.has_return = true;
		expect(";");
		return;
	}

	std::string second = next();
	for (size_t i = 0; i < first.size(); ++i)
	{
		if (first[i] < '0' || first[i] > '9')
			throw std::runtime_error("Invalid return code: " + first);
	}

	location.return_code = std::atoi(first.c_str());
	location.return_url = second;
	location.has_return = true;
	expect(";");
}

void configParse::parseLocation(ServerConfig &server)
{
	LocationConfig location;

	if (peek() == ";")
		throw std::runtime_error("missing location path");

	location.path = next();
	expect("{");
	
	while (pos < tokens.size() && peek() != "}")
	{
		if (peek() == "allow_methods")
		{
			next();
			if (peek() == ";")
				throw std::runtime_error("missing allow_methods values in location");
			while (peek() != ";")
			{
				location.methods.push_back(next());
			}
			expect(";");
		}

		else if (peek() == "root")
		{
			next();
			if (peek() == ";")
				throw std::runtime_error("missing root in location");
			location.root = next();
			expect(";");
		}
		else if (peek() == "upload_store")
		{
			next();
			if (peek() == ";")
				throw std::runtime_error("missing upload_store in location");
			location.upload_store = next();
			expect(";");
		}
		else if (peek() == "index")
		{
			next();
			if (peek() == ";")
				throw std::runtime_error("missing index in location");
			while (peek() != ";")
			{
				location.index.push_back(next());
			}
			expect(";");
		}
		else if (peek() == "autoindex")
		{
			next();
			if (peek() == "on")
				location.autoindex = true;
			else if (peek() == "off")
				location.autoindex = false;
			else
				throw std::runtime_error("autoindex must be 'on' or 'off'");
			next();
			expect(";");
		}
		else if (peek() == "return")
		{
			parseReturn(location);
		}
		else
			throw std::runtime_error("unknown directive '" + peek() + "' in location");
	}

	if (pos >= tokens.size())
		throw std::runtime_error("missing '}' for location block");

	expect("}");
	server.locations.push_back(location);
}

void configParse::parseServer()
{
	ServerConfig server;

	expect("{");
	
	while (pos < tokens.size() && peek() != "}")
	{
		if (peek() == "listen")
		{
			next();
			
			if (peek() == ";")
				throw std::runtime_error("missing value for listen");
			std::string portStr = next();
			for (size_t i = 0; i < portStr.size(); ++i)
			{
				if (portStr[i] < '0' || portStr[i] > '9')
					throw std::runtime_error("invalid listen port: " + portStr);
			}
			server.port = std::atoi(portStr.c_str());
			expect(";");
		}
		else if (peek() == "server_name")
		{
			next();

			if (peek() == ";")
				throw std::runtime_error("missing server name");
			while (peek() != ";")
			{
				server.server_names.push_back(next());
			}
			expect(";");
		}
		else if (peek() == "root")
		{
			next();

			if (peek() == ";")
				throw std::runtime_error("missing root address");
			std::string rootStr = next();
			server.root = rootStr;
			expect(";");
		}
		else if (peek() == "index")
		{
			next();

			if (peek() == ";")
				throw std::runtime_error("missing index");
			while (peek() != ";")
			{
				server.index.push_back(next());
			}
			expect(";");
		}
		else if (peek() == "client_max_body_size")
		{
			next();
			
			if (peek() == ";")
				throw std::runtime_error("missing value for client_max_body_size");
			std::string client_sizeStr = next();
			for (size_t i = 0; i < client_sizeStr.size(); ++i)
			{
				if (client_sizeStr[i] < '0' || client_sizeStr[i] > '9')
					throw std::runtime_error("invalid client_max_body_size: " + client_sizeStr);
			}
			server.client_max_body_size = std::atoi(client_sizeStr.c_str());
			expect(";");
		}
		else if (peek() == "error_page")
		{
			parseErrorPage(server);
		}
		else if (peek() == "location")
		{
			next();
			parseLocation(server);
		}
		else
			throw std::runtime_error("unknown directive '" + peek() + "'");
	}

	if (pos >= tokens.size())
		throw std::runtime_error("missing '}' for server block");

	if (server.port < 0)
		throw std::runtime_error("server block missing required listen directive");
	if (server.root.empty())
		throw std::runtime_error("server block missing required root directive");

	expect("}");
	servers.push_back(server);
}

void configParse::parse()
{
	if (tokens.empty())
		throw std::runtime_error("empty configuration");

	while (pos < tokens.size())
	{
		if (peek() == "server")
		{
			next();
			parseServer();
		}
		else
			throw std::runtime_error("unknown top-level directive '" + peek() + "'");
	}

	if (servers.empty())
		throw std::runtime_error("no server blocks found");
}

const std::vector<ServerConfig>& configParse::getServers() const
{
	return servers;
}