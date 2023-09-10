#/bin/bash

# Starting the manager service
gnome-terminal --window --command="bash -c './manager -n 5 -k 3;exec bash'"
sleep 5

# Calling the client
echo -e "\n ./test_client --put key1 --val value1"
./test_client --put key1 --val value1
sleep 2

echo -e "\n ./test_client --get key1"
./test_client --get key1
sleep 2

echo -e "\n ./test_client --put key1 --val value2"
./test_client --put key1 --val value2
sleep 2

echo -e "\n ./test_client --put key2 --val value3"
./test_client --put key2 --val value3
sleep 2

echo -e "\n ./test_client --put key3 --val value4"
./test_client --put key3 --val value4
sleep 2

echo -e "\n ./test_client --get key1"
./test_client --get key1
sleep 2

echo -e "\n ./test_client --get key2"
./test_client --get key2
sleep 2

echo -e "\n ./test_client --get key3"
./test_client --get key3
sleep 2




#gnome-terminal --window --command="bash -c 'echo \"Command 1\"; echo \"Command 2\"; echo \"Command 3\"; exec bash'"

