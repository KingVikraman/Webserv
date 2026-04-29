#include "CgiHandler.hpp"
#include <fcntl.h>
#include <signal.h>
#include <ctime>
#include <limits.h>


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
	static const int CGI_TIMEOUT_SECONDS = 5;
	static const char* CGI_TIMEOUT_MARKER = "__CGI_TIMEOUT__";

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
		close(pipe_in[0]);
		close(pipe_in[1]);

		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);
		close(pipe_out[0]);

		std::string exec_script_path = script_path;
		char resolved[PATH_MAX];
		if (realpath(script_path.c_str(), resolved) != NULL)
			exec_script_path = resolved;
		std::map<std::string, std::string> env_map = _buildEnvironment(req, exec_script_path);
		char** env_vars = _mapToCharArray(env_map);

		// Run CGI in its own directory so relative file paths work.
		size_t slash_pos = exec_script_path.find_last_of('/');
		if (slash_pos != std::string::npos)
		{
			std::string script_dir = exec_script_path.substr(0, slash_pos);
			if (!script_dir.empty() && chdir(script_dir.c_str()) != 0)
			{
				_freeCharArray(env_vars);
				std::cerr << "chdir Failed" << std::endl;
				exit(1);
			}
		}

		char* args_usr[] = {(char*)"/usr/bin/php-cgi", (char*)"-f", (char*)exec_script_path.c_str(), NULL};
		execve(args_usr[0], args_usr, env_vars);

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

		// Read CGI output with timeout protection
		int flags = fcntl(pipe_out[0], F_GETFL, 0);
		if (flags != -1)
			fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK);

		std::string output;
		char buffer[1024];
		int status;
		time_t start = time(NULL);
		bool timed_out = false;

		while (true)
		{
			ssize_t bytes = read(pipe_out[0], buffer, sizeof(buffer));
			if (bytes > 0)
				output.append(buffer, bytes);

			pid_t w = waitpid(pid, &status, WNOHANG);
			if (w == pid)
				break;
			if (w < 0)
				break;

			if (time(NULL) - start >= CGI_TIMEOUT_SECONDS)
			{
				timed_out = true;
				kill(pid, SIGKILL);
				waitpid(pid, &status, 0);
				break;
			}
			usleep(10000);
		}

		// Drain remaining output after child exits/killed.
		while (true)
		{
			ssize_t bytes = read(pipe_out[0], buffer, sizeof(buffer));
			if (bytes <= 0)
				break;
			output.append(buffer, bytes);
		}
		close(pipe_out[0]);

		if (timed_out)
			return CGI_TIMEOUT_MARKER;

		return output;
	}
}
