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

using namespace std;


struct masterServer{
    std::string ip;
    std::string port;
};

// Logic and data behind the server's behavior.
class RoutingServer final : public SNS::Service {
    /********************************
     *
     * Server Relevant Variables
     *
     *******************************/
public:
    //current available server
    masterServer available_server;
    //list of all servers, that could be the new available server
    std::vector<masterServer> server_list;


    /*****************************
     *
     * GRPC IMPLEMENTATION
     *
     ****************************/

    Status Connect(ServerContext* context, const ClientConnect* connection, ServerInfo* response) override {
        //Fail by default
        response->set_ireplyvalue(5);
        
        response->set_ip(available_server.ip);
        response->set_port(available_server.port);

        return Status::OK;
    }

    Status Crash(ServerContext* context, const ServerInfo* server_info, KeepAliveReply* response) override {
        
        //Remove server_info from server_list

        //if server_info was the available server, choose a new available server

        //simple response
        response->set_ireplyvalue(0);

        return Status::OK;
    }
}; //End grpc implementation


void RunRouter(std::string port) {
    std::string router_address("0.0.0.0:" + port);
    RoutingServer service;
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

    server->Wait();
}

int main(int argc, char** argv) {
    std::string port = "5116";
    //int opt = 0;
    //while ((opt = getopt(argc, argv, "p:")) != -1){
    //    switch(opt) {
    //        case 'p':
    //            port = optarg;break;
    //        default:
    //            std::cerr << "Invalid Command Line Argument\n";
    //    }
    //}
    if (argc != 2) {
        fprintf(stderr, "usage: ./fbsd <port number>\n");
        return 1;
    }
    else {
        port = std::string(argv[1]);
    }
    RunRouter(port);

    return 0;
};
