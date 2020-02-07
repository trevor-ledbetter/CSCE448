#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>

#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "network.grpc.pb.h"
#include <google/protobuf/util/time_util.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using network::SNS;
using network::FollowRequest;
using network::FollowReply;
using network::UnfollowRequest;
using network::UnfollowReply;
using network::UnfollowRequest;
using network::UnfollowReply;
using network::DebugRequest;
using network::DebugReply;
// Not including message type includes because there are also server side implementations

using namespace std;

// Logic and data behind the server's behavior.
class SNSImpl final : public SNS::Service {
    //// Server Relevant Variables
    struct Post {
        string name;
        time_t timestamp;
        string content;
    };

    struct User {
        string name;
        vector<string> following;
        vector<Post> timeline;
    };

    unordered_map<string, User> UserDB;

    //// GRPC IMPLEMENTATION

    Status Follow(ServerContext* context, const FollowRequest* request, FollowReply* reply) override {
        reply->set_ireplyvalue(0);
        
        return Status::OK;
    }

    Status Unfollow(ServerContext* context, const UnfollowRequest* request, UnfollowReply* reply) override {
        reply->set_ireplyvalue(0);
        return Status::OK;
    }

    Status ExecDebug(ServerContext* context, const DebugRequest* debug, DebugReply* dbgrep) override {
        using namespace google::protobuf;
        std::cout << "Timestamped in : " << util::TimeUtil::ToString(debug->timein()) << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(2));
        Timestamp tStamp = util::TimeUtil::GetCurrentTime();
        *dbgrep->mutable_timeout() = tStamp;
        std::cout << "Timestamped out: " << util::TimeUtil::ToString(tStamp) << std::endl;
        return Status::OK;
    }

    //// END GRPC IMPLEMENTATION
};

void RunServer(std::string port) {
    std::string server_address("0.0.0.0:" + port);
    SNSImpl service;
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.

    server->Wait();
}

int main(int argc, char** argv) {
    std::string port = "5116";
    int opt = 0;
    while ((opt = getopt(argc, argv, "p:")) != -1){
        switch(opt) {
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }
    RunServer(port);

    return 0;
};