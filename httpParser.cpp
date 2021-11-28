#include "httpParser.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>



HttpRequest HttpParser::parseRequest(const std::string & raw_request){
	HttpRequest http_request;
	const std::string delimiter = "\r\n";
	std::vector<std::string> out;
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;

	while ((pos = raw_request.find(delimiter, prev)) != std::string::npos) {
		out.push_back(raw_request.substr(prev, pos - prev));
		prev = pos + delimiter.size(); ;
	}

	std::string tmp;
	std::vector<std::string> typePr;
	std::istringstream parsStr(out[0]);
	int vector_size = out.size();

	while (std::getline(parsStr, tmp, ' ')) {
		typePr.push_back(tmp);
	}

	http_request.type = typePr[0];
	http_request.protocol = typePr[2];
	bool isBodyParse = false;
	std::cout << "Output headers:" << std::endl;

	for (int i = 1; i < vector_size; i++) {
		if (out[i].empty()) {
			isBodyParse = true;
			continue;
		}
		if (isBodyParse)
		{
			http_request.body += out[i];
		}
		else
		{
			pos = out[i].find(':');
			std::string key = out[i].substr(0, pos);
			std::string value = out[i].substr(pos + 1, out[i].size());
			http_request.headers[key] = value;
			std::cout << key << ":" << value << std::endl;
		}

	} 
	std::cout << "Output body:" << std::endl << http_request.body << std::endl;
	return http_request;
}