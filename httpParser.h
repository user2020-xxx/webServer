#pragma once
#include <string>
#include <unordered_map>

struct HttpRequest
{
	std::string type;
	std::string protocol;
	std::unordered_map <std::string, std::string> headers;
	std::string body;
};

class HttpParser
{
public:
	static	HttpRequest parseRequest(const std::string & raw_request);
};

