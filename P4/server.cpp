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
#include <signal.h>
#include <sys/wait.h>
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
using grpc::ClientContext;
using grpc::Channel;

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
using network::KeepAliveReply;
using network::KeepAliveRequest;
using network::ServerInfo;

using namespace std;

chrono::steady_clock::time_point checkTime;
const float SLAVE_CHECK_CRASH_SECONDS = 13.0f;
const int SLAVE_CHECK_LOOP_SECONDS = 9;
std::string PROCESS_ADDR = "";

// Logic and data behind the server's behavior.
class SNSImpl final : public SNS::Service {
    /********************************
     *
     * Server Relevant Variables
     *
     *******************************/
public:
    //stub for communication with routing server
    std::unique_ptr<SNS::Stub> stub_;

    void set_stub(std::string address){
        auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        stub_ = network::SNS::NewStub(channel);
        //stub_ = std::move(stub);
    }

    /* Maximum amount of posts allowed in a user's timeline
     */
    static const int MAX_TIMELINE = 20;
    void populateDB(){
        //Check to see if clients folder exists, if not create one
        std::string folder = "clients";
        struct stat sb;
        if (!(stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
        {
            int status = mkdir(folder.c_str(), 0777);
            if(status != 0){
                cout << "\033[1;4;36m[MASTER  " << PROCESS_ADDR << "]:\033[0m " << "Error: Folder creation" << endl;
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
            //make sure the file ends in .txt
            if(has_suffix(fileNames[i], ".txt")){
                //Read in the client's data
                ifstream ClientInput("clients/" + fileNames[i]); //input open
                network::User client;
                client.ParseFromIstream(&ClientInput);
                ClientInput.close(); //input close
                
                //Copy cleints data to DB field by field (unfortuanitely)
                User user(client.name());
                user.following = {client.mutable_following()->begin(), client.mutable_following()->end()};
                user.followers = {client.mutable_followers()->begin(), client.mutable_followers()->end()};
                user.clientStaleDataCount = client.clientdatastale();
                for(int i=0; i<client.timeline_size(); i++){
                    struct Post p;
                    p.name = client.timeline(i).name();
                    p.timestamp = google::protobuf::util::TimeUtil::TimestampToTimeT(client.timeline(i).time());
                    p.content = client.timeline(i).content();
                    user.timeline.push_front(p);
                    //user.timeline.insert(i, p);
                }
                UserDB.insert({ client.name(), user});
            }
        }
    }

    void Register(string machine_addr, string master_port, string routing_addr, string routing_port){
        std::string address = routing_addr + ":" + routing_port;
        set_stub(address);
        
        ClientContext context;
        ServerInfo info;
        KeepAliveReply reply;
        IReply replyStatus;

        info.set_ireplyvalue(0);
        info.set_hostname(machine_addr);
        info.set_port(master_port);

        replyStatus.grpc_status = stub_->RegisterServer(&context, info, &reply);
        return;
    }
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
            : name(name_), clientStaleDataCount(-1), connected(false) {}

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
        bool connected;
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

    void setStaleFile(std::string clientName, int staleValue){
        ifstream inputFile("clients/" + clientName + ".txt");
        network::User user;
        user.ParseFromIstream(&inputFile);
        inputFile.close();
        user.set_clientdatastale(staleValue);
        ofstream outputFile("clients/" + clientName + ".txt", ios::trunc);
        user.SerializeToOstream(&outputFile);
        outputFile.close();
        return;
    }

    void AddUserPostFile(const Post& post) {
        network::Post netPost;
        netPost.set_name(post.name);
        //netPost.set_allocated_time(&google::protobuf::util::TimeUtil::TimeTToTimestamp(post.timestamp));
        *netPost.mutable_time() = google::protobuf::util::TimeUtil::TimeTToTimestamp(post.timestamp);
        netPost.set_content(post.content);

        ifstream SenderInput("clients/" + post.name + ".txt");
        network::User sender;
        sender.ParseFromIstream(&SenderInput);
        SenderInput.close();
        
        // This is based on the implementation I used in Update() in the for loop at line 416
        network::Post* instancedPost = sender.mutable_timeline()->Add();
        // I used swap here since netPost isn't modified after this line, but if it is, you can probably use CopyFrom()
        instancedPost->CopyFrom(netPost);
        //sender.mutable_timeline()->RemoveLast();
        ofstream SenderOutput("clients/" + post.name + ".txt", ios::trunc);
        sender.SerializeToOstream(&SenderOutput);
        SenderOutput.close();
        // If you feed in a value with Add() it needs to be an rvalue (tried looking at reference for it, but it's a little confusing)
        // Using std::move() still doesn't work though, and I can't figure out why, so this is just what works for now I guess
        
        for (auto followerIt = sender.mutable_followers()->begin(); followerIt != sender.mutable_followers()->end(); followerIt++) {
            ifstream FollowerInput("clients/" + *followerIt + ".txt");
            network::User follower;
            follower.ParseFromIstream(&FollowerInput);
            FollowerInput.close();
            network::Post* followerInstancedPost = follower.mutable_timeline()->Add();
            //followerInstancedPost->Swap(&netPost);
            followerInstancedPost->CopyFrom(netPost);
            //follower.mutable_timeline()->RemoveLast();

            int stale = follower.clientdatastale();
            if (stale >= 0 && stale < 20) {
                follower.set_clientdatastale(stale+1);
            }
            ofstream FollowerOutput("clients/" + *followerIt + ".txt", ios::trunc);
            follower.SerializeToOstream(&FollowerOutput);
            FollowerOutput.close();
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
            response->set_ireplyvalue(0);

            //create a file with the client's username and place an empty User message in it
            ofstream File("clients/" + clientName + ".txt");
            network::User user;
            user.set_name(clientName);
            user.set_clientdatastale(-1);
            user.SerializeToOstream(&File);
            File.close();
        }
        else {
            // Check user is not already connected, reject if connected
            if (UserDB.at(clientName).connected) {
                response->set_ireplyvalue(1);
                return Status::OK;
            }
            UserDB.at(clientName).clientStaleDataCount = -1;

            ifstream inputFile("clients/" + clientName + ".txt");
            network::User user;
            user.ParseFromIstream(&inputFile);
            inputFile.close();
            user.set_clientdatastale(-1);
            ofstream outputFile("clients/" + clientName + ".txt", ios::trunc);
            user.SerializeToOstream(&outputFile);
            outputFile.close();

            response->set_ireplyvalue(0);
        }

        // Set User as connected
        UserDB.at(clientName).connected = true;

        return Status::OK;
    }
    
    /* RPC called when client wants to follow a user
     */
    Status Follow(ServerContext* context, const FollowRequest* request, FollowReply* reply) override {
        // Initialize IReplyValue to FAILURE_UNKNOWN by default
        reply->set_ireplyvalue(5);

        // Extract values from protocol buffer
        const string reqUser   = request->requestingclient(); //Follower
        const string followReq = request->followrequest(); //Followed

        // Find specified user, and add if not already in follow list
        // Check not self
        if (followReq == reqUser) {
            reply->set_ireplyvalue(1);
            return Status::OK;
        }

        // Check request in database
        auto dbIt = UserDB.find(followReq);
        if (dbIt == UserDB.end()) {
            // Reply FAILURE_NOT_EXISTS
            reply->set_ireplyvalue(3);
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

    /* RPC called when client wants to unfollow a user
     */
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
            reply->set_ireplyvalue(3);
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
        // Add user to own follower list
        reply->add_followers(username);
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
            setStaleFile(request->username(), requester.timeline.size());
        }

        if(quantity > 20){
            quantity = 20;
        }

        for (auto postIt = requester.timeline.begin(); postIt < requester.timeline.begin() + quantity; postIt++) {
            network::Post* currPost = reply->mutable_updated()->add_posts();
            currPost->set_name(postIt->name);
            currPost->set_content(postIt->content);
            *currPost->mutable_time() = google::protobuf::util::TimeUtil::TimeTToTimestamp(postIt->timestamp);
        }
        // Reset stale quantity to 0
        quantity = 0;
        setStaleFile(request->username(), 0);
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

        // Add to correct user timeline and follower's timelines
        AddUserPost(inPost);
        AddUserPostFile(inPost);

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

    Status Disconnect(ServerContext* context, const ClientConnect* request, ServerAllow* response) {
        const string& username = request->connectingclient();
        UserDB.at(username).connected = false;
        response->set_ireplyvalue(0);
        return Status::OK;
    }
    
    bool has_suffix(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    //Keep alive message between master server and slave server
    Status KeepAlive(ServerContext* context, const KeepAliveRequest* request, KeepAliveReply* reply) override {
        checkTime = chrono::steady_clock::now();
        reply->set_ireplyvalue(0);
        return Status::OK;
    }

}; //// END GRPC IMPLEMENTATION

void RunServer(std::string machine_addr, std::string master_port, std::string routing_addr, std::string routing_port) {
    std::string server_address(machine_addr + ":" + master_port);
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
    cout << "\033[1;4;36m[MASTER  " << PROCESS_ADDR << "]:\033[0m " << "Server listening on " << server_address << endl;
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    service.Register(machine_addr, master_port, routing_addr, routing_port);
    server->Wait();
}

int SlaveClockCheck()
{
    chrono::seconds timeSinceLastCheck = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - checkTime);
    if (timeSinceLastCheck.count() > SLAVE_CHECK_CRASH_SECONDS)
    {
        cout << "\033[1;4;36m[MASTER  " << PROCESS_ADDR << "]:\033[0m " << "Exceeded timeout on slave..." << endl;
        return 0;
    }
    return 1;
}

void ChildReaper(int sig)
{
    wait(NULL);
}

void SlaveCheckLoop(char** argv)
{
    while (true)
    {
		// Wait a little before checking
        sleep(SLAVE_CHECK_LOOP_SECONDS);
        
        const int checkResult = SlaveClockCheck();
        
        if (checkResult == 1)
        {
            cout << "\033[1;4;36m[MASTER  " << PROCESS_ADDR << "]:\033[0m " << "Slave check succeeded, waiting for " << SLAVE_CHECK_LOOP_SECONDS << " seconds." << endl;
        }
        else
        {
            // Assume slave has crashed
            cout << "\033[1;4;36m[MASTER  " << PROCESS_ADDR << "]:\033[0m " << "Assuming slave crashed, starting new process" << endl;
            auto pid = fork();
            if (pid == 0)
            {
                // Child
                pid = fork();
                if (pid == 0)
                {
                    // Grandchild
					const char* slave_server_executable = "./fbss";

					if (execvp(slave_server_executable, argv))
					{
						cout << "\033[1;4;31m[ERROR(Master " << PROCESS_ADDR << ")]:\033[0m " << "Failed to spawn new slave server" << endl;
						exit(0);
					}
                }
                else
                {
                    exit(0);
                }
            }
            else
            {
                // Parent
                // Handle SIGCHLD to prevent zombie processes
                //signal(SIGCHLD, SIG_IGN);
                wait(NULL);
            }
        }
    }
}

int main(int argc, char** argv) {
    std::string master_addr = "10.0.2.1";
    std::string master_port = "5116";
    std::string routing_addr = "10.0.2.3";
    std::string routing_port = "5115";

    if (argc != 5) {
        fprintf(stderr, "usage: ./fbsd <machine address> <server port> <routing address> <routing port>\n");
        return 1;
    }
    else {
        master_addr = std::string(argv[1]);
        master_port = std::string(argv[2]);
        routing_addr = std::string(argv[3]);
        routing_port = std::string(argv[4]);
    }

    checkTime = chrono::steady_clock::now();
    // Start slave timeout loop
    thread SlaveCheckThread(SlaveCheckLoop, argv);

    PROCESS_ADDR = master_addr;

    RunServer(master_addr, master_port, routing_addr, routing_port);

    // Join slave timeout loop
    SlaveCheckThread.join();

    return 0;
};
