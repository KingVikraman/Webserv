# Socket Programming Basics for Webserv

## Core Header Files

### `<sys/socket.h>`
**Tools for building connections:**
- `socket()` - creates a socket endpoint
- `bind()` - attaches socket to an address
- `listen()` - starts listening for connections  
- `accept()` - accepts incoming connections
- `send()` / `recv()` - send/receive data

### `<netinet/in.h>`
**Tools for addressing (where to connect):**
- `struct sockaddr_in` - IPv4 address structure
- `INADDR_ANY` - "any IP address" constant
- `htons()` - converts port numbers to network format

**Analogy:** `<sys/socket.h>` = toolbox for connections, `<netinet/in.h>` = toolbox for addresses.

## Key Socket Parameters

### `AF_INET` (Address Family)
- **AF** = Address Family, **INET** = Internet
- Means: "Use **IPv4** addresses" (like `192.168.1.1`)
- Alternatives:
  - `AF_INET6` = IPv6 (like `2001:0db8:85a3::8a2e:0370:7334`)
  - `AF_UNIX` = Unix domain sockets (file-based)
- **For webserv:** Use `AF_INET` (IPv4)

### `SOCK_STREAM` (Socket Type)
Means: "Use **TCP** connection" (reliable, ordered)

| Type          | Protocol | Behavior |
|---------------|----------|----------|
| `SOCK_STREAM` | **TCP**  | Reliable, ordered, connection-based *(like a phone call)* |
| `SOCK_DGRAM`  | **UDP**  | Fast but unreliable, no connection *(like shouting across the street)* |

**HTTP uses TCP (`SOCK_STREAM`)** for reliable delivery.

## Setting Up the Server Address

```cpp
struct sockaddr_in address;
address.sin_family = AF_INET;           // (1) Use IPv4
address.sin_addr.s_addr = INADDR_ANY;   // (2) Listen on ALL IPs
address.sin_port = htons(8080);         // (3) Port 8080 (network format)
```

**Creates:** `0.0.0.0:8080` (all interfaces, port 8080)

### Breakdown

**(1) `address.sin_family = AF_INET;`**
- "This address uses IPv4"

**(2) `address.sin_addr.s_addr = INADDR_ANY;`**
- **IP to listen on:** `0.0.0.0` = **ALL interfaces**
```
Your computer:
├── 127.0.0.1 (localhost)   ✅ Can connect
├── 192.168.1.10 (WiFi)     ✅ Can connect  
└── 10.0.0.5 (Ethernet)     ✅ Can connect
```
**Analogy:** Opening **all doors** of your shop (not just one entrance).

**(3) `address.sin_port = htons(8080);`**
- **Port:** 8080
- **`htons()`** = Host TO Network Short (converts to network byte order)
- **Always use:** `htons()` for ports → network, `ntohs()` for network → ports

## Server Workflow (The Key Functions)

### 1. `socket(AF_INET, SOCK_STREAM, 0)`
**Creates:** Communication endpoint
- Like installing an **electrical outlet** on the wall
- `server_fd` = file descriptor (ID/handle for this socket, e.g., `fd=3`)

### 2. `bind(server_fd, (struct sockaddr*)&address, sizeof(address))`
**Does:** Attaches address to socket + reserves the port
- Tells network: "This endpoint is at port 8080"
- Occupies port so others can't use it
**Analogy:** Giving your endpoint a **physical location**

### 3. `listen(server_fd, 5)`
**Does:** "Ready for connections!" (queue up to 5 waiting clients)

### 4. `accept(server_fd, NULL, NULL)`
**Does:** Waits for client → returns **NEW** `client_fd` for that client
**Analogy:** Door stays open, greets each customer with a personal table

### 5. Client Communication Loop
```cpp
while (true) {
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        std::cout << "Client disconnected\n";
        break;
    }
    
    std::cout << "Received " << bytes_read << " bytes: " << buffer;
    write(client_fd, buffer, bytes_read);  // Echo back
}
```
**Does:** Reads client input → echoes it back (bit by bit)

## Complete Server Flow
```
1. socket() → Create endpoint (server_fd)
2. bind()  → Assign address (0.0.0.0:8080) 
3. listen()→ Open for business
4. accept()→ Wait for client → get client_fd
5. Loop:  read() → process → write()
```

## Real-World Analogy: Opening a Restaurant
```
sin_family = AF_INET     // "We serve food" (IPv4)
sin_addr = INADDR_ANY    // "All entrances open" (any IP)  
sin_port = htons(8080)   // "Located at 8080" (port)

bind()  = "Reserve this address"
listen()= "We're open!"
accept()= "Next customer, please!"
```
```
