#ifndef GTSTORE
#define GTSTORE

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include<bits/stdc++.h>
#include <openssl/sha.h>
#include <grpcpp/grpcpp.h>
#include "kvstorage.grpc.pb.h"


#define MAX_KEY_BYTE_PER_REQUEST 20
#define MAX_VALUE_BYTE_PER_REQUEST 1000

using namespace std;
using keyvaluestorage::KeyValueStorage;



typedef vector<string> val_t;

class GTStoreClient {
		private:
				int client_id;
				val_t value;
		public:
				void init(int id, int verbose);
				void finalize();
				val_t get(string key);
				bool put(string key, val_t value);
				unordered_map<string,shared_ptr<KeyValueStorage::Stub>> getStubInfo();
};

class GTStoreManager {
		public:
				void init();
};

class GTStoreStorage {
		public:
				void init();
};


#endif
