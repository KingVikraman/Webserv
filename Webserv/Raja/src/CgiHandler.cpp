#include "../includes/CgiHandler.hpp"


CgiHandler::CgiHandler()
{

}

CgiHandler::~CgiHandler()
{

}
std::map<std::string, std::string> CgiHandler::_buildEnvironment(
	HttpRequest& req,
	const std::string& script_path) const
{
	std::map<std::string, std::string>env;

	//The CGI varialble that are needed
	env["REQUEST_METHOD"] = req.getMethodStr();
	env["SCRIPT_FILENAME"] = script_path;
	env["REDIRECT_STATUS"] = "200";

	//Query string (extract from path if present)
	std::string path = req.getPath();
	size_t query_pos = path.find('?');
	if (query_pos != std::string::npos)
	{
		env["QUERY_STRING"] = path.substr(query_pos + 1);
	}
	else
	{
		env["QUERY_STRING"] = "";
	}
	
	//Content Info
	std::string content_length = req.getHeader("content-length");
	if (!content_length.empty())
	{
		env["CONTENT_LENGTH"] = content_length;
	}
	else
	{
		env["CONTENT_LENGTH"] = "0";
	}
	std::string content_type = req.getHeader("content-type");
	if (!content_type.empty())
	{
		env["CONTENT_TYPE"] = content_type;
	}
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "Webserv/1.0";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";

	std::string host = req.getHeader("host");
	if (!host.empty())
	{
		env["HTTP_HOST"] = host;
	}
	std::string user_agent = req.getHeader("user-agent");
	if (!user_agent.empty())
	{
		env["HTTP_USER_AGENT"] = user_agent;
	}
	return env;
}

char** CgiHandler::_mapToCharArray(const std::map<std::string, std::string> &env) const
{
	char** result = new char*[env.size() + 1];
	size_t i = 0;

	for (std::map<std::string, std::string>::const_iterator it = env.begin();
	it != env.end(); ++it)
	{
		std::string env_str = it->first + "=" + it->second;
		result[i] = new char[env_str.length() + 1];
		std::strcpy(result[i], env_str.c_str());
		i++;
	}

	result[i] = NULL;
	return result;
}

void CgiHandler::_freeCharArray(char** arr) const
{
	for (size_t i = 0; arr[i] != NULL; i++)
	{
		delete[] arr[i];
	}
	delete[] arr;
}

std::string CgiHandler::_readFromPipe(int fd) const
{
	std::string output;
	char buffer[1024];
	ssize_t bytes;

	while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
	{
		output.append(buffer, bytes);
	}
	return output;
}

std::string CgiHandler:: execute(const std::string& script_path, HttpRequest & req) const
{
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
	{
		std::cerr << "Pipe creation Failed" << std::endl;
		return "";
	}

	pid_t pid = fork();

	if(pid < 0)
	{
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		std::cerr << "Fork Failed" << std::endl;
		return "";
	}

	if (pid == 0)
	{

		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[1]);

		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[0]);

		std::map<std::string, std::string> env_map = _buildEnvironment(req, script_path);
		char** env_vars = _mapToCharArray(env_map);

		char* args[] = {(char*)"/usr/bin/php-cgi", (char*)script_path.c_str(), NULL};

		execve(args[0], args, env_vars);

		_freeCharArray(env_vars);
		std::cerr << "Execve Failed" << std::endl;
		exit(1);
	}
	else
	{
		//Parent Process

		close(pipe_in[0]);
		close(pipe_out[1]);

		// Write request body to CGI stdin
		const std::string& body = req.getBody();
		if (!body.empty())
		{
			write(pipe_in[1], body.c_str(), body.length());
		}
		close(pipe_in[1]);

		// Read CGI output
		std::string output = _readFromPipe(pipe_out[0]);
		close(pipe_out[0]);

		// Wait for child
		int status;
		waitpid(pid, &status, 0);

		return output;
	}
}