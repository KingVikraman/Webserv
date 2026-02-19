### Day 2-3: Single Client Echo Server

**Objective:** Learn socket programming basics

#### Task 2.1: Write Echo Server Together (2 hours)

**File:** `learning/echo_single.cpp`
```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main() {
    // STEP 1: Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }
    
    // STEP 2: Setup address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    // STEP 3: Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }
    
    // STEP 4: Listen for connections
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }
    
    std::cout << "Server listening on port 8080...\n";
    
    // STEP 5: Accept ONE client
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        std::cerr << "Accept failed\n";
        return 1;
    }
    
    std::cout << "Client connected!\n";
    
    // STEP 6: Echo loop
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read <= 0) {
            std::cout << "Client disconnected\n";
            break;
        }
        
        std::cout << "Received: " << buffer;
        write(client_fd, buffer, bytes_read);
    }
    
    close(client_fd);
    close(server_fd);
    
    return 0;
}
```

**Instructions:**
1. Type this code together (DON'T copy-paste)
2. Compile: `c++ -Wall -Wextra -Werror echo_single.cpp -o echo_single`
3. Run: `./echo_single`
4. Test in another terminal: `telnet localhost 8080`
5. Type anything - it should echo back

**Understanding check (BOTH must answer):**
- [ ] What does `socket()` do?
- [ ] What does `bind()` do?
- [ ] What does `listen()` do?
- [ ] What does `accept()` do?
- [ ] What does `read()` return when client disconnects?

**Deliverable:** Both can explain each system call. Both can modify the port number.

---