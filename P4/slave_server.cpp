#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <queue>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "network.grpc.pb.h"
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/message.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using network::SNS;
using network::ClientConnect;
using network::ServerInfo;
using network::KeepAliveReply;
using network::KeepAliveRequest;


using namespace std;

void RunSlave(std::string port) {
    std::string router_address("0.0.0.0:" + port);
    /*RoutingServer service;
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(router_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Router listening on " << router_address << std::endl;
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.

    server->Wait();*/
}

int main(int argc, char** argv) {
    std::string master_port = "5116";
    std::string routing_port = "5115";
    //int opt = 0;
    //while ((opt = getopt(argc, argv, "p:")) != -1){
    //    switch(opt) {
    //        case 'p':
    //            port = optarg;break;
    //        default:
    //            std::cerr << "Invalid Command Line Argument\n";
    //    }
    //}
    if (argc != 3) {
        fprintf(stderr, "usage: ./fbsd <master port> <routing port>\n");
        return 1;
    }
    else {
        master_port = std::string(argv[1]);
        routing_port = std::string(argv[2]);
    }
    RunSlave(port);

    return 0;
};
