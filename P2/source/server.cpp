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
    //// Server Relevant Variables
    struct Post {
        string name;
        time_t timestamp;
        string content;
    };

    struct User {
        User(const string& name_)
            : name(name_), clientDataStale(-1) {}

        string name;
        deque<Post> timeline;
        int clientDataStale;
        vector<string> following;
        vector<string> followers;
    };

    unordered_map<string, User> UserDB;

    // Add post to user's and follower's timelines and flags data as stale
    void AddUserPost(const Post& post) {

    }

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

            //create a file with the client's username and place an empty User message in it
            ofstream File("clients/" + clientName + ".txt");
            network::User user;
            user.set_name(clientName);
            user.SerializeToOstream(&File);
            File.close();
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
        const string reqUser   = request->requestingclient(); //Follower
        const string followReq = request->followrequest(); //Followed

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
            // Add to following list
            followVec.push_back(followReq);
            // Add to other user's follower's list
            dbIt->second.followers.push_back(reqUser);

            //Repeat for persistent memory
            ifstream FollowedInput("clients/" + followReq + ".txt"); //input open
            network::User followed;
            followed.ParseFromIstream(&FollowedInput);
            FollowedInput.close(); //input close
            followed.add_followers(reqUser);
            ofstream FollowedOutput("clients/" + followReq + ".txt"); //output open
            followed.SerializeToOstream(&FollowedOutput);
            FollowedOutput.close(); //output close

            ifstream FollowerInput("clients/" + reqUser + ".txt"); //input open
            network::User follower;
            follower.ParseFromIstream(&FollowerInput);
            FollowerInput.close(); //input close
            follower.add_following(followReq);
            ofstream FollowerOutput("clients/" + reqUser + ".txt"); //output open
            follower.SerializeToOstream(&FollowerOutput);
            FollowerOutput.close(); //output close
            
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
        const string reqUser = request->requestingclient(); //reqUser is unfollowing unfollowReq
        const string unfollowReq = request->unfollowrequest(); //unfollowed

        // Find specified user, and add if not already in follow list
        // Check request in following list
        auto& requester = UserDB.at(reqUser);
        auto& followVec = requester.following;
        auto followVecIt = find(followVec.begin(), followVec.end(), unfollowReq);
        if (followVecIt != followVec.end()) {
            // Erase from following list
            followVec.erase(followVecIt);

            //Erase from Persistant Memory
            ifstream UnfollowerInput("clients/" + reqUser + ".txt"); //input open
            network::User unfollower;
            unfollower.ParseFromIstream(&UnfollowerInput);
            UnfollowerInput.close(); //input close
            for(int i=0; i<unfollower.following_size(); i++){
                if(unfollower.following(i) == unfollowReq){
                    unfollower.mutable_following()->SwapElements(i, unfollower.following_size()-1);
                    unfollower.mutable_following()->RemoveLast();
                    break;
                }
            }
            ofstream UnfollowerOutput("clients/" + reqUser + ".txt", ios::trunc); //output open, and delete previous contents!
            unfollower.SerializeToOstream(&UnfollowerOutput);
            UnfollowerOutput.close(); //output close
            
            // Erase from other user's followers list
            auto& otherFollowerVec = UserDB.at(unfollowReq).followers;
            auto userInOtherVecIt = find(otherFollowerVec.begin(), otherFollowerVec.end(), reqUser);
            if (userInOtherVecIt != otherFollowerVec.end()) {
                otherFollowerVec.erase(userInOtherVecIt);

                //Erase from persistant memory
                ifstream UnfollowedInput("clients/" + unfollowReq + ".txt"); //input open
                network::User unfollowed;
                unfollowed.ParseFromIstream(&UnfollowedInput);
                UnfollowedInput.close(); //input close
                for(int i=0; i<unfollowed.followers_size(); i++){
                    if(unfollowed.followers(i) == reqUser){
                        unfollowed.mutable_followers()->SwapElements(i, unfollowed.followers_size()-1);
                        unfollowed.mutable_followers()->RemoveLast();
                        break;
                    }
                }
                ofstream UnfollowedOutput("clients/" + unfollowReq + ".txt", ios::trunc); //output open, and delete previous contents!
                unfollower.SerializeToOstream(&UnfollowerOutput);
                UnfollowerOutput.close(); //output close
            }
            else {
                // If the requesting user wasn't found in the other's followers list
                // There is an inherent error in the database
                reply->set_ireplyvalue(5);
                return Status::CANCELLED;
            }
            
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

    Status List(ServerContext* context, const ListRequest* request, ListReply* reply) override {
        // Initialize IReplyValue to FAILURE_UNKNOWN by default
        reply->set_ireplyvalue(5);

        //Iterate over entire list of users and copy their names to reply's list "users"
        for (auto it : UserDB){
            std::string name = it.first;
            reply->add_users(name);
        }

        //Find the DB entry that cooresponds to the requester's username
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

    Status Update(ServerContext* context, const UpdateRequest* request, UpdateReply* reply) override {
        network::Post* testPost = reply->mutable_updated()->add_posts();
        testPost->set_name("default");
        *testPost->mutable_time() = google::protobuf::util::TimeUtil::GetCurrentTime();
        testPost->set_content("testPost 1");

        return Status::OK;
        
    }

    // Handle client post sending
    // Returns server Status
    // Requires ServerContext*, network::Post* (NOT SNSImpl::Post), PostReply*
    Status SendPost(ServerContext* context, const network::Post* postReq, PostReply* postRep) override {
        using namespace google::protobuf;
        // Put data into new SNSImpl::Post
        Post inPost;
        inPost.name = postReq->name();
        inPost.timestamp = util::TimeUtil::TimestampToTimeT(postReq->time());
        inPost.content = postReq->content();
        // Add to correct user timeline and follower's timelines

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

    public:
    void populateDB(){
        //Check to see if clients folder exists, if not create one
        std::string folder = "clients";
        struct stat sb;
        if (!(stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
        {
            int status = mkdir(folder.c_str(), 0777);
            if(status != 0){
                std::cout << "Error: Folder creation" << std::endl;
            }
        }

        //Obtain list of file names in the clients directory
        std::vector<std::string> fileNames;
        DIR* dirp = opendir(folder.c_str());
        struct dirent* dp;
        while((dp = readdir(dirp)) != NULL){
            fileNames.push_back(dp->d_name);
        }
        closedir(dirp);

        //For each client's file create an entry in the server's DB
        for(int i=0; i<fileNames.size(); i++){
            //Read in the client's data
            ifstream ClientInput("clients/" + fileNames[i]); //input open
            network::User client;
            client.ParseFromIstream(&ClientInput);
            ClientInput.close(); //input close
            
            //Copy cleints data to DB field by field (unfortuanitely)
            User user(client.name());
            user.following = {client.mutable_following()->begin(), client.mutable_following()->end()};
            user.followers = {client.mutable_followers()->begin(), client.mutable_followers()->end()};
            user.timeline.resize(client.timeline_size());
            for(int i=0; i<client.timeline_size(); i++){
                struct Post p;
                p.name = client.timeline(i).name();
                p.timestamp = google::protobuf::util::TimeUtil::TimestampToTimeT(client.timeline(i).time());
                p.content = client.timeline(i).content();
                user.timeline.push_back(p);
            }
            UserDB.insert({ client.name(), user});
        }
    }
    //// END GRPC IMPLEMENTATION
};

void RunServer(std::string port) {
    std::string server_address("0.0.0.0:" + port);
    SNSImpl service;
    service.populateDB();
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
