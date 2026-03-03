### Day 4-5: Multi-Client with poll()

**Objective:** Handle multiple clients without blocking

#### Task 3.1: Non-Blocking Echo Server (3 hours)

**File:** `learning/echo_multi.cpp`
```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <cstring>

void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Enable address reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Make non-blocking
    setNonBlocking(server_fd);
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);
    
    std::cout << "Multi-client server on port 8080\n";
    
    // Array of file descriptors to monitor
    std::vector<struct pollfd> poll_fds;
    
    // Add server socket to poll array
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    poll_fds.push_back(server_pollfd);
    
    while (true) {
        // Wait for events on ANY socket
        int activity = poll(poll_fds.data(), poll_fds.size(), -1);
        
        if (activity < 0) {
            std::cerr << "Poll error\n";
            break;
        }
        
        // Check each socket
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents == 0)
                continue;
            
            // New connection?
            if (poll_fds[i].fd == server_fd) {
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) {
                    std::cerr << "Accept failed\n";
                    continue;
                }
                
                setNonBlocking(client_fd);
                
                std::cout << "New client: fd=" << client_fd << "\n";
                
                struct pollfd client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;
                poll_fds.push_back(client_pollfd);
            }
            // Data from client?
            else {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                
                int bytes_read = read(poll_fds[i].fd, buffer, sizeof(buffer) - 1);
                
                if (bytes_read <= 0) {
                    std::cout << "Client disconnected: fd=" << poll_fds[i].fd << "\n";
                    close(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    i--;
                } else {
                    std::cout << "fd=" << poll_fds[i].fd << " sent: " << buffer;
                    write(poll_fds[i].fd, buffer, bytes_read);
                }
            }
        }
    }
    
    for (size_t i = 0; i < poll_fds.size(); i++) {
        close(poll_fds[i].fd);
    }
    
    return 0;
}
```

**Instructions:**
1. Type this code together
2. Compile and run
3. Test with 3 terminals:
```bash
   # Terminal 1: Server
   ./echo_multi
   
   # Terminal 2: Client 1
   telnet localhost 8080
   
   # Terminal 3: Client 2
   telnet localhost 8080
   
   # Terminal 4: Client 3
   telnet localhost 8080
```
4. Type in each client - all should work independently
5. Close one client - others should still work

**Understanding check (BOTH must answer):**
- [ ] What does `poll()` do?
- [ ] What does `POLLIN` mean?
- [ ] Why do we need `setNonBlocking()`?
- [ ] What happens if we don't use `poll()` and just call `read()`?
- [ ] Why do we need `poll_fds.erase()` when client disconnects?

**Deliverable:** Both understand every line. Can handle 3+ simultaneous clients.

---