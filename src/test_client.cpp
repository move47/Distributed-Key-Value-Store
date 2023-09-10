#include<bits/stdc++.h>
#include <openssl/sha.h>
#include <grpcpp/grpcpp.h>
#include "kvstorage.grpc.pb.h"
#include <sys/time.h>

#include "gtstore.hpp"

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
using keyvaluestorage::NodeSize;


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

int isGet=0;
int isPut=0;
int isBulk = 0;
int num_op;
string KEY;
string VALUE;

int verbosee = 0;

void parse_args(int argc, char* argv[])
{
        int inx=1;
        if(!strcmp(argv[inx], "--put"))
        {
            verbosee = 1;
            isPut=1;
            if(argc<5)
            {
                cout<<"Please provide valid arguments ./client (--get key)(--put key --val val)"<<endl;
                exit(1);
            }
            inx++;
            KEY=argv[inx];
            inx++;

            if(!strcmp(argv[inx], "--val"))
            {
                inx++;
                VALUE = argv[inx];
            }
            else
            {
                cout<<"Please provide valid arguments ./client (--get key)(--put key --val val)"<<endl;
                exit(1);
            }
        }
        else if(!strcmp(argv[inx], "--get"))
        {
            verbosee = 1;
            isGet=1;
            if(argc<3)
            {
                cout<<"Please provide valid arguments ./client (--get key)(--put key --val val)"<<endl;
                exit(1);
            }
            inx++;
            KEY=argv[inx];
        }
        else if(!strcmp(argv[inx], "--bulk"))
        {
            isBulk = 1;
            inx++;
            num_op = atoi(argv[inx]);
        }else{
            cout<<"Please provide valid arguments ./client (--bulk num_op)(--get key)(--put key --val val)"<<endl;
            exit(1);
        }
}

int main(int argc, char* argv[])
{
    // check whether it is get or put request
    parse_args(argc,argv);
    
    GTStoreClient client;
	client.init(0, verbosee);
    int success;
    if(isBulk>0){
        cout<<"bulk mode, verbose turned off"<<endl;
        int key = 1;
        struct timeval start_time;
        struct timeval end_time;
        struct timeval tot_exec_time;
        gettimeofday(&start_time , NULL);
        while(key<num_op){
            vector<string> valreq;
            valreq.push_back(to_string(key+1));
            valreq.push_back(to_string(key+2));
            success = client.put(to_string(key), valreq);
            if(!success)
            {
                cout<< "PUT request failed"<<endl;
            }
            vector<string> valres = client.get(to_string(key));
            key++;
        }
        gettimeofday(&end_time , NULL);
        timersub(&end_time, &start_time, &tot_exec_time);
        long tot_exec_time_in_micro = tot_exec_time.tv_sec*1000000 + tot_exec_time.tv_usec;
        long tot_exec_time_in_ms = tot_exec_time_in_micro/1000;
        printf("Total execution time(in ms): %ld in sec: %ld\n", tot_exec_time_in_ms, tot_exec_time.tv_sec);
        printf("Throughput: %f req/ms\n",(key*2)/(double)tot_exec_time_in_ms);
        if(tot_exec_time.tv_sec>0)
         printf("Throughput: %f req/s\n",(key*2)/(double)tot_exec_time.tv_sec);

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
    }else if(isGet){
        
        vector<string>val = client.get(KEY);
        
    }else if(isPut){
        vector<string> val;
        val.push_back(VALUE);
        success = client.put(KEY, val);
        if(!success)
        {
            cout<< "PUT request failed"<<endl;
        }
    }
}