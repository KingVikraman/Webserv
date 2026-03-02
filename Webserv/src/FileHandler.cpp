#include "../includes/FileHandler.hpp"


FileHandler::FileHandler()
{

}

FileHandler::~FileHandler()
{

}

bool FileHandler::fileExists(const std::string &path) const
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool FileHandler::isDirectory(const std::string &path) const
{
	struct stat buffer;
	if (stat(path.c_str(), &buffer) != 0)
	{
		return false;
	}
	return S_ISDIR(buffer.st_mode);
}

size_t FileHandler::getFileSize(const std::string &path) const
{
	struct stat buffer;
	if(stat(path.c_str(), &buffer) != 0)
	{
		return 0;
	}
	return buffer.st_size;
}

std::string FileHandler::getFileContents(const std::string &path) const
{
	std::ifstream file(path.c_str(), std::ios::binary);

	if(!file.is_open())
	{
		return "";
	}

	std::ostringstream contents;
	contents << file.rdbuf();

	return contents.str();
}

std::string FileHandler::getMimeType(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos)
        return "application/octet-stream";
    
    std::string ext = path.substr(dot_pos + 1);
    
    if (ext == "html" || ext == "htm")
        return "text/html";
    else if (ext == "css")
        return "text/css";
    else if (ext == "js")
        return "application/javascript";
    else if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";
    else if (ext == "png")
        return "image/png";
    else if (ext == "gif")
        return "image/gif";
    else if (ext == "txt")
        return "text/plain";
    else if (ext == "json")
        return "application/json";
    else
        return "application/octet-stream";
}


