#pragma once
#include <iostream>
#include "httpParser.h"

class RequestManager
{
    explicit RequestManager();
public:
    RequestManager* instance();

    static std::string loadRequest(HttpRequest);
    static void httpResponse();
    static void executeOperation(std::string action, std::string left, std::string right);
};
