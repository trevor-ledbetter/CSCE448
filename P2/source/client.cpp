#include <iostream>
#include <memory>
#include <thread>
//#include <vector>
#include <ctime>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "network.grpc.pb.h"
#include <google/protobuf/util/time_util.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using network::SNS;
using network::ClientConnect;
using network::ServerAllow;
using network::FollowRequest;
using network::FollowReply;
using network::UnfollowRequest;
using network::UnfollowReply;
using network::ListRequest;
using network::ListReply;
using network::UpdateRequest;
using network::UpdateReply;
using network::Post;
using network::PostReply;

/**
* Amount of time to sleep update thread between update RPC calls
*/
const int UPDATE_SLEEP_MS = 500;

volatile sig_atomic_t receivedSignal;
void setSignal(int sig) {
    receivedSignal = sig;
}

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {
            }
        ~Client() {
            if (chatUpdateThread.joinable()) {
                chatUpdateThread.join();
            }
        }
        void set_stub(std::string address){
            auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
            stub_ = network::SNS::NewStub(channel);
            //stub_ = std::move(stub);
        }
        
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();

        /**
         * Posts to user and follower timeline on server
         * @param msg Message to post
         */
        virtual IReply sendPost(const std::string& msg);

        /**
         * Requests updates to the timeline from the server and prints if any
         */
        virtual void checkForUpdate();

        /**
         * Calls checkForUpdate() at regular intervals and displays updates to the screen
         */
        virtual void updateTimeline();

        /**
         * Handles termination signals and disconnects from server
         */
        virtual void handleDisconnect();

        /** 
         Returns proper IStatus from RPC status id
         * @param statusID RPC id number
         */
        virtual IStatus getStatus(const int statusID);
    private:
        std::string hostname;
        std::string username;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        //std::unique_ptr<NameOfYourStubClass::Stub> stub_;
        std::unique_ptr<SNS::Stub> stub_;

        std::thread chatUpdateThread;
        std::thread signalCheckingThread;

};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "5116";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    signal(SIGINT, setSignal);
    signal(SIGTERM, setSignal);
    signal(SIGKILL, setSignal);

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------
    
    std::string address = this->hostname + ":" + this->port;
    set_stub(address); //sets Client's stub with a channel created with address
    
    ClientContext clientCtx;
    ClientConnect connectionReq;
    ServerAllow serverResponse;
    IReply replyStatus;

    connectionReq.set_connectingclient(username);
    replyStatus.grpc_status = stub_->InitConnect(&clientCtx, connectionReq, &serverResponse);
    if (replyStatus.grpc_status.ok()) {
        replyStatus.comm_status = getStatus(serverResponse.ireplyvalue());
    }
    else {
        replyStatus.comm_status = FAILURE_UNKNOWN;
    }

    if (replyStatus.comm_status == SUCCESS) {
        signalCheckingThread = std::thread(&Client::handleDisconnect, this);
        
        return 1;
    }
    else return -1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// - JOIN/LEAVE and "<username>" are separated by one space.
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Join" service method for JOIN command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Join(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
	// ------------------------------------------------------------
    
    //maybe need to do some more error checking??
    std::size_t index = input.find_first_of(" ");
    std::string cmd = input.substr(0, index);\
    std::string argument = input.substr(index+1, (input.length()-index));

    IReply reply;
    ClientContext clientCtxt;
    if(cmd == "FOLLOW"){
        FollowRequest followReq;
        followReq.set_followrequest(argument);
        followReq.set_requestingclient(username);
        FollowReply followRep;
        reply.grpc_status = stub_->Follow(&clientCtxt, followReq, &followRep);
        if (reply.grpc_status.ok()) {
            reply.comm_status = getStatus(followRep.ireplyvalue());
        }
        else {
            reply.comm_status = FAILURE_UNKNOWN;
        }

    }else if(cmd == "UNFOLLOW"){
        UnfollowRequest unfollowReq;
        unfollowReq.set_unfollowrequest(argument);
        unfollowReq.set_requestingclient(username);
        UnfollowReply unfollowRep;
        reply.grpc_status = stub_->Unfollow(&clientCtxt, unfollowReq, &unfollowRep);
        if (reply.grpc_status.ok()) {
            reply.comm_status = getStatus(unfollowRep.ireplyvalue());
        }
        else {
            reply.comm_status = FAILURE_UNKNOWN;
        }

    }else if(cmd == "LIST"){
        ListRequest listReq;
        listReq.set_username(username);
        ListReply listRep;
        reply.grpc_status = stub_->List(&clientCtxt, listReq, &listRep);
        if (reply.grpc_status.ok()) {
            //Copy names from the ListReply to the IReply, which is returned
            reply.all_users = {listRep.mutable_users()->begin(), listRep.mutable_users()->end()};
            reply.followers = {listRep.mutable_followers()->begin(), listRep.mutable_followers()->end()};
            reply.comm_status = getStatus(listRep.ireplyvalue());
        }
        else {
            reply.comm_status = FAILURE_UNKNOWN;
        }
        
    }else if(cmd == "TIMELINE"){
        reply.comm_status = SUCCESS;

    }else if(cmd == "DEBUG"){
        UpdateRequest upReq;
        upReq.set_username(username);
        UpdateReply upRep;
        reply.grpc_status = stub_->Update(&clientCtxt, upReq, &upRep);
        if (reply.grpc_status.ok()) {
            reply.comm_status = SUCCESS;
        }
        else {
            reply.comm_status = FAILURE_UNKNOWN;
        }
        
        if (reply.comm_status == SUCCESS && upRep.has_updated()) {
            for (auto It = upRep.updated().posts().begin(); It != upRep.updated().posts().end(); It++) {
                std::cout << It->content() << std::endl;
            }
        }

    }else if (cmd == "SEND") {
        sendPost(argument);

    }else if (cmd == "UPDATE") {
        checkForUpdate();

    }else{
        std::cout << "Invalid Command\n";

    }

    return reply;
}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------

    // Initial update check to get recent posts in timeline
    checkForUpdate();

    // Spawn update handler thread
    chatUpdateThread = std::thread(&Client::updateTimeline, this);

    // Get input from user continuously
    while (true) {
        const std::string& msg = getPostMessage();
        const IReply reply = sendPost(msg);
        if (reply.comm_status != SUCCESS) {
            std::cerr << "Error posting message" << std::endl;

        }
    }
}

IReply Client::sendPost(const std::string& msg)
{
    Post post;
    post.set_name(username);
    post.set_content(msg);
    *post.mutable_time() = google::protobuf::util::TimeUtil::GetCurrentTime();
    PostReply postRep;
    IReply reply;
    ClientContext clientCtxt;
    reply.grpc_status = stub_->SendPost(&clientCtxt, post, &postRep);
    if (reply.grpc_status.ok()) {
        reply.comm_status = SUCCESS;
    }
    else {
        reply.comm_status = FAILURE_UNKNOWN;
    }
    return reply;
}

void Client::checkForUpdate()
{
    UpdateRequest request;
    UpdateReply requestReply;
    ClientContext clientCtxt;
    request.set_username(username);
    Status stats = stub_->Update(&clientCtxt, request, &requestReply);
    if (stats.ok()) {
        if (requestReply.has_updated()) {
            for (auto postsIt = requestReply.updated().posts().begin(); postsIt != requestReply.updated().posts().end(); postsIt++) {
                const std::string& name = postsIt->name();
                const std::string& msg = postsIt->content();
                time_t time = google::protobuf::util::TimeUtil::TimestampToTimeT(postsIt->time());
                displayPostMessage(name, msg, time);
            }
        }
    }
    else {
        std::cerr << "Error occurred receiving update" << std::endl;
    }
}

void Client::updateTimeline()
{
    while (true) {
        // Get an update
        checkForUpdate();
        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SLEEP_MS));
    }
}

void Client::handleDisconnect()
{
    while (true) {
        if (receivedSignal == SIGTERM || receivedSignal == SIGKILL || receivedSignal == SIGINT) {
			ClientContext clientCtx;
			ClientConnect connectReq;
			connectReq.set_connectingclient(username);
			ServerAllow serverRes;
			grpc::Status reply;
			reply = stub_->Disconnect(&clientCtx, connectReq, &serverRes);
			if (reply.ok()) {
			}
			else {
			}
			exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SLEEP_MS));
    }
}

IStatus Client::getStatus(const int statusID)
{
    switch (statusID) {
    case 0:
        return SUCCESS;
    case 1:
        return FAILURE_ALREADY_EXISTS;
    case 2:
        return FAILURE_NOT_EXISTS;
    case 3:
        return FAILURE_INVALID_USERNAME;
    case 4:
        return FAILURE_INVALID;
    case 5:
        return FAILURE_UNKNOWN;
    }
    return FAILURE_UNKNOWN;
}
