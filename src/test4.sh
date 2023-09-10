#/bin/bash

# Starting the manager service
gnome-terminal --window --command="bash -c './manager -n 7 -k 3;exec bash'"
sleep 5

# Calling the client
echo -e "\n ./test_client --put key1 --val value1"
./test_client --put key1 --val value1
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

echo -e "\n ./test_client --put key8 --val value8"
./test_client --put key8 --val value8
sleep 2

echo -e "\n ./test_client --put key9 --val value9"
./test_client --put key9 --val value9
sleep 2

echo -e "\n ./test_client --put key10 --val value10"
./test_client --put key10 --val value10
sleep 2

echo -e "\n ./test_client --put key11 --val value11"
./test_client --put key11 --val value11
sleep 2

echo -e "\n ./test_client --put key12 --val value12"
./test_client --put key12 --val value12
sleep 2

echo -e "\n ./test_client --put key13 --val value13"
./test_client --put key13 --val value13
sleep 2

echo -e "\n ./test_client --put key14 --val value14"
./test_client --put key14 --val value14
sleep 2

echo -e "\n ./test_client --put key15 --val value15"
./test_client --put key15 --val value15
sleep 2

echo -e "\n ./test_client --put key16 --val value16"
./test_client --put key16 --val value16
sleep 2

echo -e "\n ./test_client --put key17 --val value17"
./test_client --put key17 --val value17
sleep 2

echo -e "\n ./test_client --put key18 --val value18"
./test_client --put key18 --val value18
sleep 2

echo -e "\n ./test_client --put key19 --val value19"
./test_client --put key19 --val value19
sleep 2

echo -e "\n ./test_client --put key20 --val value20"
./test_client --put key20 --val value20
sleep 2


echo -e "\n Overwriting key5, key10, key15"
echo -e "\n ./test_client --put key5 --val newvalue5"
./test_client --put key5 --val newvalue5
sleep 2

echo -e "\n ./test_client --put key10 --val newvalue10"
./test_client --put key10 --val newvalue10
sleep 2

echo -e "\n ./test_client --put key15 --val newvalue15"
./test_client --put key15 --val newvalue15
sleep 2

echo -e "\n let us kill it two nodes "
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

