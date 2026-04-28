Typed commands -> telnet google.com


![alt text](Screencastfrom02-20-2026125155PM-ezgif.com-video-to-gif-converter.gif)


Responce recived -> 301 Redirect

HTTP/1.1 301 Moved Permanently

- `200 OK` = "Here's your content"
- `301 Moved Permanently` = "The page moved to a different address"

Google is telling you: "Hey, you asked for `google.com`, but we want you to use `www.google.com` instead".

## The Headers (Everything Before the Blank Line)

Let me explain each one you saw:
```
Location: http://www.google.com/
```
👉 **"Go to this address instead"** - This is where the page has moved to
```
Content-Type: text/html; charset=UTF-8
```
👉 **"I'm sending you HTML text, encoded in UTF-8"**
```
Date: Fri, 20 Feb 2026 04:52:27 GMT
```
👉 **When the server sent this response**
```
Expires: Sun, 22 Mar 2026 04:52:27 GMT
Cache-Control: public, max-age=2592000
```
👉 **Browser can cache this response for 30 days** (2592000 seconds = 30 days)
```
Server: gws
```
👉 **Server software** (gws = Google Web Server)
```
Content-Length: 219
```
👉 **The body is 219 bytes long**
```
X-XSS-Protection: 0
X-Frame-Options: SAMEORIGIN
```
👉 **Security headers** (you'll learn about these later)

---

## The Body (After the Blank Line)
```html

301 Moved
301 Moved
The document has moved
here.

```

This is HTML! It's saying:
- **Title:** "301 Moved"
- **Body:** "The document has moved [here](http://www.google.com/)"

If you opened this in a browser, you'd see a page saying "The document has moved here" with a clickable link.

---

**So what does this mean?**


- What i understand about 301 is that the request that I have currently sent is for google.com right, but when I sent that request google responded saying that hey we have changed our location, its currently not google.com but its, www.google.com instead, so that is what 301 means that its moved permenetly.

- So, since they have told me that they have moved they are now asking me to go to a new place which is www.google.com.


- I also managed to get the actual request to www.google.com and i got a
proper 200 OK | SUCCESS  page. WHich was great, and i also experimented with error 404, which means Page not found | ERROR | "This page does not exist".


## Status Codes Summary

You've now seen 3 different status codes:

| Code | Meaning | What happened |
|------|---------|---------------|
| **200 OK** | Success | "Here's your page!" |
| **301 Moved Permanently** | Redirect | "Go to this new address instead" |
| **404 Not Found** | Error | "This page doesn't exist" |

**There are MANY more status codes:**
- `400 Bad Request` = "Your request was malformed"
- `403 Forbidden` = "You're not allowed to access this"
- `500 Internal Server Error` = "Something broke on the server"

**For your webserv project, you'll need to return the correct status code for each situation!**

---

## Why This Matters for Webserv

When you build your HTTP server, you'll need to:

1. **Read the request** (what you just typed manually)
2. **Decide what to do:**
   - File exists? → Send `200 OK` + file contents
   - File doesn't exist? → Send `404 Not Found` + error page
   - Method not allowed? → Send `405 Method Not Allowed`
3. **Build the response** (just like what Google sent you)
4. **Send it back to the client**

**You just manually did what your server will do automatically!**

---