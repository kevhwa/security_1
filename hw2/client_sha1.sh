#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Testing client certificate generated using sha1"
echo " "

printf 'GET /file.txt HTTP/1.0\r\n' > $HOME/temp1.txt

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client_sha1.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain_sha1.cert.pem -key $HOMEINTER/private/www.example_client_sha1.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet -ign_eof < temp1.txt >> $HOME/clientlog.txt 2>&1

