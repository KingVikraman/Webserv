#!/usr/bin/env bash
set -u

BASE_URL="${1:-http://127.0.0.1:8000}"
UPLOAD_FILE="${2:-/tmp/webserv_defense_upload.txt}"
LARGE_FILE="/tmp/webserv_defense_large.bin"

PASS=0
FAIL=0

cleanup() {
  rm -f "$UPLOAD_FILE" "$LARGE_FILE"
}
trap cleanup EXIT

print_header() {
  echo ""
  echo "============================================================"
  echo "$1"
  echo "============================================================"
}

status_of() {
  local method="$1"
  local url="$2"
  local data="${3:-}"

  if [ -n "$data" ]; then
    curl -sS -o /tmp/webserv_body.out -w "%{http_code}" -X "$method" "$url" --data-binary "$data"
  else
    curl -sS -o /tmp/webserv_body.out -w "%{http_code}" -X "$method" "$url"
  fi
}

status_upload() {
  local url="$1"
  local file="$2"
  curl -sS -o /tmp/webserv_body.out -w "%{http_code}" -X POST "$url" -F "file=@$file"
}

status_delete_json() {
  local url="$1"
  local json="$2"
  curl -sS -o /tmp/webserv_body.out -w "%{http_code}" -X DELETE "$url" -H "Content-Type: application/json" --data "$json"
}

check_one_of() {
  local name="$1"
  local got="$2"
  local allowed_csv="$3"

  IFS=',' read -r -a allowed <<< "$allowed_csv"
  local ok=1
  for code in "${allowed[@]}"; do
    if [ "$got" = "$code" ]; then
      ok=0
      break
    fi
  done

  if [ $ok -eq 0 ]; then
    echo "[PASS] $name -> $got"
    PASS=$((PASS + 1))
  else
    echo "[FAIL] $name -> got $got, expected one of [$allowed_csv]"
    FAIL=$((FAIL + 1))
  fi
}

check_redirect() {
  local name="$1"
  local url="$2"
  local expected_codes="$3"
  local expect_location_part="$4"

  local headers
  headers="$(curl -sS -D - -o /dev/null "$url")"
  local code
  code="$(printf "%s" "$headers" | awk 'NR==1 {print $2}')"
  local loc
  loc="$(printf "%s" "$headers" | awk -F': ' 'tolower($1)=="location" {print $2}' | tr -d '\r')"

  check_one_of "$name (status)" "$code" "$expected_codes"

  if printf "%s" "$loc" | grep -q "$expect_location_part"; then
    echo "[PASS] $name (location) -> $loc"
    PASS=$((PASS + 1))
  else
    echo "[FAIL] $name (location) -> missing expected fragment '$expect_location_part', got '$loc'"
    FAIL=$((FAIL + 1))
  fi
}

extract_uploaded_name() {
  if [ -f /tmp/webserv_body.out ]; then
    sed -n 's/.*File uploaded: //p' /tmp/webserv_body.out | head -n 1 | tr -d '\r'
  fi
}

print_header "Webserv Defense Test"
echo "Base URL: $BASE_URL"
echo "Tip: start server first, e.g. ./webserv config/server.conf"

print_header "Basic GET / 404 Unknown Method"
code="$(status_of GET "$BASE_URL/")"
check_one_of "GET /" "$code" "200"

code="$(status_of GET "$BASE_URL/this_should_not_exist_42")"
check_one_of "GET unknown path" "$code" "404"

code="$(status_of FOO "$BASE_URL/")"
check_one_of "Unknown method" "$code" "400,405,501"

print_header "Method Restrictions"
code="$(status_of POST "$BASE_URL/list" "abc=1")"
check_one_of "POST /list (GET-only route)" "$code" "405"

print_header "Upload + List + Delete"
printf "webserv defense upload %s\n" "$(date)" > "$UPLOAD_FILE"
code="$(status_upload "$BASE_URL/upload" "$UPLOAD_FILE")"
check_one_of "POST upload file" "$code" "200,201,204"

uploaded_name="$(extract_uploaded_name)"
if [ -n "${uploaded_name:-}" ]; then
  echo "Uploaded filename reported by server: $uploaded_name"
else
  uploaded_name="$(basename "$UPLOAD_FILE")"
  echo "Uploaded filename not reported, falling back to local name: $uploaded_name"
fi

code="$(status_of GET "$BASE_URL/list")"
check_one_of "GET /list autoindex" "$code" "200"

if grep -q "$uploaded_name" /tmp/webserv_body.out; then
  echo "[PASS] Uploaded filename appears in /list/"
  PASS=$((PASS + 1))
else
  echo "[WARN] Uploaded filename not found in /list/ output (may still be valid depending on implementation)."
fi

code="$(status_of DELETE "$BASE_URL/uploads/$uploaded_name")"
check_one_of "DELETE uploaded file" "$code" "200,204"

print_header "Body Size Limit (expect 413)"
dd if=/dev/zero of="$LARGE_FILE" bs=1m count=11 >/dev/null 2>&1
code="$(curl -sS -o /tmp/webserv_body.out -w "%{http_code}" -X POST "$BASE_URL/upload" -F "file=@$LARGE_FILE")"
check_one_of "POST > client_max_body_size" "$code" "413"

print_header "Redirect Route"
redirect_code="$(curl -sS -o /dev/null -w "%{http_code}" "$BASE_URL/redirect")"
if [ "$redirect_code" = "404" ]; then
  echo "[WARN] /redirect is not configured with a return directive (got 404). Skipping redirect assertion."
else
  check_redirect "GET /redirect" "$BASE_URL/redirect" "301,302,303,307,308" "/"
fi

print_header "Summary"
echo "PASS: $PASS"
echo "FAIL: $FAIL"

if [ "$FAIL" -eq 0 ]; then
  echo "Overall: SUCCESS"
  exit 0
else
  echo "Overall: FAILURES FOUND"
  exit 1
fi
