#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>



class FileHandler 
{
	private:
		std::string _getMimeType(const std::string &path) const;

	public:
		FileHandler();
		~FileHandler();

		bool fileExists(const std::string &path) const;
		size_t getFileSize(const std::string &path) const;
		std::string getFileContents(const std::string &path) const;
		std::string getMimeType(const std::string &path)const;
		bool isDirectory(const std::string &path) const;
};


#endif