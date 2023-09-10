#/bin/bash

make clean
make

# Launch the GTStore Manager
./bin/manager &
sleep 5

# Launch couple GTStore Storage Nodes
./bin/storage &
sleep 5
./bin/storage &
sleep 5

# Launch the client testing app
# Usage: ./test_app <test> <client_id>
./bin/test_app single_set_get 1 &
./bin/test_app single_set_get 2 &
./bin/test_app single_set_get 3 

