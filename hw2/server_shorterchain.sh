#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=1

echo "***"
echo "Creates error on certificate length"
echo " "


openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_server.com.key.pem -pass pass:pass -verify $DEPTH -verify_depth $DEPTH -CAfile $HOMEROOT/certs/ca.cert.pem -www -HTTP -verify_return_error >> $HOME/serverlog.txt 2>&1 
