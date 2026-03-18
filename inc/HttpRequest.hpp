#pragma once

#include "Webserv.hpp"

enum HttpMethod
{
    GET,
    POST,
    DELETE,
    PUT,
    HEAD,
    NONE
};

enum ParsingState
{
    Request_Line,                   // Detect first letter G P D H
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

class HttpRequest
{
    public:
        HttpRequest();
        ~HttpRequest();

        HttpMethod                                  &getMethod();
        std::string                                 &getPath();
        std::string                                 &getQuery();
        std::string                                 &getFragment();
        std::string                                 getHeader(std::string const &);
		const std::map<std::string, std::string>    &getHeaders() const;
		std::string                                 getMethodStr();
        std::string                                 &getBody();
        std::string                                 getServerName();
        std::string                                 &getBoundary();
        bool                                        getMultiformFlag();

        void        setMethod(HttpMethod &);
        void        setHeader(std::string &, std::string &);
        void        setMaxBodySize(size_t);
        void        setBody(std::string name);

        void        feed(const char *data, size_t size);
        bool        parsingCompleted();
        void        printMessage();
        void        clear();
        int         errorCode();
        bool        keepAlive();
        void        cutReqBody(int bytes);
    
    private:
        std::string                         _path;
        std::string                         _query;
        std::string                         _fragment;
        std::map<std::string, std::string>  _request_headers;
        std::vector<u_int8_t>               _body;
        std::string                         _boundary;
        HttpMethod                          _method;
        std::map<int, std::string>          _method_str;
        ParsingState                        _state;
        size_t                              _max_body_size;
        size_t                              _body_length;
        int                                 _error_code;
        size_t                              _chunk_length;
        std::string                         _storage;
        std::string                         _key_storage;
        int                                 _method_index;
        u_int8_t                            _ver_major;
        u_int8_t                            _ver_minor;
        std::string                         _server_name;
        std::string                         _body_str;

        bool                                _fields_done_flag;
        bool                                _body_flag;
        bool                                _body_done_flag;
        bool                                _complete_flag;
        bool                                _chunked_flag;
        bool                                _multiform_flag;

        void        _handle_headers();
};