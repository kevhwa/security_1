~/bin/bash

HOME=/home/mushu/security_1/hw2

echo "Testing client tests on acceptall server"

./client_default.sh
./client_default2.sh
./client_default3.sh

./client_digsig.sh
./client_badpathlen.sh
./client_onecertchain.sh
./client_wrongcertchain.sh


