### WEEK 2: HTTP Request Parser


## BRIAN'S TASKS (Request Pipeline)

**Your Responsibility:** Socket → Raw data → Parsed request → Routing decision

**Files you own:**
- `src/HttpRequest.{hpp,cpp}`
- `src/Config.{hpp,cpp}`
- `src/Router.{hpp,cpp}`

---

#### Brian Task 1: Parse Request Line (Day 8-9)

**File:** `src/HttpRequest.hpp`
```cpp
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <sstream>

class HttpRequest {
private:
    std::string _method;
    std::string _path;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    
    bool _parseRequestLine(const std::string& line);
    bool _parseHeader(const std::string& line);
    
public:
    HttpRequest();
    ~HttpRequest();
    
    bool parse(const std::string& raw_request);
    
    // Getters
    std::string getMethod() const { return _method; }
    std::string getPath() const { return _path; }
    std::string getVersion() const { return _version; }
    std::string getHeader(const std::string& key) const;
    std::string getBody() const { return _body; }
    
    // Validation
    bool isValid() const;
    bool hasHeader(const std::string& key) const;
};

#endif
```

**File:** `src/HttpRequest.cpp`
```cpp
#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}
HttpRequest::~HttpRequest() {}

bool HttpRequest::_parseRequestLine(const std::string& line) {
    std::istringstream iss(line);
    
    iss >> _method >> _path >> _version;
    
    // Validation
    if (_method.empty() || _path.empty() || _version.empty())
        return false;
    
    // Method must be GET, POST, or DELETE
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return false;
    
    // Version must be HTTP/1.0 or HTTP/1.1
    if (_version != "HTTP/1.0" && _version != "HTTP/1.1")
        return false;
    
    // Path must start with /
    if (_path[0] != '/')
        return false;
    
    return true;
}

bool HttpRequest::parse(const std::string& raw_request) {
    std::istringstream stream(raw_request);
    std::string line;
    
    // Parse request line
    std::getline(stream, line);
    if (!_parseRequestLine(line))
        return false;
    
    return true;
}

std::string HttpRequest::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return "";
}

bool HttpRequest::hasHeader(const std::string& key) const {
    return _headers.find(key) != _headers.end();
}

bool HttpRequest::isValid() const {
    return !_method.empty() && !_path.empty();
}
```

**Test file:** `tests/test_request_line.cpp`
```cpp
#include "../src/HttpRequest.hpp"
#include <iostream>

int main() {
    HttpRequest req;
    
    // Test 1: Valid request
    std::string test1 = "GET /index.html HTTP/1.1\r\n\r\n";
    if (req.parse(test1)) {
        std::cout << "✓ Test 1 passed\n";
        std::cout << "  Method: " << req.getMethod() << "\n";
        std::cout << "  Path: " << req.getPath() << "\n";
        std::cout << "  Version: " << req.getVersion() << "\n";
    } else {
        std::cout << "✗ Test 1 failed\n";
    }
    
    // Test 2: Invalid method
    HttpRequest req2;
    std::string test2 = "INVALID /index.html HTTP/1.1\r\n\r\n";
    if (!req2.parse(test2))
        std::cout << "✓ Test 2 passed (rejected invalid method)\n";
    else
        std::cout << "✗ Test 2 failed\n";
    
    // Test 3: Missing path
    HttpRequest req3;
    std::string test3 = "GET HTTP/1.1\r\n\r\n";
    if (!req3.parse(test3))
        std::cout << "✓ Test 3 passed (rejected missing path)\n";
    else
        std::cout << "✗ Test 3 failed\n";
    
    // Test 4: Path doesn't start with /
    HttpRequest req4;
    std::string test4 = "GET index.html HTTP/1.1\r\n\r\n";
    if (!req4.parse(test4))
        std::cout << "✓ Test 4 passed (rejected invalid path)\n";
    else
        std::cout << "✗ Test 4 failed\n";
    
    return 0;
}
```

**Compile and test:**
```bash
c++ -Wall -Wextra -Werror tests/test_request_line.cpp src/HttpRequest.cpp -o test_request_line
./test_request_line
```

**Success criteria:**
- [ ] All 4 tests pass
- [ ] Can parse method, path, version from any valid request line
- [ ] Rejects invalid methods
- [ ] Rejects invalid paths

---
