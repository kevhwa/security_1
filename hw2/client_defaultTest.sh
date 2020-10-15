#~/bin/bash

HOME=/home/mushu/security_1/hw2

echo "Testing client tests"

./client_default.sh
./client_default2.sh
./client_default3.sh
./client_expired.sh
./client_nocert.sh
./client_selfsigned.sh
./client_tooearly.sh
./client_badtrust.sh
./client_wrongemail.sh
./client_wronghostname.sh
./client_wrongipaddr.sh
./client_authLevel.sh
./client_noauth.sh

