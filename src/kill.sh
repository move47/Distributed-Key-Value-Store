#!/bin/bash




if [ $# -ne 1 ]; then

    echo "Usage: $0 port_number"

    exit 1

fi


port=1000$1

pid=$(lsof -i:$port | awk '/storage/ {print $2}')




if [ -z "$pid" ]; then

    echo "No process is listening on port $port"

    exit 1

fi




echo "Killing process with PID $pid"

kill $pid
