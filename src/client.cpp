#include "gtstore.hpp"

#include<bits/stdc++.h>
#include <openssl/sha.h>
#include <grpcpp/grpcpp.h>
#include "kvstorage.grpc.pb.h"

#include <thread>
#include <pthread.h>
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
using keyvaluestorage::StorageNodes;
using keyvaluestorage::NodeCount;
using keyvaluestorage::KeyValueStorage;
using keyvaluestorage::ControlInfo;


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;

string manager_port = "9001";
string manager_address("0.0.0.0:9001"); // manager address

class Client{
  private:
    int NUMBER_OF_STORAGE_SERVERS;
    unique_ptr<ControlInfo::Stub> manager_stub;
    
    // Initialize the lock
    
   int get_primary_id(string key)
  {
        // cout<<"Input KEY: "<<key<<endl;
        int primary_node_id=0;
        for (char c : key) {
            primary_node_id = (primary_node_id + int(c)) % NUMBER_OF_STORAGE_SERVERS;
        }
        return primary_node_id;
  }

  public:
    unordered_map<string,shared_ptr<KeyValueStorage::Stub>>stub_info;
    int verbose;
    pthread_mutex_t lock;
    map<string,int>StoragePorts;
    Client() {
        // Initialize the lock
        pthread_mutex_init(&lock, nullptr);
    }
    int get_cluster_info()
    {
        shared_ptr<Channel> manager_channel = grpc::CreateChannel(manager_address, grpc::InsecureChannelCredentials());

        manager_stub = ControlInfo::NewStub(manager_channel); // channel for manager node
        
        google::protobuf::Empty request;
        ClientContext context;
        // Get count of storage nodes from the manager node
        NodeCount reply;
        // Actual Remote Procedure Call
        Status status = manager_stub->get_node_count(&context, request, &reply);
        if(status.ok())
        {
            // cout<<"Number of nodes heard from manager: "<<reply.numnodes()<<endl;
            NUMBER_OF_STORAGE_SERVERS = reply.numnodes();
            return 1;
        }
        else
        {
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            cout<< "RPC failed while getting storage node count from manager"<<endl;
            return 0;
        }
    }

    /*
    1. Asks manager to send list of k avialable nodes
    2. If Failed: return -1 
    3. Else: 
    4.      Connect with every node among k nodes list and requests to store the <key,value>
    5.       If failes to store with any node: return
    6.       Else: return successfull ; meaning the <key,value> got stored at k available nodes        
    */
    void upload_value(PutRequest put_req, string node_id,shared_ptr<KeyValueStorage::Stub> &storage_stub,unordered_map<string,int>&storagePorts)
    {
        PutResponse put_reply;
        ClientContext context;
        string Storage_node_IP_Address = "0.0.0.0:"+node_id;

        //cout<<"Storage_IP: "<<Storage_node_IP_Address<<endl;

        // shared_ptr<Channel>storage_node_channel = grpc::CreateChannel(Storage_node_IP_Address, grpc::InsecureChannelCredentials());
    
        // shared_ptr<KeyValueStorage::Stub> storage_stub = KeyValueStorage::NewStub(storage_node_channel); 
        
        // cout<<"Get response from storage"<<endl; 
        Status status = storage_stub->put(&context, put_req, &put_reply);
        //storagePorts[node_id]=1;
        if(status.ok())
        {
            // cout<<"key "<<key<<" stored at node ip: "<<Storage_node_IP_Address<<endl;
            //success = 1;
            //cout<<"Status Ok"<<endl;
            //pthread_mutex_lock(&lock);
            storagePorts[node_id]=1;
            //pthread_mutex_unlock(&lock);
            return;
        }
        else
        {
           
            //success = 0;
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            cout<< "RPC failed while storing value at "<<Storage_node_IP_Address<<endl;
            return;
        }
    }

    int store_val(string key, vector<string> &valueVec)
    {

        PutRequest put_req;
        put_req.set_key(key);
        for(string val : valueVec){
            put_req.add_value(val); 
        }
    
        StorageNodes reply;
        ClientContext context;
        // cout<<"calling manager"<<" key: "<<key<<" value: "<<value<<endl;
        // Actual Remote Procedure Call
        Status status = manager_stub->get_k_nodes(&context, put_req, &reply);
        // Returns results based on RPC status
        if (status.ok()) {

            // cout<<"call to manager success"<<endl;
            // Get the vector field values
            //vector<string>storagePorts;
            
            std::vector<std::thread> threads;
            unordered_map<string,int>storagePorts;
            // StoragePorts = storagePorts;
            for (int i = reply.nodeid_size()-1; i>=0 ; i--) 
            {
                string node_id = reply.nodeid(i);
               
                string Storage_node_IP_Address = "0.0.0.0:"+node_id;

                //cout<<"nodeip: "<<Storage_node_IP_Address<<endl;
               
                // Create a stub for the service
                // Marking storage node unreachalb initally
                storagePorts[node_id]=0;
                if(stub_info.find(node_id)==stub_info.end())
                {
                    // cout<<"Node id not found, creating stub"<<endl;
                   shared_ptr<Channel>storage_node_channel = grpc::CreateChannel(Storage_node_IP_Address, grpc::InsecureChannelCredentials());
    
                   stub_info[node_id] = KeyValueStorage::NewStub(storage_node_channel); // channel for manager node
                }

                /*
                PutResponse put_reply;
                // cout<<"calling storage"<<endl;
                ClientContext context1;
                Status status = stub_info[node_id]->put(&context1, put_req, &put_reply);
                // cout<<"Get response from storage"<<endl;
                if(status.ok())
                {
                    // cout<<"key "<<key<<" stored at node ip: "<<Storage_node_IP_Address<<endl;
                    //storagePorts.push_back(node_id);
                    storagePorts[node_id]=1;
                }
                else
                {
                    std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
                    cout<< "RPC failed while storing value at "<<Storage_node_IP_Address<<endl;
                    return 0;
                }
                */

                int success=0;
                
                threads.emplace_back(std::thread(&Client::upload_value,this,put_req,node_id,ref(stub_info[node_id]),ref(storagePorts)));

                // if(success==0)
                // {
                //     std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
                //     cout<< "RPC failed while storing value at "<<Storage_node_IP_Address<<endl;
                //     //return 0;
                // }
                
                // else
                // storagePorts[node_id]=1;

                
            }
            //wait for the threads to complete
            for (auto& thread : threads) {
                thread.join();
            }

            if(verbose) cout<<"key "<<key<<" stored at Storage Nodes: ";
            
                
                auto it = storagePorts.begin();
                while(it!=storagePorts.end())
                {
                    if(it->second==0)
                    {
                        return 0;
                    }
                    if(verbose)
                    {
                        cout<<stoi((it->first))%10000<<"  ";//<<it->second;
                    }
                    it++;
                }
                if(verbose)
                cout<<endl;

            

            return 1;

    
        } 
        else {

            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            cout<< "Intial RPC with manager Failed"<<endl;
            return 0;
        }

    }



    // contact the manager to get the total number of nodes
    
    // NUMBER_OF_STORAGE_SERVERS = contact_manager_for_storage_nodes_count()
    
    // try to fetch the value for this key from the primary_node..
    
    //  fetch from primary_node() if status_ok: SUCCESSFUL=1
    
    // If SUCCESSFUL:
        // done return.. val
    // ELSE:
        // Contact the manager for the node_id to fetch from..

        // call_manager_to_get_next_node_id()
        // fetch value from the returned node_id ()

    int get_val(std::string key, vector<string>&valueVec)
    {
       
        GetRequest get_req;

        get_req.set_key(key);


        const char* key_charArray = key.c_str();
        const unsigned char* key_ucharArray = reinterpret_cast<const unsigned char*>(key_charArray);
        
        int primary_node_id = get_primary_id(key);

        int port_number = 10001+(primary_node_id%NUMBER_OF_STORAGE_SERVERS);
        // cout<<"nodeid:"<<port_number<<endl;
        string node_id = to_string(port_number);
        string primary_address = "0.0.0.0:"+to_string(port_number);

        
        if(stub_info.find(node_id)==stub_info.end())
        {
            shared_ptr<Channel>storage_node_channel = grpc::CreateChannel(primary_address, grpc::InsecureChannelCredentials());
    
            stub_info[node_id] = KeyValueStorage::NewStub(storage_node_channel);// channel for manager node
        }
        
        ClientContext context;
        GetResponse get_reply;

        Status status = stub_info[node_id]->get(&context, get_req, &get_reply);
        if(status.ok())
        {
            string print_value = "";
            for (int i = 0; i < get_reply.value_size(); i++) 
            {
                valueVec.push_back(get_reply.value(i));
                print_value += get_reply.value(i) + " ";
            }
            if(verbose) cout<<"Got response from master node: "<<primary_node_id+1<<" for key: "<<key<<" value: "<<print_value<<endl;

            return 1;
        }
        else if (status.error_code() == grpc::StatusCode::NOT_FOUND) {
            if(verbose) cout<<"Master node exist, but key not found. Not calling manager to get updated master node"<<endl;
            return 0;
        }
        else
        {
            if(verbose) cout<<"Master node unavailable. Contacting the manager node to get the updated master node"<<endl;

            // contact the manager for the valid storage node to ask..
            
        
            PutResponse reply;
            ClientContext context1;
            
            // Actual Remote Procedure Call
            Status status = manager_stub->get_next_primary_node(&context1, get_req, &reply);

            // Returns results based on RPC status
            if(status.ok())
            {
                if(verbose) cout<<"valid node heard from the manager: "<<reply.nodeid()<<endl;
                string next_primary_node_id = reply.nodeid();
                string next_primary_address = "0.0.0.0:"+next_primary_node_id;
                if(stub_info.find(next_primary_node_id)==stub_info.end())
                {
                    shared_ptr<Channel>storage_node_channel = grpc::CreateChannel(next_primary_address, grpc::InsecureChannelCredentials());
                    stub_info[next_primary_node_id] = KeyValueStorage::NewStub(storage_node_channel); // channel for manager node
                }
                

                GetResponse get_reply;
                ClientContext context2;

                Status status = stub_info[next_primary_node_id]->get(&context2, get_req, &get_reply);
                if(status.ok())
                {

                    string print_value = "";
                    for (int i = 0; i < get_reply.value_size(); i++) 
                    {
                        valueVec.push_back(get_reply.value(i));
                        print_value += get_reply.value(i) + " ";
                    }
                    if(verbose) cout<<"Heard from node: "<<stoi(next_primary_node_id)%10000<<" value: "<<print_value<<endl;
                    return 1;
                }
                else
                {
                    if(verbose) cout<<"Not able to read value from the next primary valid node heard from the manager"<<endl;
                    return 0;
                }
            }
            else
            {

                std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
                cout<< "RPC failed while getting next valid primary storage node from manager"<<endl;
                return -1;
            }
        }

    }
    
};

Client client;
int verbo;


void GTStoreClient::init(int id, int verbose) {

		cout << "Inside GTStoreClient::init() for client " << id << "\n";
		client_id = id;
        verbo = verbose;
        client.verbose = verbose;
		int success = client.get_cluster_info();
		 if(!success)
		{
			cout<<"Unable to get the cluster info "<<endl;
			exit(0);
		}
}

val_t GTStoreClient::get(string key) {

        if(verbo) cout << "Inside GTStoreClient::get() for client: " << client_id << " key: " << key << "\n";
		val_t value;
        client.get_val(key,value);

        string print_value = "";
		for (uint i = 0; i < value.size(); i++) {
				print_value += value[i] + " ";
		}
		if(verbo) cout << " key: " << key << " value: " << print_value << "\n";
		
		return value;
}

bool GTStoreClient::put(string key, val_t value) {

		string print_value = "";
		for (uint i = 0; i < value.size(); i++) {
				print_value += value[i] + " ";
		}
		if(verbo) cout << "Inside GTStoreClient::put() for client: " << client_id << " key: " << key << " value: " << print_value << "\n";
		// Put the value!
        client.store_val(key, value);
		return true;
}
unordered_map<string,shared_ptr<KeyValueStorage::Stub>> GTStoreClient::getStubInfo() {
    return client.stub_info;
}

void GTStoreClient::finalize() {

		if(verbo) cout << "Inside GTStoreClient::finalize() for client " << client_id << "\n";
}

