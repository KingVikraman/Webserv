#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include <string>
#include "HttpRequest.hpp"

class UploadHandler {
private:
    std::string _generateFilename() const;
    
public:
    UploadHandler();
    ~UploadHandler();
    
    std::string saveFile(const std::string& upload_dir, HttpRequest& req) const;
    bool deleteFile(const std::string& file_path) const;
};

#endif