#/bin/bash

# Starting the manager service
gnome-terminal --window --command="bash -c './manager -n 3 -k 2;exec bash'"
sleep 5

# Calling the client
echo -e "\n ./test_client --put key1 --val value1"
./test_client --put key1 --val value1
sleep 2

echo -e "\n ./test_client --put key1 --val newvalue1"
./test_client --put key1 --val newvalue1
sleep 2


echo -e "\n ./test_client --put key2 --val value2"
./test_client --put key2 --val value2
sleep 2

echo -e "\n ./test_client --put key3 --val value3"
./test_client --put key3 --val value3
sleep 2

echo -e "\n ./test_client --put key4 --val value4"
./test_client --put key4 --val value4
sleep 2

echo -e "\n ./test_client --put key5 --val value5"
./test_client --put key5 --val value5
sleep 2

echo -e "\n ./test_client --put key6 --val value6"
./test_client --put key6 --val value6
sleep 2

echo -e "\n ./test_client --put key7 --val value7"
./test_client --put key7 --val value7
sleep 2

echo -e "\n Now since key1, key4, key7 have same primary node 1, let us kill it... "
# echo -e "\n ./test_client --get key1"
# ./test_client --get key1
# sleep 2

# echo -e "\n ./test_client --get key2"
# ./test_client --get key2
# sleep 2

# echo -e "\n ./test_client --get key3"
# ./test_client --get key3
# sleep 2




#gnome-terminal --window --command="bash -c 'echo \"Command 1\"; echo \"Command 2\"; echo \"Command 3\"; exec bash'"

