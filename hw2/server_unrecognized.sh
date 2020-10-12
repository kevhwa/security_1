#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Server signed by another CA"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server_unrecognized.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain_unrecognized.cert.pem -key $HOMEINTER/private/www.example_server_unrecognized.com.key.pem -pass pass:pass -Verify $DEPTH -CAfile $HOMEROOT/certs/ca_unrecognized.cert.pem -www -HTTP -verify_return_error 
