#include "gtstore.hpp"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using keyvaluestorage::NodeSize;

void single_set_get(int client_id, int verbose) {
		cout << "Testing single set-get for GTStore by client " << client_id << ".\n";

		GTStoreClient client;
		client.init(client_id, verbose);

		string key = to_string(client_id);
		vector<string> value;
		value.push_back("phone");
		value.push_back("phone_case");

		client.put(key, value);
		client.get(key);

		client.finalize();
}

void single_set_get_multi_server(int client_id, int verbose) {
		cout << "Testing set-get multiple server for GTStore by client " << client_id << ".\n";
		cout<<"This test will insert 20 keys, update keys from range 7-9, and then will let the user kill any server.\n then will keys from 6-10"<<endl;

		GTStoreClient client;
		client.init(client_id, verbose);

		int success = 0;
		int num = 1;
		string key;
		while(num<20){
            vector<string> valreq;
            valreq.push_back(to_string(client_id)+"_"+to_string(num+1));
            valreq.push_back(to_string(client_id)+"_"+to_string(num+2));
			key = to_string(client_id)+"_"+to_string(num);
            success = client.put(key, valreq);
            if(!success)
            {
                cout<< "PUT request failed"<<endl;
            }
            // vector<string> valres = client.get(key);
            num++;
		}

		num = 7;
		while(num<=9){
			vector<string> valreq;
            valreq.push_back(to_string(client_id)+"_"+to_string(num+1)+"_"+to_string(num+1));
            valreq.push_back(to_string(client_id)+"_"+to_string(num+2)+"_"+to_string(num+2));
			key = to_string(client_id)+"_"+to_string(num);
            success = client.put(key, valreq);
            if(!success)
            {
                cout<< "PUT request failed"<<endl;
            }
            // vector<string> valres = client.get(key);
            num++;
		}
		cout<<"Kill any storage node if required."<<endl;

		int a;
		cin>>a;
		num = 6;
		while(num<10){
			vector<string> valres = client.get(key);
			num++;
		}


		unordered_map<string,shared_ptr<KeyValueStorage::Stub>> stub_info = client.getStubInfo();
        for (auto it = stub_info.begin(); it != stub_info.end(); ++it) {
            std::string nodeid = it->first;
            std::shared_ptr<KeyValueStorage::Stub>& stub_ = it->second;
            
            ClientContext context;
            google::protobuf::Empty request;
            NodeSize size_reply;

            Status status = stub_->size(&context, request, &size_reply);
            if(status.ok())
            {
                cout<<"Size of nodeId: "<<nodeid<<": "<<size_reply.size()<<endl;
            }
            else{
                cout<<"Failed to get size for nodeid: "<<nodeid<<endl;
            }
        }
		client.finalize();
}


int main(int argc, char **argv) {
		string test = string(argv[1]);
		int client_id = atoi(argv[2]);
		int verbose = 0;
		if(argc>2){
			verbose = atoi(argv[3]);
		}

		string test1 = "test1";
		string test2 = "test2";
		if(test == "help"){
			cout<<"test1 -> single_set_get"<<endl;
			cout<<"test2 -> set_get_multi_server"<<endl;

		}else if (test ==  test1) {
				single_set_get(client_id, verbose);
		}else if (test ==  test2) {
				single_set_get_multi_server(client_id, verbose);
		}
}

///./test_client --get key1
// now key1, key4 and key 7 are mapped to same primary node server 1...let us kill it
