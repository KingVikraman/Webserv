#include "UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <unistd.h>

namespace {

static bool _isSafeFilenameChar(char c) {
    return ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '.' || c == '_' || c == '-');
}

static std::string _sanitizeFilename(const std::string& raw_name) {
    std::string base = raw_name;
    size_t slash = base.find_last_of("/\\");
    if (slash != std::string::npos)
        base = base.substr(slash + 1);

    std::string safe;
    for (size_t i = 0; i < base.size(); ++i) {
        char c = base[i];
        if (_isSafeFilenameChar(c))
            safe += c;
        else if (c == ' ')
            safe += '_';
    }

    if (safe.empty())
        return "";

    return safe;
}

static std::string _extractMultipartFilename(const std::string& body) {
    size_t name_pos = body.find("filename=\"");
    if (name_pos == std::string::npos)
        return "";

    size_t begin = name_pos + 10;
    size_t end = body.find("\"", begin);
    if (end == std::string::npos || end <= begin)
        return "";

    return body.substr(begin, end - begin);
}

static bool _extractMultipartContent(HttpRequest& req, std::string& out_content) {
    if (!req.getMultiformFlag())
        return false;

    std::string boundary = req.getBoundary();
    if (boundary.empty())
        return false;

    std::string body = req.getBody();
    std::string delimiter = "--" + boundary;

    size_t part_start = body.find(delimiter);
    if (part_start == std::string::npos)
        return false;

    size_t headers_end = body.find("\r\n\r\n", part_start);
    if (headers_end == std::string::npos)
        return false;

    size_t data_start = headers_end + 4;
    size_t data_end = body.find("\r\n" + delimiter, data_start);
    if (data_end == std::string::npos)
        return false;

    out_content = body.substr(data_start, data_end - data_start);
    return true;
}

}

UploadHandler::UploadHandler() {}
UploadHandler::~UploadHandler() {}

std::string UploadHandler::_generateFilename() const {
    std::ostringstream oss;
    oss << "upload_" << time(NULL);
    return oss.str();
}

std::string UploadHandler::saveFile(const std::string& upload_dir, HttpRequest& req) const {
    std::string body_content;
    std::string filename;
    std::string original_name = _extractMultipartFilename(req.getBody());
    std::string safe_name = _sanitizeFilename(original_name);

    if (!_extractMultipartContent(req, body_content))
        body_content = req.getBody();

    if (!safe_name.empty()) {
        filename = safe_name;
    } else {
        filename = _generateFilename();
    }

    std::string separator = "/";
    if (!upload_dir.empty() && upload_dir[upload_dir.size() - 1] == '/')
        separator = "";

    std::string full_path = upload_dir + separator + filename;
    int counter = 1;
    while (access(full_path.c_str(), F_OK) == 0) {
        std::ostringstream oss;
        size_t dot = filename.find_last_of('.');
        if (dot == std::string::npos)
            oss << filename << "_" << counter;
        else
            oss << filename.substr(0, dot) << "_" << counter << filename.substr(dot);
        filename = oss.str();
        full_path = upload_dir + separator + filename;
        ++counter;
    }
    
    std::ofstream file(full_path.c_str(), std::ios::binary);
    if (!file.is_open())
        return "";
    
    file.write(body_content.c_str(), body_content.size());
    file.close();
    
    return filename;
}

bool UploadHandler::deleteFile(const std::string& file_path) const {
    return (unlink(file_path.c_str()) == 0);
}