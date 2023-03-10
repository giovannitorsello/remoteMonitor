HOST=$1
PORT=$2
CMD=$3

echo "Sending $CMD to $HOST $PORT"
echo -n $CMD | nc -4u -w1 $HOST $PORT