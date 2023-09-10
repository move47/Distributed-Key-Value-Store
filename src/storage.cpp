// #include "gtstore.hpp"

// void GTStoreStorage::init() {
	
// 	cout << "Inside GTStoreStorage::init()\n";
// }

// int main(int argc, char **argv) {

// 	GTStoreStorage storage;
// 	storage.init();
	
// }

#include <bits/stdc++.h>
#include <grpcpp/grpcpp.h>
#include "kvstorage.grpc.pb.h"

using namespace std;


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using keyvaluestorage::Ack;
using keyvaluestorage::GetRequest;
using keyvaluestorage::GetResponse;
using keyvaluestorage::PutRequest;
using keyvaluestorage::PutResponse;
using keyvaluestorage::StorageNodeStatus;
using keyvaluestorage::KeyValueStorage;
using keyvaluestorage::NodeSize;



#include <string>

string PORT_NUM;

class StorageNodeStatusImplementation final : public StorageNodeStatus::Service {
  Status healthCheck(ServerContext* context, const google::protobuf::Empty* request,
                    Ack* reply) override {
    reply->set_num(1);
    return Status::OK;
  }
};

class KeyValueStorageImplementation final : public KeyValueStorage::Service {
    std::map<std::string, vector<std::string>>kvData;
    
    Status get(ServerContext* context, const GetRequest* request,
                     GetResponse* reply) override {
    
    std::string key = request->key();

    if (kvData.find(key) != kvData.end()){
        for(string val : kvData[key]){
          reply->add_value(val);
        }
        
        return Status::OK;
    }else{
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Key not found");
    }
  }

  Status put(ServerContext* context, const PutRequest* request,
                     PutResponse* reply) override {
    std::string key = request->key();
    vector<std::string> value;
    for(int i = 0;i<request -> value_size();i++){
      value.push_back(request->value(i));
    }
    
    kvData[key] = value;
    reply->set_nodeid(PORT_NUM);
    return Status::OK;
  }

  Status size(ServerContext* context, const google::protobuf::Empty* request, NodeSize* reply) override {

    reply->set_size(kvData.size());
    return Status::OK;
  }
};

void RunServer() {

  std::string server_address("0.0.0.0:"+PORT_NUM);
  cout<<"server address: "<<server_address<<endl;
  StorageNodeStatusImplementation storageNodeStatusService;
  KeyValueStorageImplementation keyValueStorageSerice;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which
  // communication with client takes place
  builder.RegisterService(&storageNodeStatusService);
  builder.RegisterService(&keyValueStorageSerice);

  // Assembling the server
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Storage Node listening on port: " << server_address << std::endl;

  server->Wait();
}


void parse_args(int argc, char* argv[])
{
	int inx;
    printf("argc: %d\n", argc);
	for(inx=0; inx<argc; inx++)
	{
        if(!strcmp(argv[inx], "-p")){
            inx++;
            PORT_NUM = argv[inx];
            cout<<"Port to listen: "<<PORT_NUM<<endl;
        }
	}
}

int main(int argc, char* argv[]) {
    parse_args(argc,argv);
    RunServer();
    return 0;
}