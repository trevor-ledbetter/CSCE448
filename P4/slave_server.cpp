#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include <sys/wait.h>
#include <fcntl.h>



#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "network.grpc.pb.h"
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/message.h>

using grpc::ClientContext;
using grpc::Status;

using network::SNS;
using network::KeepAliveReply;
using network::KeepAliveRequest;
using network::ServerInfo;


using namespace std;

class Slave{
    private:
        std::string master_port;
        std::string routing_port;

        std::unique_ptr<SNS::Stub> master_stub_;
        std::unique_ptr<SNS::Stub> routing_stub_;

        void set_stub(std::unique_ptr<SNS::Stub> &stub_, std::string address){
            auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
            stub_ = network::SNS::NewStub(channel);
        }
    public:
        Slave(std::string _master_port, std::string _routing_port){
            master_port = _master_port; 
            routing_port = _routing_port;

            //Create stubs for the master and the routing servers
            set_stub(master_stub_, "localhost:" + master_port);
            set_stub(routing_stub_, "localhost:" + routing_port);
        }

        void RunSlave(char** argv) {
            KeepAliveRequest keep_request;
            KeepAliveReply keep_reply;
            IReply replyStatus;
            keep_request.set_ireplyvalue(0);
            
            //Every 3 sec send KeepAlive to server. If a send is unsucessfull retry .25 sec later.
            //if that one does not work, notify the routing server, and restart the master.
            while(1){
                cout << "\033[1;4;35m[SLAVE " << master_port << "]:\033[0m " << "Checking master..." << endl;
                sleep(3);
                ClientContext context;
                replyStatus.grpc_status = master_stub_->KeepAlive(&context, keep_request, &keep_reply);
                
                if ( !replyStatus.grpc_status.ok() ){
                    //Keep alive failed! Try again.
                    sleep(.25);
                    ClientContext context2;
                    replyStatus.grpc_status = master_stub_->KeepAlive(&context2, keep_request, &keep_reply);
                    if ( !replyStatus.grpc_status.ok() ){
                        //Keep alive failed again, assume server is down.
                        //Notify the routing server!

                        ServerInfo info;
                        info.set_port(master_port);
                        info.set_hostname("localhost");
                        
                        //This loop keeps trying to message the router until sucessfull. Should work 1st time
                        while( !replyStatus.grpc_status.ok() ){
                            ClientContext context3;
                            replyStatus.grpc_status = routing_stub_->Crash(&context3, info, &keep_reply);
							if (!replyStatus.grpc_status.ok()) sleep(2);
						}
                        cout << "\033[1;4;35m[SLAVE " << master_port << "]:\033[0m " << "Sent Crash message to router" << endl;

                        //restart the master here!
                        int status = fork();
                        if(status == 0){
							//child
                            status = fork();
                            if (status == 0) {
                                // grandchild
							    //Restart the master
                                cout << "\033[1;4;35m[SLAVE " << master_port << "]:\033[0m " << "Restarting master" << endl;
								int return_int = execvp("./fbsd", argv);
								if (return_int == -1) {
                                    cout << "\033[1;4;35m[SLAVE " << master_port << "]:\033[0m " << "Error: Execvp() failed!" << endl;
								}
                            }
                            else {
                                exit(0);
                            }
                        }
                        else {
                            wait(NULL);
                        }
                        //wait(&status);
                        //wait(NULL);
                    }
                }
            } //end while
        } //end RunSlave
};

//Used to reap zombies created by fork() and execvp
void SIGCHLD_handler(int sig){
    wait(NULL);
}

int main(int argc, char** argv) {
    std::string master_port = "5116";
    std::string routing_port = "5115";

    if (argc != 3) {
        fprintf(stderr, "usage: ./fbss <master port> <routing port>\n");
        return 1;
    }else {
        master_port = std::string(argv[1]);
        routing_port = std::string(argv[2]);
    }

    //signal(SIGCHLD, SIGCHLD_handler);
    Slave slave_server(master_port, routing_port);
    slave_server.RunSlave(argv);

    return 0;
};
