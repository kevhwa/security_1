#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Server with no cert sign"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server_nocertsign.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain_nocertsign.cert.pem -key $HOMEINTER/private/www.example_server_nocertsign.com.key.pem -pass pass:pass -Verify $DEPTH -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca_nocertsign.cert.pem -www -HTTP -verify_return_error >> $HOME/serverlog.txt 2>&1 
