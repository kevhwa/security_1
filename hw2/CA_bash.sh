#~/bin/bash

HOME=/home/mushu/security_1/hw2
#
# Creating Root CA
#

mkdir $HOME/ca
HOMEROOT=$HOME/ca

#
### Creating CA
#
mkdir $HOMEROOT/certs $HOMEROOT/crl $HOMEROOT/newcerts $HOMEROOT/private
chmod 700 $HOMEROOT/private
touch $HOMEROOT/index.txt
echo 1000 > $HOMEROOT/serial

openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca.key.pem 4096

chmod 400 $HOMEROOT/private/ca.key.pem
cp $HOME/root_openssl.cnf $HOMEROOT/root_openssl.cnf
cp $HOME/root_openssl_unrecognized.cnf $HOMEROOT/root_openssl_unrecognized.cnf

openssl req -config $HOMEROOT/root_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA' -passin pass:pass -key $HOMEROOT/private/ca.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca.cert.pem 

chmod 444 $HOMEROOT/certs/ca.cert.pem

#
### Creating Unrecognized CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_unrecognized.key.pem 4096

chmod 400 $HOMEROOT/private/ca_unrecognized.key.pem

openssl req -config $HOMEROOT/root_openssl_unrecognized.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_unrecog' -passin pass:pass -key $HOMEROOT/private/ca_unrecognized.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_unrecognized.cert.pem 

chmod 444 $HOMEROOT/certs/ca_unrecognized.cert.pem

#
# Creating Intermediary CA
#

mkdir $HOME/ca/intermediate
HOMEINTER=$HOMEROOT/intermediate

mkdir $HOMEINTER/certs $HOMEINTER/crl $HOMEINTER/csr $HOMEINTER/newcerts $HOMEINTER/private
chmod 700 $HOMEINTER/private
touch $HOMEINTER/index.txt
echo 1000 > $HOMEINTER/serial

cp $HOME/inter_openssl.cnf $HOMEINTER/inter_openssl.cnf
cp $HOME/inter_openssl_longerchain.cnf $HOMEINTER/inter_openssl_longerchain.cnf
cp $HOME/inter_openssl_unrecognized.cnf $HOMEINTER/inter_openssl_unrecognized.cnf

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate.key.pem

chmod 400 $HOMEINTER/private/intermediate.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate.key.pem -out $HOMEINTER/csr/intermediate.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate.csr.pem -out $HOMEINTER/certs/intermediate.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate.cert.pem

# Verify intermediate certificate
#echo ""
#echo "Verifying inter ca"
#openssl verify -CAfile $HOMEROOT/certs/ca.cert.pem $HOMEINTER/certs/intermediate.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate.cert.pem $HOMEROOT/certs/ca.cert.pem > $HOMEINTER/certs/ca-chain.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain.cert.pem

#
## Creating Intermediary > Intermediary CA
# 

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_longerchain.key.pem

chmod 400 $HOMEINTER/private/intermediate_longerchain.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_longerchain.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_longer' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_longerchain.key.pem -out $HOMEINTER/csr/intermediate_longerchain.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_longerchain.csr.pem -out $HOMEINTER/certs/intermediate_longerchain.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediat_longerchain2.cert.pem

#openssl x509 -noout -text -in $HOMEINTER/certs/intermediate_longerchain.cert.pem
# Verify intermediate certificate
#echo ""
#echo "Verifying inter ca2"
#openssl verify -CAfile $HOMEINTER/certs/ca-chain.cert.pem $HOMEINTER/certs/intermediate_longerchain.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_longerchain.cert.pem $HOMEINTER/certs/ca-chain.cert.pem > $HOMEINTER/certs/ca-chain2.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain2.cert.pem

#
# Creating Unrecognized Intermediary CA
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_unrecognized.key.pem

chmod 400 $HOMEINTER/private/intermediate_unrecognized.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_unrecognized.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_unrecog' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_unrecognized.key.pem -out $HOMEINTER/csr/intermediate_unrecognized.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_unrecognized.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_unrecognized.csr.pem -out $HOMEINTER/certs/intermediate_unrecognized.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_unrecognized.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_unrecognized.cert.pem $HOMEROOT/certs/ca_unrecognized.cert.pem > $HOMEINTER/certs/ca-chain_unrecognized.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_unrecognized.cert.pem

#
# Creating a server certificate
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserver.com' -passin pass:pass -key $HOMEINTER/private/www.example_server.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server.com.csr.pem -out $HOMEINTER/certs/www.example_server.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server.com.cert.pem

#
# Creating a server certificate (EXPIRED)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server2.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server2.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserver2.com' -passin pass:pass -key $HOMEINTER/private/www.example_server2.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server2.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -startdate 201010231800Z -enddate 201010231800Z -extensions server_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_server2.com.csr.pem -out $HOMEINTER/certs/www.example_server2.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server2.com.cert.pem

#
# Creating a server certificate (Wrong root ca cert)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_unrecognized.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_unrecognized.com.key.pem

# Create a CSR for the server
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_unrecognized.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserverunrecog.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_unrecognized.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_unrecognized.com.csr.pem

# Intermediate CA signs client CSR
openssl ca -config $HOMEINTER/inter_openssl_unrecognized.cnf -passin pass:pass -days 100 -extensions server_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_server_unrecognized.com.csr.pem -out $HOMEINTER/certs/www.example_server_unrecognized.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_unrecognized.com.cert.pem

#
# Creating a self-signed client certificate 
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_selfsigned.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_selfsigned.com.key.pem

# Create a CSR for client
openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserverselfsigned.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_selfsigned.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_selfsigned.com.csr.pem

# Self sign certificate
openssl x509 -req -in $HOMEINTER/csr/www.example_server_selfsigned.com.csr.pem -extfile $HOMEROOT/root_openssl.cnf -extensions v3_ca -signkey $HOMEINTER/private/www.example_server_selfsigned.com.key.pem -out $HOMEINTER/certs/www.example_server_selfsigned.com.cert.pem -passin pass:pass

chmod 444 $HOMEINTER/certs/www.example_server_selfsigned.com.cert.pem

#
# Creating a client certificate
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclient.com' -passin pass:pass -key $HOMEINTER/private/www.example_client.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client.com.csr.pem -out $HOMEINTER/certs/www.example_client.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client.com.cert.pem

echo " "
echo "Verifying chain of trust server certificate"
openssl verify -CAfile $HOMEINTER/certs/ca-chain.cert.pem $HOMEINTER/certs/www.example_server.com.cert.pem

echo " "
echo "Verifying chain of trust client certificate"
openssl verify -CAfile $HOMEINTER/certs/ca-chain.cert.pem $HOMEINTER/certs/www.example_client.com.cert.pem

#
# Creating a client certificate (EXPIRED)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_expired.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_expired.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientexpired.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_expired.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_expired.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -startdate 201010231800Z -enddate 201010231800Z -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client_expired.com.csr.pem -out $HOMEINTER/certs/www.example_client_expired.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_expired.com.cert.pem

#
# Creating a client certificate (EXPIRED CONFLICTING)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client3.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client3.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclient3.com' -passin pass:pass -key $HOMEINTER/private/www.example_client3.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client3.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -startdate 201010231800Z -enddate 201010231800Z -days 100 -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client3.com.csr.pem -out $HOMEINTER/certs/www.example_client3.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client3.com.cert.pem


#
# Creating a client certificate (Wrong root ca cert)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client4.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client4.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclient4.com' -passin pass:pass -key $HOMEINTER/private/www.example_client4.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client4.com.csr.pem

# Intermediate CA signs client CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -days 100 -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client4.com.csr.pem -out $HOMEINTER/certs/www.example_client4.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client4.com.cert.pem

#
# Creating a self-signed client certificate 
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_selfsigned.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_selfsigned.com.key.pem

# Create a CSR for client
openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclient_selfsigned.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_selfsigned.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_selfsigned.com.csr.pem

# Self sign certificate
openssl x509 -req -in $HOMEINTER/csr/www.example_client_selfsigned.com.csr.pem -extfile $HOMEROOT/root_openssl.cnf -extensions v3_ca -signkey $HOMEINTER/private/www.example_client_selfsigned.com.key.pem -out $HOMEINTER/certs/www.example_client_selfsigned.com.cert.pem -passin pass:pass

chmod 444 $HOMEINTER/certs/www.example_client_selfsigned.com.cert.pem

#
# Creating a client certificate (Chain intermediate CA)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_longerchain.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_longerchain.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_longerchain.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclient_longerchain.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_longerchain.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_longerchain.com.csr.pem

# Intermediate CA signs client CSR

openssl ca -config $HOMEINTER/inter_openssl_longerchain.cnf -passin pass:pass -days 100 -extensions usr_cert -md sha256 -in $HOMEINTER/csr/www.example_client_longerchain.com.csr.pem -out $HOMEINTER/certs/www.example_client_longerchain.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_longerchain.com.cert.pem

#echo "Verifying chain of trust client certificate"
#openssl verify -CAfile $HOMEINTER/certs/ca-chain2.cert.pem $HOMEINTER/certs/www.example_client8.com.cert.pem

#openssl x509 -noout -text -in $HOMEINTER/certs/www.example_client8.com.cert.pem

