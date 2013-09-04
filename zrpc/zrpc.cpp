#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "glog/logging.h"
#include "zmq.hpp"
#include "zrpc.hpp"
#include "rpc.pb.h"

using std::string;
using namespace std::chrono;

// add back the missing device function in the cpp binding
namespace zmq
{
    inline void device (int device_, void * insocket_, void* outsocket_)
    {
        int rc = zmq_device (device_, insocket_, outsocket_);
        if (rc != 0)
            throw error_t ();
    }
}

namespace
{
    bool echo_test(const string& input, string& output)
    {
        output = input;
        return true;
    }

    bool wait_test(const string& input, string& output)
    {
        int wait = atoi(input.c_str()) + 200;
        LOG(INFO) << "wait " << input.c_str() << ":" <<  wait;
        if(wait>0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait));
        }
        return true;
    }
}

namespace zrpc {

struct RpcServerImpl : RpcServer
{
    RpcServerImpl(int port, int threads) :
        port_(port), threads_(threads)
    {
        addMethod("echo_test", echo_test);
        addMethod("wait_test", wait_test);
    }

    virtual ~RpcServerImpl() noexcept
    {
    }

    virtual void addMethod(const string& name, ServiceFunc func)
    {
        methods_[name] = func;
    }

    virtual void run()
    {
        zmq::context_t context(1);
        zmq::socket_t clients(context, ZMQ_ROUTER);

        string bind_string = string("tcp://*:") + std::to_string(port_);
        clients.bind(bind_string.c_str());

        zmq::socket_t workers(context, ZMQ_DEALER);
        workers.bind("inproc://workers");

        auto worker_routine = [&]() {
            zmq::socket_t socket(context, ZMQ_REP);
            socket.connect("inproc://workers");

            while (true) {
                bool success;
                zmq::message_t message;
                socket.recv(&message);

                // parse request
                zrpc::Request request;
                success = request.ParseFromArray(message.data(), message.size());
                if (!success) {
                    LOG(ERROR) << "Error parsing request message!";
                    continue;
                }

                // call method
                string response_message;
                auto mit = methods_.find(request.method());
                if (mit == methods_.end()) {
                    LOG(ERROR) << "Unknown method: " << request.method();
                    // create response
                    // TODO return error message
                    zrpc::Response response;
                    // and reply
                    zmq::message_t reply(response.ByteSize());
                    response.SerializeToArray(reply.data(), reply.size());
                    socket.send(reply);
                } else {
                    ServiceFunc func = mit->second;
                    auto t1 = high_resolution_clock::now();
                    success = false;
                    std::string exception_str;
                    try {
                        success = func(request.param(), response_message);
                    } catch (std::exception& e) {
                        exception_str = e.what();
                    }
                    auto t2 = high_resolution_clock::now();
                    auto time_span = duration_cast<duration<double>>(t2 - t1);

                    // create response
                    zrpc::Response response;
                    response.set_time_elapsed((int)(time_span.count() * 1000));
                    response.set_data(response_message);

                    // and reply
                    zmq::message_t reply(response.ByteSize());
                    response.SerializeToArray(reply.data(), reply.size());

                    // let the message fly
                    size_t reply_size = reply.size();
                    socket.send(reply);

                    if (success) {
                        LOG(INFO) << "M: " << request.method() << ", reply size: " << reply_size << ", duration: " << time_span.count();
                    } else {
                        LOG(ERROR) << "E: " << request.method() << ", e.what(): " << exception_str;
                    }
                }
            }
        };

        for (int i = 0; i < threads_; i++) {
            std::thread t(worker_routine);
            t.detach();
        }

        LOG(INFO) << "Server is up and running at port: " << port_ << ", threads: " << threads_;
        zmq::device(ZMQ_QUEUE, clients, workers);
    }

private:
    int port_, threads_;
    std::unordered_map<string, ServiceFunc> methods_;
};

RpcServer* RpcServer::CreateServer(int port, int threads)
{
    return new RpcServerImpl(port, threads);

}

} // namespace zrpc
