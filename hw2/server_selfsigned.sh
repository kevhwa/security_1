#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Self signed server"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server_selfsigned.com.cert.pem -key $HOMEINTER/private/www.example_server_selfsigned.com.key.pem -pass pass:pass -Verify $DEPTH -CAfile $HOMEROOT/certs/ca.cert.pem -www -HTTP -verify_return_error 
