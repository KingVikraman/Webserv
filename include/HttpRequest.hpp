#pragma once

#include <map>
#include <string>
#include <vector>
#include <sys/types.h>


#define MAX_URI_LENGTH 4096

enum HttpMethod
{
    GET,
    POST,
    DELETE,
    PUT,
    HEAD,
    NONE
};

/*
GET /images/logo.png?size=small#top HTTP/1.1
Host: example.com
User-Agent: curl/8.0
Accept: 
*/

enum ParsingState
{
    Request_Line,                   // Detect first letter G P D H (Get, post, delete, head)
    Request_Line_Post_Put,          // Special handling for Post and Put
    Request_Line_Method,            // Detect the rest of the method
    Request_Line_First_Space,       // Detect the first space after the method
    Request_Line_URI_Path_Slash,    // Detect the first slash of the URI path
    Request_Line_URI_Path,          // Detect the rest of the URI path
    Request_Line_URI_Query,         // Detect the URI query after the '?'
    Request_Line_URI_Fragment,      // Detect the URI fragment after the '#'
    Request_Line_Ver,               // Detect the first letter of the HTTP version (H)
    Request_Line_HT,                // Vallidate T
    Request_Line_HTT,               // Validate T
    Request_Line_HTTP,              // Validate P
    Request_Line_HTTP_Slash,        // Validate '/'
    Request_Line_Major,             // Detect the major version number   
    Request_Line_Dot,               // detect .
    Request_Line_Minor,             // Detect the minor version number
    Request_Line_CR,                // Expect \r
    Request_Line_LF,                // Expect \n
    Field_Name_Start,               // Detect the start of a header field name
    Fields_End,                     // After seeing \r, expect \n to confirm end of headers.
    Field_Name,                     // Read header key until ':'
    Field_Value,                    // Detect the header field value after the ':'
    Field_Value_End,                // Detect the end of the header field value (CRLF)
    Chunked_Length_Begin,           // Detect the start of a chunked length (hex)
    Chunked_Length,                 // Read the chunked length (hex) until CRLF
    Chunked_Ignore,                 // Ignore chunk extensions until \r
    Chunked_Length_CR,              // Expect \r after chunk size
    Chunked_Length_LF,              // Expect \n after chunk size 
    Chunked_Data,                   // Read chunk data based on previously read chunk length
    Chunked_Data_CR,                // Expect \r after chunk data   
    Chunked_Data_LF,                // Expect \n after chunk data
    Chunked_End_CR,                 // Expect \r after last chunk (0 length)
    Chunked_End_LF,                 // Expect \n after last chunk (0 length)
    Message_Body,                   // Read message body based on Content-Length
    Parsing_Done                    
};

/*
Chunked 
POST /upload HTTP/1.1
Host: example.com
Transfer-Encoding: chunked

4
Wiki
5
pedia
0


*/

class HttpRequest
{
    public:
        HttpRequest();
        ~HttpRequest();

        HttpMethod                                  &getMethod();
        std::string                                 &getPath();
        std::string                                 getHeader(std::string const &);
        std::string                                 getMethodStr();
        std::string                                 &getBody();
        std::string                                 &getBoundary();
        bool                                        getMultiformFlag();

        void        setHeader(std::string &, std::string &);

        void        feed(const char *data, size_t size);
        bool        parsingCompleted();
        int         errorCode();
    
    private:
        std::string                         _path;              // Parsed target path from request line (e.g. "/upload/file.txt").
        std::map<std::string, std::string>  _request_headers;   // Header dictionary (keys normalized to lowercase).
        std::vector<u_int8_t>               _body;              // Raw body bytes collected during parsing.
        std::string                         _boundary;          // multipart/form-data boundary token when present.
        HttpMethod                          _method;            // Parsed HTTP method.
        std::map<int, std::string>          _method_str;        // Method lookup table used by request-line validation.
        ParsingState                        _state;             // Current state position.
        size_t                              _body_length;       // Expected body length when Content-Length is used.
        int                                 _error_code;        // HTTP parse error code (0 means no error).
        size_t                              _chunk_length;      // Remaining bytes for current chunk in chunked mode.
        std::string                         _storage;           // Generic temporary token buffer used across states.
        std::string                         _key_storage;       // Header key temporary storage while parsing "Key: Value".
        int                                 _method_index;      // Current character index while validating method string. (GET, method index 0 is 'G', index 1 is 'E', index 2 is 'T')
        std::string                         _body_str;          // Final body string exposed through getBody().

        bool                                _body_flag;         // True once parser decides a body is expected.
        bool                                _chunked_flag;      // True when "Transfer-Encoding: chunked" is detected.
        bool                                _multiform_flag;    // True when multipart/form-data content type is detected.

        void        _handle_headers();                          // Post-header processing: derive body mode, sizes, boundary, flags.
};
