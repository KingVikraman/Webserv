#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP


#include "../../Brian/includes/HttpRequest.hpp"
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

class CgiHandler {

	private:
		std::map<std::string, std::string> _buildEnvironment(HttpRequest& req,
		const std::string& script_path) const;

		std::string _readFromPipe(int fd) const;
		char** _mapToCharArray(const std::map<std::string, std::string>& env) const;
		void _freeCharArray(char** arr) const;
	
	public:
			CgiHandler();
			~CgiHandler();

			std::string execute(const std::string& script_path,
			HttpRequest& req) const;
};

#endif