#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Server without digsig"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server_digsig.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain_digsig.cert.pem -key $HOMEINTER/private/www.example_server_digsig.com.key.pem -pass pass:pass -Verify $DEPTH -CAfile $HOMEROOT/certs/ca_digsig.cert.pem -www -HTTP -verify_return_error 
