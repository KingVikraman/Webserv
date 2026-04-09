### Day 1: Understanding HTTP

**Objective:** See what HTTP actually looks like

#### Task 1.1: Manual HTTP Requests (30 min)

**Steps:**
1. Open terminal
2. Run: `telnet google.com 80`
3. When connected, type:
```
   GET / HTTP/1.1
   Host: google.com
```
   (Press Enter twice after Host line)

4. Observe the response
5. Take screenshot

**Repeat with:**
- `telnet example.com 80`
- `telnet github.com 80`

**Questions to answer:**
- [✅] What does `HTTP/1.1 200 OK` mean?
- [✅] What are headers?
- [✅] Where does the body start?
- [✅] What happens if you forget `Host:` header?

**Deliverable:** Both can draw HTTP request/response format on paper from memory.

---

#### Task 1.2: Try Different Methods (30 min)

**Experiments:**
```bash
# Test 1: Request non-existent page
telnet google.com 80
GET /nonexistent HTTP/1.1
Host: google.com


# What status code do you get?

# Test 2: Forget Host header
telnet google.com 80
GET / HTTP/1.1


# What error do you get?
```

**Questions:**
- [✅] What does `404 Not Found` mean?
- [✅] Why is `Host:` header required?
- [✅] What's the difference between GET and POST?

---