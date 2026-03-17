#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

class HttpResponse
{
    private:
        int _status_code;
        std::string _status_message;
        std::map<std::string, std::string> _headers;
        std::string _body;

        std::string _getStatusMessage(int code) const;

    public:
        HttpResponse();
        ~HttpResponse();

        void setStatus(int code);
        void setHeader(const std::string &key, const std::string &value);
        void setBody(const std::string &body);

        std::string build() const;

        static HttpResponse ok(const std::string &body, const std::string &content_type);
        static HttpResponse created(const std::string &body);
        static HttpResponse noContent();
        static HttpResponse notFound();
        static HttpResponse methodNotAllowed();
        static HttpResponse internalError();
        static HttpResponse badRequest();
};

#endif
