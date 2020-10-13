#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Default placebo server"
echo " "

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_server.com.key.pem -pass pass:pass -Verify $DEPTH -CApath $HOMEROOT/certs -CAfile $HOMEINTER/certs/ca-chain_nocertsign.cert.pem -www -HTTP -verify_return_error >> $HOME/serverlog.txt 2>&1 
