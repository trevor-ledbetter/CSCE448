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
using network::ClientConnect;
using network::ServerAllow;
using network::FollowRequest;
using network::FollowReply;
using network::UnfollowRequest;
using network::UnfollowReply;
using network::UnfollowRequest;
using network::UnfollowReply;
using network::DebugRequest;
using network::DebugReply;

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
        User(const string& name_)
            : name(name_) {}

        string name;
        vector<string> following;
        vector<Post> timeline;
    };

    unordered_map<string, User> UserDB;


    //// GRPC IMPLEMENTATION

    Status InitConnect(ServerContext* context, const ClientConnect* connection, ServerAllow* response) override {
        // Fail by default
        response->set_ireplyvalue(5);

        // Check if user already exists, if not, then add entry to database
        const string clientName = connection->connectingclient();

        auto connector = UserDB.find(clientName);
        if (connector == UserDB.end()) {
            UserDB.insert({ clientName, User(clientName) });
            cout << "Adding user:\t" << clientName << endl;
            response->set_ireplyvalue(0);
        }
        else {
            cout << "Not adding duplicate user:\t" << clientName << endl;
            response->set_ireplyvalue(0);
        }

        return Status::OK;
    }

    Status Follow(ServerContext* context, const FollowRequest* request, FollowReply* reply) override {
        // Initialize IReplyValue to FAILURE_UNKNOWN by default
        reply->set_ireplyvalue(5);

        // Extract values from protocol buffer
        const string reqUser   = request->requestingclient();
        const string followReq = request->followrequest();

        // Find specified user, and add if not already in follow list
        // Check not self
        if (followReq == reqUser) {
            reply->set_ireplyvalue(4);
            return Status::OK;
        }

        // Check request in database
        auto dbIt = UserDB.find(followReq);
        if (dbIt == UserDB.end()) {
            // Reply FAILURE_NOT_EXISTS
            reply->set_ireplyvalue(2);
            return Status::OK;
        }

        // Check request in following list
        auto& requester = UserDB.at(reqUser);
        auto& followVec = requester.following;
        auto followVecIt = find(followVec.begin(), followVec.end(), followReq);
        if (followVecIt == followVec.end()) {
            followVec.push_back(followReq);
            reply->set_ireplyvalue(0);
            return Status::OK;
        }
        else {
            // Reply FAILURE_ALREADY_EXISTS
            reply->set_ireplyvalue(1);
            return Status::OK;
        }
        
        return Status::CANCELLED;
    }

    Status Unfollow(ServerContext* context, const UnfollowRequest* request, UnfollowReply* reply) override {
        // Initialize IReplyValue to FAILURE_UNKNOWN by default
        reply->set_ireplyvalue(5);

        // Extract values from protocol buffer
        const string reqUser = request->requestingclient();
        const string unfollowReq = request->unfollowrequest();

        // Find specified user, and add if not already in follow list
        // Check request in following list
        auto& requester = UserDB.at(reqUser);
        auto& followVec = requester.following;
        auto followVecIt = find(followVec.begin(), followVec.end(), unfollowReq);
        if (followVecIt != followVec.end()) {
            followVec.erase(followVecIt);
            reply->set_ireplyvalue(0);
            return Status::OK;
        }
        else {
            // Reply FAILURE_NOT_EXISTS
            reply->set_ireplyvalue(2);
            return Status::OK;
        }

        return Status::CANCELLED;
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