#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <string>
#include <iostream>

namespace scheduling {

struct SchedulingException : public std::exception {
     const char* what() const noexcept { return "Could not run scheduling"; }
};

class UppaalExecutor {
public:
    UppaalExecutor(const char* model, const char* queries) : model(model), queries(queries) { }
    std::string execute();
private:
    const char* model;
    const char* queries;
};
}

#endif // UPPAAL_EXECUTOR_HPP
