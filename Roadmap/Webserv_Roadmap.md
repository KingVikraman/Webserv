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

