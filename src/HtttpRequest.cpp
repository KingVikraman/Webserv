#include "../inc/HttpRequest.hpp"

// Helper func

/* Trim leading and trailing  spaces */
void trimStr(std::string &str)
{
    static const char *spaces = " \t";
    str.erase(0, str.find_first_not_of(spaces));
    str.erase(str.find_last_not_of(spaces) + 1);
}

void toLower(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
}

bool validUriChar(uint8_t ch)
{
    if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
        ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

bool invalidUriPosition(std::string path) // prevent path from going before root
{
    std::string tmp(path);
    char *res = strtok((char *)tmp.c_str(), "/"); // tokenize the path by '/'
    int pos = 0;
    while (res != NULL)
    {
        if (!strcmp(res, ".."))
            pos--;
        else
            pos++;
        if (pos < 0)
            return (1);
        res = strtok(NULL, "/"); // get the next token
    }
    return (0);
}

bool    isToken(uint8_t ch)
{
    if (ch == '!' || (ch >= '#' && ch <= '\'') || ch == '*'|| ch == '+' || ch == '-'  || ch == '.' ||
       (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= '^' && ch <= '`') ||
       (ch >= 'a' && ch <= 'z') || ch == '|')
        return (true);
    return (false);
}

HttpRequest::HttpRequest()
{
    _method_str[::GET] = "GET";
    _method_str[::POST] = "POST";
    _method_str[::DELETE] = "DELETE";
    _method_str[::PUT] = "PUT";
    _method_str[::HEAD] = "HEAD";
    _path = "";
    _query = "";
    _fragment = "";
    _body_str = "";
    _error_code = 0;
    _chunk_length = 0;
    _method = NONE;
    _method_index = 1;
    _state = Request_Line;
    _fields_done_flag = false;
    _body_flag = false;
    _body_done_flag = false;
    _chunked_flag = false;
    _body_length = 0;
    _storage = "";
    _key_storage = "";
    _multiform_flag = false;
    _boundary = "";
}

HttpRequest::~HttpRequest() {}

// getters
HttpMethod &HttpRequest::getMethod()
{
    return (_method);
}

std::string &HttpRequest::getPath()
{
    return (_path);
}

std::string &HttpRequest::getQuery()
{
    return (_query);
}

std::string &HttpRequest::getFragment()
{
    return (_fragment);
}

std::string HttpRequest::getHeader(std::string const &name)
{
    return (_request_headers[name]);
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const // dictionary
{
    return (this->_request_headers);
}

std::string HttpRequest::getMethodStr()
{
    return (_method_str[_method]);
}

std::string &HttpRequest::getBody()
{
    return (_body_str);
}

std::string HttpRequest::getServerName()
{
    return (this->_server_name);
}

bool HttpRequest::getMultiformFlag()
{
    return (this->_multiform_flag);
}

std::string &HttpRequest::getBoundary()
{
    return (this->_boundary);
}

// Setters
void HttpRequest::setBody(std::string body)
{
    _body.clear();
    _body.insert(_body.begin(), body.begin(), body.end());
    _body_str = body;
}

void HttpRequest::setMethod(HttpMethod &method)
{
    _method = method;
}

void HttpRequest::setHeader(std::string &name, std::string &value)
{
    trimStr(value);
    toLower(name);
    _request_headers[name] = value;
}

void HttpRequest::setMaxBodySize(size_t size)
{
    _max_body_size = size;
}

void HttpRequest::feed(char *data, size_t size)
{
    u_int8_t character;
    std::stringstream s; // Do check on this

    for (size_t i = 0; i < size; ++i)
    {
        character = data[i];
        switch (_state)
        {
        case Request_Line:
        {
            if (character == 'G')
                _method = GET;
            else if (character == 'P')
            {
                _state = Request_Line_Post_Put;
                break;
            }
            else if (character == 'D')
                _method = DELETE;
            else if (character == 'H')
                _method = HEAD;
            else
            {
                _error_code = 501;
                std::cout << "Method Error Request_Line Unexpected byte: '" << character
                          << "' (" << static_cast<int>(character) << ")" << std::endl;
                return;
            }
            _state = Request_Line_Method;
            break;
        }
        case Request_Line_Post_Put:
        {
            if (character == 'O')
                _method = POST;
            else if (character == 'U')
                _method = PUT;
            else
            {
                _error_code = 501;
                std::cout << "Method Error Request_Line Unexpected byte: '" << character
                          << "' (" << static_cast<int>(character) << ")" << std::endl;
                return;
            }
            _method_index++;
            _state = Request_Line_Method;
            break;
        }
        case Request_Line_Method: // check the rest of the string to make sure correct
        {
            if (character == _method_str[_method][_method_index])
                _method_index++;
            else
            {
                _error_code = 501;
                std::cout << "Method Error Request_Line Unexpected byte: '" << character
                          << "' (" << static_cast<int>(character) << ")" << std::endl;
                return;
            }

            if ((size_t)_method_index == _method_str[_method].length())
                _state = Request_Line_First_Space;
            break;
        }
        case Request_Line_First_Space:
        {
            if (character != ' ')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                return;
            }
            _state = Request_Line_URI_Path_Slash;
            continue;
        }
        case Request_Line_URI_Path_Slash:
        {
            if (character == '/')
            {
                _state = Request_Line_URI_Path;
                _storage.clear();
            }
            else
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_URI_Path_Slash)" << std::endl;
                return;
            }
            break;
        }
        case Request_Line_URI_Path:
        {
            if (character == ' ') // URL eneded, now expect HTTP version
            {
                _state = Request_Line_Ver;
                _path.append(_storage);
                _storage.clear();
                continue;
            }
            else if (character == '?') // URL query string starts, now expect query string
            {
                _state = Request_Line_URI_Query;
                _path.append(_storage);
                _storage.clear();
                continue;
            }
            else if (character == '#') // URL fragment starts, now expect fragment
            {
                _state = Request_Line_URI_Fragment;
                _path.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!validUriChar(character))
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_URI_Path)" << std::endl;
                return;
            }
            else if (i > MAX_URI_LENGTH)
            {
                _error_code = 414;
                std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                return;
            }
            break;
        }
        case Request_Line_URI_Query: // for query string
        {
            if (character == ' ') // url eneded, now expect HTTP version
            {
                _state = Request_Line_Ver;
                _query.append(_storage);
                _storage.clear();
                continue;
            }
            else if (character == '#')
            {
                _state = Request_Line_URI_Fragment;
                _query.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!validUriChar(character))
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_URI_Query)" << std::endl;
                return;
            }
            else if (i > MAX_URI_LENGTH)
            {
                _error_code = 414;
                std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                return;
            }
            break;
        }
        case Request_Line_URI_Fragment:
        {
            if (character == ' ')
            {
                _state = Request_Line_Ver;
                _fragment.append(_storage);
                _storage.clear();
                continue;
            }
            else if (!validUriChar(character))
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_URI_Fragment)" << std::endl;
                return;
            }
            else if (i > MAX_URI_LENGTH)
            {
                _error_code = 414;
                std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                return;
            }
            break;
        }
        case Request_Line_Ver:
        {
            if (invalidUriPosition(_path))
            {
                _error_code = 400;
                std::cout << "Request URI ERROR: goes before root !" << std::endl;
                return;
            }
            if (character != 'H') // Should be H for HTTP version if not error
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_Ver)" << std::endl;
                return;
            }
            _state = Request_Line_HT;
            break;
        }
        case Request_Line_HT:
        {
            if (character != 'T')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_HT)" << std::endl;
                return;
            }
            _state = Request_Line_HTT;
            break;
        }
        case Request_Line_HTT:
        {
            if (character != 'T')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_HTT)" << std::endl;
                return;
            }
            _state = Request_Line_HTTP;
            break;
        }
        case Request_Line_HTTP:
        {
            if (character != 'P')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_HTTP)" << std::endl;
                return;
            }
            _state = Request_Line_HTTP_Slash;
            break;
        }
        case Request_Line_HTTP_Slash:
        {
            if (character != '/')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_HTTP_Slash)" << std::endl;
                return;
            }
            _state = Request_Line_Major;
            break;
        }
        case Request_Line_Major:
        {
            if (!isdigit(character))
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_Major)" << std::endl;
                return;
            }
            _ver_major = character;

            _state = Request_Line_Dot;
            break;
        }
        case Request_Line_Dot:
        {
            if (character != '.')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_Dot)" << std::endl;
                return;
            }
            _state = Request_Line_Minor;
            break;
        }
        case Request_Line_Minor:
        {
            if (!isdigit(character))
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_Minor)" << std::endl;
                return;
            }
            _ver_minor = character;
            _state = Request_Line_CR;
            break;
        }
        case Request_Line_CR: // request line should end with CRLF, so expect \r first
        {
            if (character != '\r')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_CR)" << std::endl;
                return;
            }
            _state = Request_Line_LF;
            break;
        }
        case Request_Line_LF:
        {
            if (character != '\n')
            {
                _error_code = 400;
                std::cout << "Bad Character (Request_Line_LF)" << std::endl;
                return;
            }
            _state = Field_Name_Start;
            _storage.clear();
            continue;
        }
        case Field_Name_Start:
        {
            if (character == '\r')
                _state = Fields_End;
            else if (isToken(character))
                _state = Field_Name;
            else
            {
                _error_code = 400;
                std::cout << "Bad Character (Field_Name_Start)" << std::endl;
                return;
            }
            break;
        }
        }
    }
}