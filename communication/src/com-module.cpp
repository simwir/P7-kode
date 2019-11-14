/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "communication/com-module.hpp"
#include "tcp/connection.hpp"
#include "tcp/exception.hpp"

#include <iostream>
#include <mutex>
#include <thread>

namespace communication {

std::mutex mutex;

std::pair<std::string, std::string> split_message(const std::string &message)
{
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::pair{start, end};
}

Function ComModule::parse_function(const std::string &function)
{
    if (function == "get_robot_info") {
        return Function::get_robot_info;
    }
    else if (function == "post_robot_info") {
        return Function::post_robot_info;
    }
    else {
        throw tcp::MessageException(function);
    }
}

void ComModule::get_robot_info(std::shared_ptr<tcp::Connection> conn)
{
    std::unique_lock<std::mutex> lock(mutex);
    Json::Value result = robot_info.to_json();
    lock.unlock();
    conn->send(result.toStyledString());
}

void ComModule::post_robot_info(const std::string &robot_payload)
{
    std::scoped_lock<std::mutex> lock(mutex);
    robot::Info info = robot::Info::from_json(robot_payload);
    robot_info[info.id] = info;
}

void ComModule::call_function(Function function, const std::string &parameters,
                              std::shared_ptr<tcp::Connection> conn)
{
    switch (function) {
    case Function::post_robot_info:
        post_robot_info(parameters);
        break;
    case Function::get_robot_info:
        get_robot_info(conn);
        break;
    default:
        throw UnknownFunctionException("Function not implemented");
    }
}

void ComModule::parse_message(std::shared_ptr<tcp::Connection> conn)
{
    while (true) {
        try {
            auto message = conn->receive_blocking();
            std::pair<std::string, std::string> result = split_message(message);
            Function function = parse_function(result.first);
            call_function(function, result.second, conn);
        }
        catch (tcp::MalformedMessageException &e) {
            conn->send(e.what());
            std::cerr << "MalformedMessageException" << e.what() << std::endl;
        }
        catch (UnknownFunctionException &e) {
            conn->send(e.what());
            std::cerr << "UnknownFunctionException" << e.what() << std::endl;
        }
        catch (UnknownParameterException &e) {
            conn->send(e.what());
            std::cerr << "UnknownParameterException" << e.what() << std::endl;
        }
        catch (tcp::ReceiveException &e) {
            std::cerr << "ReceiveException" << e.what() << std::endl;
            break;
        }
    }
}

void ComModule::start_broadcasting()
{
    std::cout << "Waiting for connections on port: " << server.get_port() << std::endl;
    while (true) {
        std::shared_ptr<tcp::Connection> conn = server.accept();
        std::thread t1(&ComModule::parse_message, this, conn);
        t1.detach();
    }
}
} // namespace communication
