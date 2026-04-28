### Day 6-7: Speak HTTP

**Objective:** Make the server respond with HTTP instead of echoing

#### Task 4.1: HTTP Hello World (1 hour)

**File:** `learning/http_hello.cpp`

Modify `echo_multi.cpp` - replace the echo section with:
```cpp
// In the "data from client" section:
if (bytes_read <= 0) {
    // ... same disconnect logic ...
} else {
    // Build HTTP response
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 52\r\n"
        "\r\n"
        "<html><body><h1>Hello from our server!</h1></body></html>";
    
    write(poll_fds[i].fd, response.c_str(), response.length());
    
    // HTTP/1.0 behavior: close after response
    close(poll_fds[i].fd);
    poll_fds.erase(poll_fds.begin() + i);
    i--;
}
```

**Test:**
1. Run server
2. Open browser: `http://localhost:8080`
3. Should see "Hello from our server!"

**Understanding check:**
- [ ] Why does the response start with `HTTP/1.1 200 OK`?
- [ ] What does `Content-Length: 52` mean?
- [ ] Why is there a blank line (`\r\n\r\n`) before the HTML?
- [ ] Why do we close the connection after sending response?

**Deliverable:** Browser displays your HTML. Both understand HTTP response format.

1. ✅ Modified echo_multi.cpp → http_hello.cpp
2. ✅ Replaced echo → HTTP response  
3. ✅ Browser shows "Hello from our server!" page
4. ✅ Understand HTTP format completely
5. ✅ Multi-client poll() foundation intact


---
