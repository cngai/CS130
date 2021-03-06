#ifndef HEALTH_HANDLE_H
#define HEALTH_HANDLE_H

#include <iostream>

#include "request_handle.h"
#include "session.h"
#include "logger.h"

class HealthHandle : public Requesthandle {
public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    Response handleRequest(const Request &request_);
    Logger* logger = Logger::getLogger();
private:
    Response reply_;
};

#endif
