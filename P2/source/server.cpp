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
using network::ListReply;
using network::ListRequest;
using network::UpdateRequest;
using network::UpdateReply;
using network::PostReply;

using namespace std;

// Logic and data behind the server's behavior.
class SNSImpl final : public SNS::Service {
    /********************************
     *
     * Server Relevant Variables
     *
     *******************************/
public:
    /* Maximum amount of posts allowed in a user's timeline
     */
    static const int MAX_TIMELINE = 20;

private:
    /* Contains data relevant to posts
     */
    struct Post {
        string name;
        time_t timestamp;
        string content;
    };

    /* Contains data relevant to users/clients
     */
    struct User {
        User(const string& name_)
            : name(name_), clientStaleDataCount(-1) {}

        string name;
        deque<Post> timeline;
        /**
         * Value which determines how much to send in UpdateReply
         * < 0 => First time update is called, send full timeline
         * > 0 => Send quantity of posts
         */
        int clientStaleDataCount;
        vector<string> following;
        vector<string> followers;
    };

    /* Holds all users with key = username, value = User
     */
    unordered_map<string, User> UserDB;

    /* Add post to user's and follower's timelines and flags their data as stale
     * Requires Post& to be added to all timelines
     */
    void AddUserPost(const Post& post) {
        User& sender = UserDB.at(post.name);
        
        // Add to user's timeline, resizing if too large
        sender.timeline.push_front(post);
        if (sender.timeline.size() > MAX_TIMELINE) {
            sender.timeline.resize(MAX_TIMELINE);
        }

        // Add to follower's timelines and update stale value
        for (auto followerIt = sender.followers.begin(); followerIt != sender.followers.end(); followerIt++) {
            User& currentFollower = UserDB.at(*followerIt);
            currentFollower.timeline.push_front(post);
            if (currentFollower.timeline.size() > MAX_TIMELINE) {
                currentFollower.timeline.resize(MAX_TIMELINE);
            }
            if (currentFollower.clientStaleDataCount >= 0 && currentFollower.clientStaleDataCount < 20) {
                currentFollower.clientStaleDataCount += 1;
            }
        }
    }

    /*****************************
     *
     * GRPC IMPLEMENTATION
     *
     ****************************/

    /* RPC called on client construction
     * Handles adding users to database when detected as new 
     */
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
            cout << "\tSetting stale data value to -1" << endl;
            UserDB.at(clientName).clientStaleDataCount = -1;
            response->set_ireplyvalue(0);
        }

        return Status::OK;
    }

    /* RPC called when client wants to follow a user
     */
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
            cout << reqUser << " failed to follow " << followReq << endl;
            return Status::OK;
        }

        // Check request in database
        auto dbIt = UserDB.find(followReq);
        if (dbIt == UserDB.end()) {
            // Reply FAILURE_NOT_EXISTS
            reply->set_ireplyvalue(2);
            cout << reqUser << " failed to follow " << followReq << endl;
            return Status::OK;
        }

        // Check request in following list
        auto& requester = UserDB.at(reqUser);
        auto& followVec = requester.following;
        auto followVecIt = find(followVec.begin(), followVec.end(), followReq);
        if (followVecIt == followVec.end()) {
            // Add to following list
            followVec.push_back(followReq);
            // Add to other user's follower's list
            dbIt->second.followers.push_back(reqUser);
            reply->set_ireplyvalue(0);
            cout << reqUser << " is now following " << followReq << endl;
            return Status::OK;
        }
        else {
            // Reply FAILURE_ALREADY_EXISTS
            reply->set_ireplyvalue(1);
            cout << reqUser << " failed to follow " << followReq << endl;
            return Status::OK;
        }
        
        return Status::CANCELLED;
    }

    /* RPC called when client wants to unfollow a user
     */
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
            // Erase from following list
            followVec.erase(followVecIt);
            
            // Erase from other user's followers list
            auto& otherFollowerVec = UserDB.at(unfollowReq).followers;
            auto userInOtherVecIt = find(otherFollowerVec.begin(), otherFollowerVec.end(), reqUser);
            if (userInOtherVecIt != otherFollowerVec.end()) {
                otherFollowerVec.erase(userInOtherVecIt);
            }
            else {
                // If the requesting user wasn't found in the other's followers list
                // There is an inherent error in the database
                reply->set_ireplyvalue(5);
                return Status::CANCELLED;
            }
            
            reply->set_ireplyvalue(0);
            cout << reqUser << " is no longer following " << unfollowReq << endl;
            return Status::OK;
        }
        else {
            // Reply FAILURE_NOT_EXISTS
            reply->set_ireplyvalue(2);
            cout << reqUser << " failed to unfollow " << unfollowReq << endl;
            return Status::OK;
        }

        return Status::CANCELLED;
    }

    /* RPC called when client wants to get a list of all users currently in the database
     */
    Status List(ServerContext* context, const ListRequest* request, ListReply* reply) override {
        // Initialize IReplyValue to FAILURE_UNKNOWN by default
        reply->set_ireplyvalue(5);

        //Iterate over entire list of users and copy their names to reply's list "users"
        for (auto it : UserDB){
            std::string name = it.first;
            reply->add_users(name);
        }

        //Find the DB entry that corresponds to the requester's username
        std::string username = request->username();
        auto& requester = UserDB.at(username);

        //Access this entry's followers list and copy the follower names to reply's list "followers"
        auto& followVec = requester.followers;
        for(auto it : followVec){
            std::string name = it;
            reply->add_followers(name);
        }
        reply->set_ireplyvalue(0);

        return Status::OK;
    }
    
    /* RPC called from client timeline mode at regular intervals that requests any updates to the client timeline
     */
    Status Update(ServerContext* context, const UpdateRequest* request, UpdateReply* reply) override {
        // Send necessary posts to the client
        User& requester = UserDB.at(request->username());
        int& quantity = requester.clientStaleDataCount;
        // Make quantity the timeline size if initializing
        if (quantity == -1) {
            quantity = requester.timeline.size();
        }
        for (auto postIt = requester.timeline.begin(); postIt < requester.timeline.begin() + quantity; postIt++) {
            network::Post* currPost = reply->mutable_updated()->add_posts();
            currPost->set_name(postIt->name);
            currPost->set_content(postIt->content);
            *currPost->mutable_time() = google::protobuf::util::TimeUtil::TimeTToTimestamp(postIt->timestamp);
        }
        cout << "Sending updated timeline to " << requester.name << endl;
        cout << "\t" << "Number of posts: " << quantity << endl;
        // Reset stale quantity to 0
        quantity = 0;
        return Status::OK;
    }

    /* RPC called from client timeline mode when user wishes to post
     */
    Status SendPost(ServerContext* context, const network::Post* postReq, PostReply* postRep) override {
        using namespace google::protobuf;
        // Put data into new SNSImpl::Post
        Post inPost;
        inPost.name = postReq->name();
        inPost.timestamp = util::TimeUtil::TimestampToTimeT(postReq->time());
        inPost.content = postReq->content();

        cout << "Received Post from " << inPost.name << endl;
        cout << "\t" << "Time: " << ctime(&inPost.timestamp) << endl;
        cout << "\t" << "Content: " << inPost.content << endl;
        // Add to correct user timeline and follower's timelines
        AddUserPost(inPost);

        postRep->set_ireplyvalue(0);
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
