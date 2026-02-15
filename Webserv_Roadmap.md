# WEBSERV PROJECT - COMPLETE ROADMAP

**Project:** Build an HTTP/1.1 Server in C++98  
**Team:** Brian (Request Processing) + Raja (Response Generation)  
**Duration:** 6 Weeks  
**Goal:** Never crash. Handle multiple clients. Serve files. Execute CGI.

---

## TABLE OF CONTENTS

- [Phase 0: Foundation (BOTH - Week 1)](#phase-0-foundation-both---week-1)
- [Phase 1: Core Development (SPLIT - Weeks 2-5)](#phase-1-core-development-split---weeks-2-5)
  - [Brian's Tasks (Request Pipeline)](#brians-tasks-request-pipeline)
  - [Raja's Tasks (Response Pipeline)](#rajas-tasks-response-pipeline)
- [Phase 2: Integration (BOTH - Week 6)](#phase-2-integration-both---week-6)
- [Testing Checklist](#testing-checklist)
- [Submission Requirements](#submission-requirements)

---

## PHASE 0: FOUNDATION (BOTH - Week 1)

> ⚠️ **CRITICAL:** You MUST complete this together. No exceptions. Both must understand every line before splitting work.




## PHASE 1: CORE DEVELOPMENT (SPLIT - Weeks 2-5)

> 💡 Now you can work in parallel, but check in daily!

---



## RAJA'S TASKS (Response Pipeline)

**Your Responsibility:** Execute route → Build response → Send to client

**Files you own:**
- `src/FileHandler.{hpp,cpp}`
- `src/HttpResponse.{hpp,cpp}`
- `src/CgiHandler.{hpp,cpp}`
- `src/UploadHandler.{hpp,cpp}`

---

### WEEK 2: Static File Handler

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

### WEEK 3: HTTP Response Builder

#### Raja Task 2: Response Builder (Day 15-17)

**File:** `src/HttpResponse.hpp`
```cpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

class HttpResponse {
private:
    int _status_code;
    std::string _status_message;
    std::map<std::string, std::string> _headers;
    std::string _body;
    
    std::string _getStatusMessage(int code) const;
    
public:
    HttpResponse();
    ~HttpResponse();
    
    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    
    std::string build() const;
    
    // Convenience methods
    static HttpResponse ok(const std::string& body, const std::string& content_type);
    static HttpResponse created(const std::string& body);
    static HttpResponse noContent();
    static HttpResponse notFound();
    static HttpResponse methodNotAllowed();
    static HttpResponse internalError();
    static HttpResponse badRequest();
};

#endif
```

**File:** `src/HttpResponse.cpp`
```cpp
#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status_code(200), _status_message("OK") {}
HttpResponse::~HttpResponse() {}

std::string HttpResponse::_getStatusMessage(int code) const {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

void HttpResponse::setStatus(int code) {
    _status_code = code;
    _status_message = _getStatusMessage(code);
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
    _body = body;
    
    // Auto-set Content-Length
    std::ostringstream oss;
    oss << body.length();
    setHeader("Content-Length", oss.str());
}

std::string HttpResponse::build() const {
    std::ostringstream response;
    
    // Status line
    response << "HTTP/1.1 " << _status_code << " " << _status_message << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    
    // Blank line
    response << "\r\n";
    
    // Body
    response << _body;
    
    return response.str();
}

// Convenience methods
HttpResponse HttpResponse::ok(const std::string& body, const std::string& content_type) {
    HttpResponse resp;
    resp.setStatus(200);
    resp.setHeader("Content-Type", content_type);
    resp.setBody(body);
    return resp;
}

HttpResponse HttpResponse::created(const std::string& body) {
    HttpResponse resp;
    resp.setStatus(201);
    resp.setHeader("Content-Type", "text/plain");
    resp.setBody(body);
    return resp;
}

HttpResponse HttpResponse::noContent() {
    HttpResponse resp;
    resp.setStatus(204);
    return resp;
}

HttpResponse HttpResponse::notFound() {
    HttpResponse resp;
    resp.setStatus(404);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>404 Not Found</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::methodNotAllowed() {
    HttpResponse resp;
    resp.setStatus(405);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::internalError() {
    HttpResponse resp;
    resp.setStatus(500);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    return resp;
}

HttpResponse HttpResponse::badRequest() {
    HttpResponse resp;
    resp.setStatus(400);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
    return resp;
}
```

**Test file:** `tests/test_response.cpp`
```cpp
#include "../src/HttpResponse.hpp"
#include <iostream>

int main() {
    // Test 1: Basic response
    HttpResponse resp1;
    resp1.setStatus(200);
    resp1.setHeader("Content-Type", "text/html");
    resp1.setBody("<h1>Hello!</h1>");
    
    std::string output1 = resp1.build();
    
    if (output1.find("HTTP/1.1 200 OK") != std::string::npos)
        std::cout << "✓ Status line correct\n";
    else
        std::cout << "✗ Status line wrong\n";
    
    if (output1.find("Content-Type: text/html") != std::string::npos)
        std::cout << "✓ Content-Type header present\n";
    else
        std::cout << "✗ Content-Type header missing\n";
    
    if (output1.find("Content-Length: 15") != std::string::npos)
        std::cout << "✓ Content-Length auto-set\n";
    else
        std::cout << "✗ Content-Length wrong\n";
    
    if (output1.find("<h1>Hello!</h1>") != std::string::npos)
        std::cout << "✓ Body present\n";
    else
        std::cout << "✗ Body missing\n";
    
    // Test 2: Convenience method
    HttpResponse resp2 = HttpResponse::ok("Test body", "text/plain");
    std::string output2 = resp2.build();
    
    if (output2.find("200 OK") != std::string::npos)
        std::cout << "✓ ok() method works\n";
    else
        std::cout << "✗ ok() method failed\n";
    
    // Test 3: 404 response
    HttpResponse resp3 = HttpResponse::notFound();
    std::string output3 = resp3.build();
    
    if (output3.find("404 Not Found") != std::string::npos)
        std::cout << "✓ notFound() method works\n";
    else
        std::cout << "✗ notFound() method failed\n";
    
    // Test 4: Multiple headers
    HttpResponse resp4;
    resp4.setHeader("X-Custom-Header", "CustomValue");
    resp4.setHeader("Cache-Control", "no-cache");
    std::string output4 = resp4.build();
    
    if (output4.find("X-Custom-Header: CustomValue") != std::string::npos &&
        output4.find("Cache-Control: no-cache") != std::string::npos)
        std::cout << "✓ Multiple headers work\n";
    else
        std::cout << "✗ Multiple headers failed\n";
    
    return 0;
}
```

**Success criteria:**
- [ ] All tests pass
- [ ] Response format is valid HTTP
- [ ] Content-Length auto-calculated
- [ ] All convenience methods work
- [ ] Can set multiple headers

---

### WEEK 4: CGI Handler

#### Raja Task 3: CGI Execution (Day 22-26)

**File:** `src/CgiHandler.hpp`
```cpp
#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <map>
#include <vector>
#include "HttpRequest.hpp"

class CgiHandler {
private:
    std::map<std::string, std::string> _buildEnvironment(
        const HttpRequest& req,
        const std::string& script_path
    ) const;
    
    std::string _readFromPipe(int fd) const;
    char** _mapToCharArray(const std::map<std::string, std::string>& env) const;
    void _freeCharArray(char** arr) const;
    
public:
    CgiHandler();
    ~CgiHandler();
    
    std::string execute(const std::string& script_path, const HttpRequest& req) const;
};

#endif
```

**File:** `src/CgiHandler.cpp`
```cpp
#include "CgiHandler.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

std::map<std::string, std::string> CgiHandler::_buildEnvironment(
    const HttpRequest& req,
    const std::string& script_path
) const {
    std::map<std::string, std::string> env;
    
    // Required CGI variables
    env["REQUEST_METHOD"] = req.getMethod();
    env["SCRIPT_FILENAME"] = script_path;
    env["REDIRECT_STATUS"] = "200";
    
    // Query string (extract from path if present)
    std::string path = req.getPath();
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
        env["QUERY_STRING"] = path.substr(query_pos + 1);
    } else {
        env["QUERY_STRING"] = "";
    }
    
    // Content info
    std::string content_length = req.getHeader("Content-Length");
    if (!content_length.empty())
        env["CONTENT_LENGTH"] = content_length;
    else
        env["CONTENT_LENGTH"] = "0";
    
    std::string content_type = req.getHeader("Content-Type");
    if (!content_type.empty())
        env["CONTENT_TYPE"] = content_type;
    
    // Server info
    env["SERVER_PROTOCOL"] = req.getVersion();
    env["SERVER_SOFTWARE"] = "webserv/1.0";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    
    // Optional headers
    std::string host = req.getHeader("Host");
    if (!host.empty())
        env["HTTP_HOST"] = host;
    
    std::string user_agent = req.getHeader("User-Agent");
    if (!user_agent.empty())
        env["HTTP_USER_AGENT"] = user_agent;
    
    return env;
}

char** CgiHandler::_mapToCharArray(const std::map<std::string, std::string>& env) const {
    char** result = new char*[env.size() + 1];
    size_t i = 0;
    
    for (std::map<std::string, std::string>::const_iterator it = env.begin();
         it != env.end(); ++it) {
        std::string env_str = it->first + "=" + it->second;
        result[i] = new char[env_str.length() + 1];
        std::strcpy(result[i], env_str.c_str());
        i++;
    }
    
    result[i] = NULL;
    return result;
}

void CgiHandler::_freeCharArray(char** arr) const {
    for (size_t i = 0; arr[i] != NULL; i++) {
        delete[] arr[i];
    }
    delete[] arr;
}

std::string CgiHandler::_readFromPipe(int fd) const {
    std::string output;
    char buffer[1024];
    ssize_t bytes;
    
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        output.append(buffer, bytes);
    }
    
    return output;
}

std::string CgiHandler::execute(const std::string& script_path, const HttpRequest& req) const {
    int pipe_in[2];
    int pipe_out[2];
    
    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) {
        std::cerr << "Pipe creation failed\n";
        return "";
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        std::cerr << "Fork failed\n";
        return "";
    }
    
    if (pid == 0) {
        // CHILD PROCESS
        
        // Redirect stdin
        dup2(pipe_in[0], STDIN_FILENO);
        close(pipe_in[1]);
        
        // Redirect stdout
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_out[0]);
        
        // Build environment
        std::map<std::string, std::string> env_map = _buildEnvironment(req, script_path);
        char** env_vars = _mapToCharArray(env_map);
        
        // Execute CGI
        char* args[] = {
            (char*)"/usr/bin/php-cgi",
            (char*)script_path.c_str(),
            NULL
        };
        
        execve(args[0], args, env_vars);
        
        // If we get here, execve failed
        std::cerr << "Execve failed\n";
        exit(1);
    }
    else {
        // PARENT PROCESS
        
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        // Write request body to CGI stdin
        const std::string& body = req.getBody();
        if (!body.empty()) {
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
```

**Test PHP script:** `tests/test.php`
```php
<?php
echo "Hello from PHP!\n";
echo "Method: " . $_SERVER['REQUEST_METHOD'] . "\n";
echo "Script: " . $_SERVER['SCRIPT_FILENAME'] . "\n";

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $input = file_get_contents('php://input');
    echo "Body: " . $input . "\n";
}
?>
```

**Test file:** `tests/test_cgi.cpp`
```cpp
#include "../src/CgiHandler.hpp"
#include "../src/HttpRequest.hpp"
#include <iostream>

int main() {
    CgiHandler cgi;
    
    // Test 1: GET request
    HttpRequest req1;
    req1.parse("GET /test.php HTTP/1.1\r\nHost: localhost\r\n\r\n");
    
    std::string output1 = cgi.execute("tests/test.php", req1);
    
    if (!output1.empty())
        std::cout << "✓ CGI executed\n";
    else
        std::cout << "✗ CGI execution failed\n";
    
    std::cout << "Output:\n" << output1 << "\n\n";
    
    if (output1.find("Hello from PHP!") != std::string::npos)
        std::cout << "✓ CGI output correct\n";
    else
        std::cout << "✗ CGI output wrong\n";
    
    if (output1.find("Method: GET") != std::string::npos)
        std::cout << "✓ REQUEST_METHOD set correctly\n";
    else
        std::cout << "✗ REQUEST_METHOD wrong\n";
    
    // Test 2: POST request
    HttpRequest req2;
    std::string post_data = "POST /test.php HTTP/1.1\r\n"
                           "Content-Length: 9\r\n"
                           "\r\n"
                           "test=data";
    req2.parse(post_data);
    
    std::string output2 = cgi.execute("tests/test.php", req2);
    
    if (output2.find("Method: POST") != std::string::npos)
        std::cout << "✓ POST method detected\n";
    else
        std::cout << "✗ POST method not detected\n";
    
    if (output2.find("Body: test=data") != std::string::npos)
        std::cout << "✓ POST body passed to CGI\n";
    else
        std::cout << "✗ POST body not passed\n";
    
    return 0;
}
```

**Success criteria:**
- [ ] All tests pass
- [ ] Can execute PHP scripts
- [ ] Environment variables set correctly
- [ ] POST body passed to CGI
- [ ] CGI output captured

---

### WEEK 5: Upload Handler

#### Raja Task 4: File Upload (Day 29-30)

**File:** `src/UploadHandler.hpp`
```cpp
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
    
    std::string saveFile(const std::string& upload_dir, const HttpRequest& req) const;
    bool deleteFile(const std::string& file_path) const;
};

#endif
```

**File:** `src/UploadHandler.cpp`
```cpp
#include "UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <unistd.h>

UploadHandler::UploadHandler() {}
UploadHandler::~UploadHandler() {}

std::string UploadHandler::_generateFilename() const {
    std::ostringstream oss;
    oss << "upload_" << time(NULL) << ".dat";
    return oss.str();
}

std::string UploadHandler::saveFile(const std::string& upload_dir, const HttpRequest& req) const {
    std::string filename = _generateFilename();
    std::string full_path = upload_dir + "/" + filename;
    
    std::ofstream file(full_path.c_str(), std::ios::binary);
    if (!file.is_open())
        return "";
    
    file << req.getBody();
    file.close();
    
    return filename;
}

bool UploadHandler::deleteFile(const std::string& file_path) const {
    return (unlink(file_path.c_str()) == 0);
}
```

**Test file:** `tests/test_upload.cpp`
```cpp
#include "../src/UploadHandler.hpp"
#include "../src/HttpRequest.hpp"
#include <iostream>
#include <fstream>

int main() {
    UploadHandler uh;
    
    // Test 1: Save file
    HttpRequest req;
    std::string post_req = 
        "POST /upload HTTP/1.1\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "0123456789";
    
    req.parse(post_req);
    
    std::string filename = uh.saveFile("/tmp", req);
    
    if (!filename.empty())
        std::cout << "✓ File saved: " << filename << "\n";
    else
        std::cout << "✗ File save failed\n";
    
    // Verify file contents
    std::string full_path = "/tmp/" + filename;
    std::ifstream check(full_path.c_str());
    std::string contents;
    std::getline(check, contents);
    check.close();
    
    if (contents == "0123456789")
        std::cout << "✓ File contents correct\n";
    else
        std::cout << "✗ File contents wrong: " << contents << "\n";
    
    // Test 2: Delete file
    if (uh.deleteFile(full_path))
        std::cout << "✓ File deleted\n";
    else
        std::cout << "✗ File deletion failed\n";
    
    return 0;
}
```

**Success criteria:**
- [ ] All tests pass
- [ ] Can save uploaded files
- [ ] Generates unique filenames
- [ ] Can delete files
- [ ] File contents preserved

---

## PHASE 2: INTEGRATION (BOTH - Week 6)

> 🔥 This is where everything comes together

### Day 36-38: Main Server Implementation

**File:** `src/Server.hpp`
```cpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Router.hpp"
#include "FileHandler.hpp"
#include "CgiHandler.hpp"
#include "UploadHandler.hpp"

class Server {
private:
    Config _config;
    std::vector<int> _server_fds;
    std::vector<struct pollfd> _poll_fds;
    std::map<int, int> _client_to_port;  // Map client fd to server port
    
    Router* _router;
    FileHandler* _file_handler;
    CgiHandler* _cgi_handler;
    UploadHandler* _upload_handler;
    
    void _setupServers();
    void _handleNewConnection(int server_fd, int port);
    void _handleClientData(int client_fd);
    HttpResponse _processRequest(const HttpRequest& req, int server_port);
    
public:
    Server(const std::string& config_file);
    ~Server();
    
    void run();
};

#endif
```

**File:** `src/Server.cpp`
```cpp
#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Server::Server(const std::string& config_file) {
    if (!_config.parse(config_file)) {
        std::cerr << "Failed to parse config file\n";
        exit(1);
    }
    
    _router = new Router(_config);
    _file_handler = new FileHandler();
    _cgi_handler = new CgiHandler();
    _upload_handler = new UploadHandler();
    
    _setupServers();
}

Server::~Server() {
    delete _router;
    delete _file_handler;
    delete _cgi_handler;
    delete _upload_handler;
    
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        close(_poll_fds[i].fd);
    }
}

void Server::_setupServers() {
    const std::vector<ServerConfig>& servers = _config.getServers();
    
    for (size_t i = 0; i < servers.size(); i++) {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "Socket creation failed\n";
            continue;
        }
        
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        fcntl(server_fd, F_SETFL, O_NONBLOCK);
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(servers[i].port);
        
        if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Bind failed for port " << servers[i].port << "\n";
            close(server_fd);
            continue;
        }
        
        if (listen(server_fd, 128) < 0) {
            std::cerr << "Listen failed\n";
            close(server_fd);
            continue;
        }
        
        struct pollfd pfd;
        pfd.fd = server_fd;
        pfd.events = POLLIN;
        _poll_fds.push_back(pfd);
        
        _server_fds.push_back(server_fd);
        
        std::cout << "Server listening on port " << servers[i].port << "\n";
    }
}

void Server::run() {
    std::cout << "Server running...\n";
    
    while (true) {
        int activity = poll(_poll_fds.data(), _poll_fds.size(), -1);
        
        if (activity < 0) {
            std::cerr << "Poll error\n";
            break;
        }
        
        for (size_t i = 0; i < _poll_fds.size(); i++) {
            if (_poll_fds[i].revents == 0)
                continue;
            
            // Check if it's a server socket
            bool is_server = false;
            for (size_t j = 0; j < _server_fds.size(); j++) {
                if (_poll_fds[i].fd == _server_fds[j]) {
                    const std::vector<ServerConfig>& servers = _config.getServers();
                    _handleNewConnection(_poll_fds[i].fd, servers[j].port);
                    is_server = true;
                    break;
                }
            }
            
            if (!is_server) {
                _handleClientData(_poll_fds[i].fd);
            }
        }
    }
}

void Server::_handleNewConnection(int server_fd, int port) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
        return;
    
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _poll_fds.push_back(pfd);
    
    _client_to_port[client_fd] = port;
    
    std::cout << "New client connected: fd=" << client_fd << " port=" << port << "\n";
}

void Server::_handleClientData(int client_fd) {
    char buffer[8192];
    int bytes = read(client_fd, buffer, sizeof(buffer));
    
    if (bytes <= 0) {
        std::cout << "Client disconnected: fd=" << client_fd << "\n";
        close(client_fd);
        _client_to_port.erase(client_fd);
        
        for (size_t i = 0; i < _poll_fds.size(); i++) {
            if (_poll_fds[i].fd == client_fd) {
                _poll_fds.erase(_poll_fds.begin() + i);
                break;
            }
        }
        return;
    }
    
    // Parse request
    HttpRequest request;
    std::string raw_request(buffer, bytes);
    
    if (!request.parse(raw_request)) {
        HttpResponse error = HttpResponse::badRequest();
        std::string response_str = error.build();
        write(client_fd, response_str.c_str(), response_str.length());
        close(client_fd);
        return;
    }
    
    // Get server port
    int server_port = _client_to_port[client_fd];
    
    // Process request
    HttpResponse response = _processRequest(request, server_port);
    std::string response_str = response.build();
    
    write(client_fd, response_str.c_str(), response_str.length());
    
    // HTTP/1.0 behavior: close connection
    close(client_fd);
    _client_to_port.erase(client_fd);
    
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].fd == client_fd) {
            _poll_fds.erase(_poll_fds.begin() + i);
            break;
        }
    }
}

HttpResponse Server::_processRequest(const HttpRequest& req, int server_port) {
    std::cout << "Request: " << req.getMethod() << " " << req.getPath() << "\n";
    
    // Route the request
    Route route = _router->route(req, server_port);
    
    switch (route.type) {
        case ROUTE_STATIC_FILE: {
            if (!_file_handler->fileExists(route.file_path))
                return HttpResponse::notFound();
            
            std::string content = _file_handler->getFileContents(route.file_path);
            std::string mime = _file_handler->getMimeType(route.file_path);
            return HttpResponse::ok(content, mime);
        }
        
        case ROUTE_CGI: {
            std::string output = _cgi_handler->execute(route.cgi_path, req);
            
            // TODO: Parse CGI headers properly
            // For now, just return as HTML
            return HttpResponse::ok(output, "text/html");
        }
        
        case ROUTE_UPLOAD: {
            std::string filename = _upload_handler->saveFile(route.upload_dir, req);
            if (filename.empty())
                return HttpResponse::internalError();
            
            std::string body = "File uploaded: " + filename;
            return HttpResponse::created(body);
        }
        
        case ROUTE_DELETE: {
            if (_upload_handler->deleteFile(route.file_path))
                return HttpResponse::noContent();
            else
                return HttpResponse::notFound();
        }
        
        case ROUTE_ERROR:
        default:
            if (route.error_code == 404)
                return HttpResponse::notFound();
            else if (route.error_code == 405)
                return HttpResponse::methodNotAllowed();
            else
                return HttpResponse::internalError();
    }
}
```

**File:** `src/main.cpp`
```cpp
#include "Server.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./webserv [config file]\n";
        return 1;
    }
    
    try {
        Server server(argv[1]);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
```

**Makefile:**
```makefile
NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = src/main.cpp \
       src/HttpRequest.cpp \
       src/HttpResponse.cpp \
       src/Config.cpp \
       src/Router.cpp \
       src/FileHandler.cpp \
       src/CgiHandler.cpp \
       src/UploadHandler.cpp \
       src/Server.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

---

### Day 39-40: Testing

**Test setup:** `tests/www/`

Create test files:
```bash
mkdir -p tests/www/html
mkdir -p tests/www/uploads
mkdir -p tests/www/cgi

# HTML file
echo "<h1>Webserv Test Page</h1>" > tests/www/html/index.html

# CSS file
echo "body { background: blue; }" > tests/www/html/style.css

# PHP CGI script
cat > tests/www/cgi/test.php << 'EOF'
<?php
echo "<h1>CGI Works!</h1>";
echo "<p>Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";
?>
EOF
chmod +x tests/www/cgi/test.php
```

**Test config:** `tests/webserv.conf`
```nginx
server {
    listen 8080;
    server_name localhost;
    client_max_body_size 1048576;
    
    location / {
        root tests/www/html;
        index index.html;
        allowed_methods GET POST;
    }
    
    location /cgi {
        root tests/www/cgi;
        allowed_methods GET POST;
    }
    
    location /upload {
        root tests/www/uploads;
        allowed_methods POST DELETE;
        upload_store tests/www/uploads;
    }
}
```

**Python test script:** `tests/integration_test.py`
```python
#!/usr/bin/env python3

import requests
import time
import sys

BASE_URL = "http://localhost:8080"

def test_static_file():
    """Test serving static HTML file"""
    print("Test 1: Static file... ", end="")
    resp = requests.get(f"{BASE_URL}/index.html")
    
    assert resp.status_code == 200, f"Expected 200, got {resp.status_code}"
    assert "Webserv Test Page" in resp.text, "HTML content missing"
    print("✓")

def test_css_file():
    """Test serving CSS file with correct MIME type"""
    print("Test 2: CSS file... ", end="")
    resp = requests.get(f"{BASE_URL}/style.css")
    
    assert resp.status_code == 200
    assert "text/css" in resp.headers.get("Content-Type", "")
    print("✓")

def test_404():
    """Test 404 for non-existent file"""
    print("Test 3: 404 Not Found... ", end="")
    resp = requests.get(f"{BASE_URL}/nonexistent.html")
    
    assert resp.status_code == 404
    print("✓")

def test_cgi():
    """Test CGI execution"""
    print("Test 4: CGI execution... ", end="")
    resp = requests.get(f"{BASE_URL}/cgi/test.php")
    
    assert resp.status_code == 200
    assert "CGI Works" in resp.text
    assert "Method: GET" in resp.text
    print("✓")

def test_upload():
    """Test file upload"""
    print("Test 5: File upload... ", end="")
    resp = requests.post(f"{BASE_URL}/upload", data="test file content")
    
    assert resp.status_code == 201
    assert "upload_" in resp.text
    print("✓")

def test_concurrent():
    """Test concurrent requests"""
    print("Test 6: Concurrent requests (100)... ", end="")
    
    import concurrent.futures
    
    def make_request(i):
        return requests.get(f"{BASE_URL}/index.html")
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=100) as executor:
        results = list(executor.map(make_request, range(100)))
    
    assert all(r.status_code == 200 for r in results)
    print("✓")

def test_method_not_allowed():
    """Test 405 Method Not Allowed"""
    print("Test 7: Method not allowed... ", end="")
    resp = requests.delete(f"{BASE_URL}/index.html")
    
    assert resp.status_code == 405
    print("✓")

if __name__ == "__main__":
    print("\n=== WEBSERV INTEGRATION TESTS ===\n")
    
    try:
        test_static_file()
        test_css_file()
        test_404()
        test_cgi()
        test_upload()
        test_concurrent()
        test_method_not_allowed()
        
        print("\n=== ALL TESTS PASSED ===\n")
        sys.exit(0)
        
    except AssertionError as e:
        print(f"✗ FAILED")
        print(f"Error: {e}")
        sys.exit(1)
    except requests.exceptions.ConnectionError:
        print("✗ FAILED")
        print("Error: Cannot connect to server. Is it running?")
        sys.exit(1)
```

**Run tests:**
```bash
# Compile
make

# Run server
./webserv tests/webserv.conf

# In another terminal, run tests
python3 tests/integration_test.py
```

---

## TESTING CHECKLIST

**Before calling the project complete:**

### Functionality Tests
- [ ] Serves static HTML files
- [ ] Serves CSS with correct MIME type
- [ ] Serves images (PNG, JPG)
- [ ] Returns 404 for non-existent files
- [ ] Executes CGI scripts (at least PHP)
- [ ] Handles file uploads via POST
- [ ] Handles DELETE requests
- [ ] Returns 405 for disallowed methods
- [ ] Handles multiple servers on different ports
- [ ] Routes requests to correct location

### Robustness Tests
- [ ] Handles 100+ concurrent connections
- [ ] Never crashes with malformed requests
- [ ] Handles very large files
- [ ] Handles very long URLs
- [ ] Handles requests without Host header
- [ ] Handles chunked encoding (bonus)
- [ ] Doesn't hang on slow clients
- [ ] Properly closes connections

### Code Quality
- [ ] No memory leaks (run with `valgrind ./webserv config.conf`)
- [ ] Compiles with `-Wall -Wextra -Werror`
- [ ] Uses C++98 standard
- [ ] No forbidden functions used
- [ ] Code is properly organized into classes
- [ ] Makefile has all required rules
- [ ] No unnecessary relinking

### Documentation
- [ ] README explains how to compile
- [ ] README explains how to run
- [ ] README explains config file format
- [ ] Example config files provided
- [ ] Test files provided

---

## SUBMISSION REQUIREMENTS

### Files to Submit
```
webserv/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── Server.{hpp,cpp}
│   ├── HttpRequest.{hpp,cpp}
│   ├── HttpResponse.{hpp,cpp}
│   ├── Config.{hpp,cpp}
│   ├── Router.{hpp,cpp}
│   ├── FileHandler.{hpp,cpp}
│   ├── CgiHandler.{hpp,cpp}
│   └── UploadHandler.{hpp,cpp}
├── configs/
│   ├── default.conf
│   └── test.conf
├── www/
│   ├── html/
│   │   ├── index.html
│   │   ├── 404.html
│   │   └── 500.html
│   └── cgi/
│       └── test.php
└── README.md
```

### Git Workflow
```bash
# Both team members
git clone <your-repo>
cd webserv

# Create branches
git checkout -b brian/request-parser
git checkout -b raja/response-handler

# Daily workflow
git add .
git commit -m "descriptive message"
git push origin <your-branch>

# Integration
git checkout main
git merge brian/request-parser
git merge raja/response-handler
```

---

## DAILY CHECK-IN FORMAT

**Copy this template and fill it out every day:**
```
Date: YYYY-MM-DD

Brian:
- [ ] Task completed today: _______________
- [ ] Blockers: _______________
- [ ] Tomorrow's task: _______________

Raja:
- [ ] Task completed today: _______________
- [ ] Blockers: _______________
- [ ] Tomorrow's task: _______________

Integration issues:
- _______________

Questions for each other:
- _______________
```

---

## FINAL NOTES

### When You Get Stuck

1. Read the error message carefully
2. Check the relevant SKILL.md file (if applicable)
3. Compare with NGINX behavior
4. Ask your partner
5. Test with telnet/curl to isolate the issue
6. Check RFCs if needed

### Common Pitfalls

- Forgetting to set `O_NONBLOCK`
- Not handling partial reads/writes
- Memory leaks in CGI environment variables
- Not closing file descriptors
- Forgetting `\r\n\r\n` in HTTP responses
- Not parsing Content-Length correctly
- CGI not finding PHP executable

### Success Indicators

You're on track if:
- Tests pass every day
- Both understand each other's code
- No merge conflicts (communicate!)
- Server handles browser requests
- No crashes after 1 hour of stress testing

---

**Good luck! You got this. Start with Day 1, Task 1. Don't skip ahead.**