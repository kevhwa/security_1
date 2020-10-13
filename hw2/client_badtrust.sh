#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Testing client not trusting ca"
echo " "

printf 'GET /file.txt HTTP/1.0\r\n' > $HOME/exampleRequest.txt

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_client.com.key.pem -pass pass:pass -CApath $HOMEINTER/certs -CAfile $HOMEINTER/certs/intermediate.cert.pem -verify $DEPTH -verify_return_error -prexit -quiet -ign_eof < exampleRequest.txt >> $HOME/clientlog.txt 2>&1

