### WEEK 2: Static File Handler


## RAJA'S TASKS (Response Pipeline)

**Your Responsibility:** Execute route → Build response → Send to client

**Files you own:**
- `src/FileHandler.{hpp,cpp}`
- `src/HttpResponse.{hpp,cpp}`
- `src/CgiHandler.{hpp,cpp}`
- `src/UploadHandler.{hpp,cpp}`

---


#### Raja Task 1: File Operations (Day 8-9)

**File:** `src/FileHandler.hpp`
```cpp
#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>

class FileHandler {
private:
    std::string _getMimeType(const std::string& path) const;
    
public:
    FileHandler();
    ~FileHandler();
    
    bool fileExists(const std::string& path) const;
    size_t getFileSize(const std::string& path) const;
    std::string getFileContents(const std::string& path) const;
    std::string getMimeType(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
};

#endif
```

**File:** `src/FileHandler.cpp`
```cpp
#include "FileHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}

bool FileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

size_t FileHandler::getFileSize(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0)
        return 0;
    return buffer.st_size;
}

bool FileHandler::isDirectory(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0)
        return false;
    return S_ISDIR(buffer.st_mode);
}

std::string FileHandler::getFileContents(const std::string& path) const {
    std::ifstream file(path.c_str(), std::ios::binary);
    
    if (!file.is_open())
        return "";
    
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
```

**Test file:** `tests/test_file_handler.cpp`
```cpp
#include "../src/FileHandler.hpp"
#include <iostream>
#include <fstream>

int main() {
    FileHandler fh;
    
    // Create test file
    std::ofstream test("tests/test.html");
    test << "<h1>Test File</h1>";
    test.close();
    
    // Test 1: File exists
    if (fh.fileExists("tests/test.html"))
        std::cout << "✓ File exists check works\n";
    else
        std::cout << "✗ File exists check failed\n";
    
    // Test 2: File size
    size_t size = fh.getFileSize("tests/test.html");
    if (size == 18)
        std::cout << "✓ File size correct: " << size << "\n";
    else
        std::cout << "✗ File size wrong: " << size << " (expected 18)\n";
    
    // Test 3: File contents
    std::string contents = fh.getFileContents("tests/test.html");
    if (contents == "<h1>Test File</h1>")
        std::cout << "✓ File contents correct\n";
    else
        std::cout << "✗ File contents wrong: " << contents << "\n";
    
    // Test 4: MIME type
    std::string mime = fh.getMimeType("tests/test.html");
    if (mime == "text/html")
        std::cout << "✓ MIME type correct: " << mime << "\n";
    else
        std::cout << "✗ MIME type wrong: " << mime << "\n";
    
    // Test 5: Non-existent file
    if (!fh.fileExists("tests/nonexistent.txt"))
        std::cout << "✓ Non-existent file check works\n";
    else
        std::cout << "✗ Non-existent file check failed\n";
    
    // Test 6: Various MIME types
    if (fh.getMimeType("test.css") == "text/css")
        std::cout << "✓ CSS MIME type correct\n";
    else
        std::cout << "✗ CSS MIME type wrong\n";
    
    if (fh.getMimeType("test.png") == "image/png")
        std::cout << "✓ PNG MIME type correct\n";
    else
        std::cout << "✗ PNG MIME type wrong\n";
    
    return 0;
}
```

**Success criteria:**
- [ ] All tests pass
- [ ] Can check file existence
- [ ] Can get file size
- [ ] Can read file contents
- [ ] Returns correct MIME types

---