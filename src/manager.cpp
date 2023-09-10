// #include "gtstore.hpp"

#include <openssl/sha.h>
#include <bits/stdc++.h>
#include <grpcpp/grpcpp.h>
#include "kvstorage.grpc.pb.h"
#include <chrono> // for std::chrono::seconds
#include <thread>
#include <unistd.h>
#include <grpcpp/grpcpp.h>

using namespace std;


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;


using keyvaluestorage::Ack;
using keyvaluestorage::GetRequest;
using keyvaluestorage::GetResponse;
using keyvaluestorage::PutRequest;
using keyvaluestorage::PutResponse;
using keyvaluestorage::StorageNodeStatus;
using keyvaluestorage::KeyValueStorage;
using keyvaluestorage::ControlInfo;
using keyvaluestorage::StorageNodes;
using keyvaluestorage::NodeCount;



#include <string>

int NUMBER_OF_STORAGE_SERVERS;
int REPLICATION_FACTOR;
int PORT_NUM = 9001;
// Key: Node_id, Value: is_alive or not?
unordered_map<string,bool>nodes_info;
string manager_address("0.0.0.0:9001"); 

// making all the nodes not_alive
int storage_port_no = 10001;



class ControlInfoImplementation final : public ControlInfo::Service {
  Status get_node_count(ServerContext* context, const google::protobuf::Empty* request,
                     NodeCount* reply) override {
    reply->set_numnodes(NUMBER_OF_STORAGE_SERVERS);
    return Status::OK;
  }

  int get_primary_id(string key)
  {

        //const unsigned char str[] = "Original String";
        cout<<"key: "<<key<<endl;

        int primary_node_id=0;

        for (char c : key) {
        primary_node_id = (primary_node_id + int(c)) % NUMBER_OF_STORAGE_SERVERS;
        }


        return primary_node_id;
  }

  Status get_k_nodes(ServerContext* context, const PutRequest* request,
                     StorageNodes* reply) override {
        
        // StorageNodes node_ids;

        const char* key_charArray = request->key().c_str();
        const unsigned char* key_ucharArray = reinterpret_cast<const unsigned char*>(key_charArray);
        
        int primary_node_id = get_primary_id(request->key());



        // cout<<"Initial Storage Node: "<<primary_node_id<<endl;

        string primary_address = to_string(storage_port_no+primary_node_id);
        // iterate over the map to check first k alive nodes
        auto it=nodes_info.find(primary_address);
        int required_nodes = REPLICATION_FACTOR;
        int found=0;
        if(it->second!=false)
        {
          
            reply->add_nodeid(primary_address);
            required_nodes--;
            found++;
        }
        
        // loop circular until k nodes are found
        int node_id=(primary_node_id+1)%NUMBER_OF_STORAGE_SERVERS;
        while(required_nodes!=0&&node_id!=primary_node_id)
        {
            string node_address = to_string(storage_port_no+node_id);
            auto it=nodes_info.find(node_address);
            if(it->second!=false)
            {
                reply->add_nodeid(node_address);
                required_nodes--;
                found++;
            }
            node_id = (node_id+1)%NUMBER_OF_STORAGE_SERVERS;

        }
        

         // Serialize the message to a string
        // string serialized_message;
        // node_ids.SerializeToString(&serialized_message);

        // Set the message as the reply payload
        // reply->set_payload(serialized_message);

        return Status::OK;
    }  

    Status get_next_primary_node(ServerContext* context, const GetRequest* request,
                    PutResponse * reply) override {

        const char* key_charArray = request->key().c_str();
        const unsigned char* key_ucharArray = reinterpret_cast<const unsigned char*>(key_charArray);
        cout<<"inside get_next_primary_node"<<endl;
        int primary_node_id = get_primary_id(request->key());

        int found=0;
        int node_id = (primary_node_id+1)%NUMBER_OF_STORAGE_SERVERS;
        while(!found && node_id != primary_node_id)
        {
            string node_address = to_string(storage_port_no+node_id);
            auto it=nodes_info.find(node_address);
            if(it->second!=false)
            {
                reply->set_nodeid(node_address);
                found=1;
                break;
            }
            node_id = (node_id+1)%NUMBER_OF_STORAGE_SERVERS;
        }

        return Status::OK;
    }
};

void RunServer() {

  cout<<"Starting Manager as Sever for Control Information"<<endl;

  
  ControlInfoImplementation controlInfoService;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism
  builder.AddListeningPort(manager_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which
  // communication with client takes place
  builder.RegisterService(&controlInfoService);

  // Assembling the server
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on address: " << manager_address << std::endl;

  server->Wait();
}




class StorageNodeClient {
 public:
  // StorageNodeClient(std::shared_ptr<Channel> channel)
  //     : stub_(StorageNodeStatus::NewStub(channel)) {}

  // Assembles client payload, sends it to the server, and returns its response
  int healthCheck() {
    // Data to be sent to server
    
    google::protobuf::Empty request;
    // Container for server response
    Ack reply;

    // Context can be used to send meta data to server or modify RPC behaviour
    ClientContext context;

    // Actual Remote Procedure Call
    Status status = stub_->healthCheck(&context, request , &reply);

    // Returns results based on RPC status
    if (status.ok()) {
      return reply.num();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        cout<<"RPC FAILED"<<endl;       
        return -1;
    }
  }

 private:
  std::unique_ptr<StorageNodeStatus::Stub> stub_;
};


void* healthCheckWrapper(void* arg){
 
  unordered_map<string,unique_ptr<StorageNodeStatus::Stub>>storageNodeStubs;

  int server_port_no = 10001;
  shared_ptr<Channel> storage_node_channel;
  for(int i = 0;i< NUMBER_OF_STORAGE_SERVERS; i++){
  
      std::string target_address("0.0.0.0:"+to_string(server_port_no));
      storage_node_channel = grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials());
      storageNodeStubs[to_string(server_port_no)] = StorageNodeStatus::NewStub(storage_node_channel);;
      server_port_no++;
  }

  while(1){
    int server_port_no = 10001;
      for(int i = 0;i< NUMBER_OF_STORAGE_SERVERS; i++){                            
          
          google::protobuf::Empty request;
          Ack reply;
          // Context can be used to send meta data to server or modify RPC behaviour
          ClientContext context;

          Status status =  storageNodeStubs[to_string(server_port_no)]->healthCheck(&context, request , &reply); 
          // cout<<"health check status for port: "<<server_port_no<<" is: "<<status.ok()<<endl; 
          if (status.ok()) {
            nodes_info[to_string(server_port_no)] = true;
          } else {
            nodes_info[to_string(server_port_no)] = false;
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            cout<<"RPC FAILED"<<endl;    
          }                     
          server_port_no++;
      }
          std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return nullptr;
}

void parse_args(int argc, char* argv[])
{
	int inx;
  printf("argc: %d\n", argc);
	for(inx=0; inx<argc; inx++)
	{
        if(!strcmp(argv[inx], "-n")){
            inx++;
            NUMBER_OF_STORAGE_SERVERS = stoi(argv[inx]);
            printf("Number of servers : %d\n", NUMBER_OF_STORAGE_SERVERS);
        }else if(!strcmp(argv[inx], "-k")){
            inx++;
            REPLICATION_FACTOR = stoi(argv[inx]);
            printf("Replication factor : %d\n", REPLICATION_FACTOR);
        }
	}
}

// void GTStoreManager::init() {
	
// 	cout << "Inside GTStoreManager::init()\n";
// }

// int main(int argc, char **argv) {

// 	GTStoreManager manager;
// 	manager.init();
	
// }

int main(int argc, char* argv[]) {
    parse_args(argc, argv);
    int server_port_no = 10001;
    for(int i = 0;i< NUMBER_OF_STORAGE_SERVERS; i++){
        int pid;
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(-1);
        }
        if(pid == 0){
            int child_pid = getpid();
            stringstream cmd_ss;
            cmd_ss << "./storage" << " -p " << to_string(server_port_no);
            cout<<"Command:"<< cmd_ss.str()<<endl;
            system(cmd_ss.str().c_str());
            exit(0);
        }
        server_port_no++;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout<<"Storage Node spawn"<<endl;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &healthCheckWrapper, NULL);

    RunServer();
    return 0;
}


