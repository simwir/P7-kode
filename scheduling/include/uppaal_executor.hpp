#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <string>
#include <iostream>

namespace scheduling {

class SchedulingException : public std::exception {
    std::string message;
public:
    SchedulingException(const std::string& inmessage){
        message = inmessage;
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class UppaalExecutor {
public:
    UppaalExecutor(const char* modelPath, const char* queriesPath) : modelPath(modelPath), queriesPath(queriesPath) { }
    std::string execute();
private:
    const char* modelPath;
    const char* queriesPath;
};
    
}

#endif // UPPAAL_EXECUTOR_HPP