#~/bin/bash

HOME=/home/mushu/security_1/hw2

echo "Testing client tests"

./client_default.sh
./client_expired.sh
./client_nocert.sh
./client_selfsigned.sh

