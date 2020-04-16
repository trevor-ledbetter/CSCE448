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
    std::string hostname;
    std::string port;

    bool operator==(const masterServer& a) const{
        return (hostname == a.hostname && port == a.port);
    }
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
        if(server_list.size() == 0){
            //No available servers
            response->set_ireplyvalue(5);
            return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Unavailable Server");
        }else{
            response->set_hostname(available_server.hostname);
            response->set_port(available_server.port);
            response->set_ireplyvalue(0);
            return Status::OK;
        }
    }

    Status RegisterServer(ServerContext* context, const ServerInfo* server_info, KeepAliveReply* response) override {
        std::cout << "registering yo motherfuckah!\n";
        masterServer new_server;
        new_server.hostname = server_info->hostname();
        new_server.port = server_info->port();
        
        //Add new_server to the list
        server_list.push_back(new_server);

        //if this is the first and/or only server make it the available one
        if(server_list.size() == 1){
            available_server.hostname = server_list[0].hostname;
            available_server.port = server_list[0].port;
        }

        response->set_ireplyvalue(0);
        return Status::OK;
    }

    Status Crash(ServerContext* context, const ServerInfo* server_info, KeepAliveReply* response) override {
        std::cout << "Crash!\n";
        masterServer crashed_server;
        crashed_server.hostname = server_info->hostname();
        crashed_server.port = server_info->port();

        //Remove server_info from server_list
        for(int i=0; i<server_list.size(); i++){
            if(server_list[i] == crashed_server){
                server_list.erase(server_list.begin() + i);
                break;
            }
        }

        //if server_info was the available server, choose a new available server
        if(crashed_server == available_server){
            //make sure there are available servers
            if(server_list.size() > 0){
                available_server.hostname = server_list[0].hostname;
                available_server.port = server_list[0].port;
            }else{
                std::cout << "Currently no available servers, please wait...\n";
                //for(;;);
            }
        }
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
        fprintf(stderr, "usage: ./fbrs <port number>\n");
        return 1;
    }
    else {
        port = std::string(argv[1]);
    }
    RunRouter(port);

    return 0;
};
