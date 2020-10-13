#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Server that cant auth client"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server_noauth.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_server_noauth.com.key.pem -pass pass:pass -Verify $DEPTH -CAfile $HOMEROOT/certs/ca.cert.pem -www -HTTP -verify_return_error >> $HOME/serverlog.txt 2>&1 
