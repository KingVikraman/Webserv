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