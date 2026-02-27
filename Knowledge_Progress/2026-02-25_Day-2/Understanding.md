# WEBSERV - Understanding Web Servers & Real-World Deployment

**Date:** Day 2 Learning Notes  
**Topic:** How webserv relates to real websites

---

## Question 1: Are we replicating what Google's servers do?

**Answer: YES! Exactly the same process.**

### What happens when you visit google.com:

1. Your browser connects to Google's server (in a data center somewhere)
2. Your browser sends HTTP request:
```
   GET / HTTP/1.1
   Host: google.com
```
3. Google's server reads that request
4. Google's server sends back HTTP response:
```
   HTTP/1.1 200 OK
   Content-Type: text/html
   
   <html>...</html>
```
5. Your browser displays the HTML

### What YOUR webserv will do:

1. Browser connects to YOUR server (localhost:8080)
2. Browser sends HTTP request:
```
   GET /index.html HTTP/1.1
   Host: localhost
```
3. YOUR server reads that request
4. YOUR server sends back HTTP response:
```
   HTTP/1.1 200 OK
   Content-Type: text/html
   
   <html>...</html>
```
5. Browser displays it

**Same architecture, same protocol, just running locally!**

---

## Question 2: How do I make it a REAL website on the internet?

**Answer: YES! You need to host it on a server connected to the internet.**

### Current Setup (Local Development):
```
Your Computer:
├── Your webserv (server running)
└── Your browser (client)
    ↓
    Connects to: localhost:8080
    
Only YOU can access it on your machine
```

### Real Website Setup (Production):
```
Remote Server (Digital Ocean, AWS, etc.):
└── Your webserv running 24/7
    ↓
    Internet
    ↓
Anyone's browser worldwide
    ↓
    Connects to: your-domain.com:80
```

---

## Steps to Deploy a Real Website:

### Step 1: Build Your Webserv (What you're doing now)
- Works on localhost
- Handles HTTP requests correctly
- Serves static files
- Runs CGI scripts
- Never crashes

### Step 2: Get a Server (VPS - Virtual Private Server)
- Rent a server online (examples):
  - Digital Ocean: $5-10/month
  - AWS EC2: $5-20/month
  - Linode: $5/month
  - Vultr: $5/month
- Comes with:
  - Public IP address (e.g., 165.227.123.45)
  - Linux operating system
  - 24/7 uptime

### Step 3: Get a Domain Name (Optional but recommended)
- Buy a domain name:
  - Namecheap: ~$10/year
  - GoDaddy: ~$12/year
  - Google Domains: ~$12/year
- Examples: `mywebsite.com`, `myproject.net`
- Point domain to your server's IP address (DNS configuration)

### Step 4: Deploy Your Code
```bash
# On your server:
scp webserv user@your-server-ip:/home/user/
ssh user@your-server-ip
./webserv config.conf
```

### Step 5: Access Your Website
- Without domain: `http://165.227.123.45:8080`
- With domain: `http://mywebsite.com`
- Anyone in the world can now visit!

---

## What Each Part Does:

### Your Webserv (The Engine)
**What it does:**
- Accepts TCP connections
- Reads HTTP requests
- Parses requests
- Routes to correct file/CGI
- Builds HTTP responses
- Sends responses back

**What it does NOT do:**
- Create website content (you provide HTML/CSS files)
- Handle domain names (DNS does this)
- Provide the server hardware (VPS does this)

### The Website Content
- HTML files (your pages)
- CSS files (styling)
- Images (PNG, JPG)
- CGI scripts (PHP, Python for dynamic content)

**You create these separately and put them in directories your webserv serves from**

### The Infrastructure
- **VPS/Server:** Physical/virtual computer running 24/7
- **Domain Name:** Human-readable address (optional, can use IP)
- **DNS:** Translates domain → IP address
- **Network:** Internet connection

---

## Architecture Comparison:

### Big Companies (Google, Facebook):
```
User's Browser
    ↓
Internet
    ↓
DNS (google.com → 142.250.xxx.xxx)
    ↓
Google's Server (running custom server software)
    ↓
Sends response back
```

### Your Webserv (Production):
```
User's Browser
    ↓
Internet
    ↓
DNS (mysite.com → 165.227.xxx.xxx)
    ↓
Your VPS Server (running YOUR webserv)
    ↓
Sends response back
```

### Your Webserv (Development - Now):
```
Your Browser
    ↓
localhost (127.0.0.1)
    ↓
Your Computer (running YOUR webserv)
    ↓
Sends response back
```

**Same code, different deployment!**

---

## Key Insight:

**Your webserv code is IDENTICAL whether running:**
- On your laptop (localhost)
- On a $5/month server
- On a million-dollar data center

**The server doesn't care about:**
- Where it's running
- Who connects to it
- What files it's serving

**It just:**
1. Accepts connections
2. Reads HTTP requests
3. Sends HTTP responses

**That's the beauty of HTTP - it's a standard protocol!**

---

## What You're Building vs. What Already Exists:

| Component | You're Building? | Already Exists |
|-----------|------------------|----------------|
| Web Server (NGINX/Apache equivalent) | ✅ YES | NGINX, Apache |
| Website content (HTML/CSS) | ❌ NO (just files) | Any HTML/CSS you write |
| Browser (Chrome/Firefox) | ❌ NO | Chrome, Firefox, Safari |
| Domain/Hosting | ❌ NO | Digital Ocean, AWS |
| Operating System | ❌ NO | Linux, macOS |

**You're ONLY building the server software (the engine)!**

---

## Real-World Example:

### After Finishing This Project, You Could:

1. **Finish webserv project** ✓
2. **Create simple website:**
```
   /var/www/
   ├── index.html
   ├── style.css
   └── about.html
```
3. **Rent a $5 server** (Digital Ocean)
4. **Upload your webserv:**
```bash
   scp webserv config.conf root@your-ip:/root/
```
5. **Create config file:**
```nginx
   server {
       listen 8080;
       root /var/www;
       index index.html;
   }
```
6. **Run it:**
```bash
   ./webserv config.conf
```
7. **Visit:** `http://your-server-ip:8080`
8. **BOOM! Your website is live to the world!**

---

## Summary:

**Local Development (Now):**
- Server: Your laptop
- Access: localhost:8080
- Users: Only you

**Production Deployment (After project):**
- Server: Rented VPS
- Access: your-domain.com or public IP
- Users: Anyone in the world

**The CODE is the same. Only the deployment location changes!**

---

## Important Notes:

1. **Your 42 project focuses ONLY on building the server software**
   - Not hosting
   - Not domain management
   - Not creating website content
   - Just the HTTP server engine

2. **The server you're building is like NGINX/Apache**
   - Professional-grade servers use the same concepts
   - Just more features, optimizations, security

3. **Localhost vs. Production is just a configuration change**
   - Same binary
   - Same code
   - Different IP/port to listen on

---

**End of Notes**

---

## Next Steps in Learning:

- ✅ Day 1: Understood HTTP protocol
- ✅ Day 2: Built single-client echo server
- ⏳ Day 3: Build multi-client server with poll()
- ⏳ Day 4-5: Make it speak HTTP
- ⏳ Week 2+: Build actual webserv with all features

**You're on the right track! Keep going!** 🚀