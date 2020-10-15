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
cp $HOME/root_openssl_comemail.cnf $HOMEROOT/root_openssl_comemail.cnf
cp $HOME/root_openssl_digsig.cnf $HOMEROOT/root_openssl_digsig.cnf
cp $HOME/root_openssl_cafalse.cnf $HOMEROOT/root_openssl_cafalse.cnf
cp $HOME/root_openssl_nocertsign.cnf $HOMEROOT/root_openssl_nocertsign.cnf
cp $HOME/root_openssl_pathlen.cnf $HOMEROOT/root_openssl_pathlen.cnf
cp $HOME/root_openssl_sha2.cnf $HOMEROOT/root_openssl_sha2.cnf

openssl req -config $HOMEROOT/root_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA' -passin pass:pass -key $HOMEROOT/private/ca.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca.cert.pem 

chmod 444 $HOMEROOT/certs/ca.cert.pem

#
### Creating Unrecognized ROOT CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_unrecognized.key.pem 4096

chmod 400 $HOMEROOT/private/ca_unrecognized.key.pem

openssl req -config $HOMEROOT/root_openssl_unrecognized.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_unrecog' -passin pass:pass -key $HOMEROOT/private/ca_unrecognized.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_unrecognized.cert.pem 

chmod 444 $HOMEROOT/certs/ca_unrecognized.cert.pem

#
### Creating ROOT CA that rejects .com
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_comemail.key.pem 4096

chmod 400 $HOMEROOT/private/ca_comemail.key.pem

openssl req -config $HOMEROOT/root_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_comemail' -passin pass:pass -key $HOMEROOT/private/ca_comemail.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_comemail.cert.pem 

chmod 444 $HOMEROOT/certs/ca_comemail.cert.pem

#
### Creating Unrecognized ROOT CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_digsig.key.pem 4096

chmod 400 $HOMEROOT/private/ca_digsig.key.pem

openssl req -config $HOMEROOT/root_openssl_digsig.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_digsig' -passin pass:pass -key $HOMEROOT/private/ca_digsig.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_digsig.cert.pem 

chmod 444 $HOMEROOT/certs/ca_digsig.cert.pem

#
### Creating ROOT CA for ca false intermediate CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_cafalse.key.pem 4096

chmod 400 $HOMEROOT/private/ca_cafalse.key.pem

openssl req -config $HOMEROOT/root_openssl_cafalse.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_cafalse' -passin pass:pass -key $HOMEROOT/private/ca_cafalse.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_cafalse.cert.pem 

chmod 444 $HOMEROOT/certs/ca_cafalse.cert.pem

#
### Creating ROOT CA for nocertsign intermediate CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_nocertsign.key.pem 4096

chmod 400 $HOMEROOT/private/ca_nocertsign.key.pem

openssl req -config $HOMEROOT/root_openssl_nocertsign.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCA_nocertsign' -passin pass:pass -key $HOMEROOT/private/ca_nocertsign.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_nocertsign.cert.pem 

chmod 444 $HOMEROOT/certs/ca_nocertsign.cert.pem

#
### Creating ROOT CA with pathlen 0 intermediate CA
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_pathlen.key.pem 4096

chmod 400 $HOMEROOT/private/ca_pathlen.key.pem

openssl req -config $HOMEROOT/root_openssl_pathlen.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCApathlen' -passin pass:pass -key $HOMEROOT/private/ca_pathlen.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca_pathlen.cert.pem 

chmod 444 $HOMEROOT/certs/ca_pathlen.cert.pem

#
### Creating ROOT CA with sha2 encryption
#
openssl genrsa -aes256 -passout pass:pass -out $HOMEROOT/private/ca_sha2.key.pem 4096

chmod 400 $HOMEROOT/private/ca_sha2.key.pem

openssl req -config $HOMEROOT/root_openssl_sha2.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=rootCAsha2' -passin pass:pass -key $HOMEROOT/private/ca_sha2.key.pem -new -x509 -days 7300 -extensions v3_ca -out $HOMEROOT/certs/ca_sha2.cert.pem 
#sha1
chmod 444 $HOMEROOT/certs/ca_sha2.cert.pem

# Create a file with all trusted root certificates
cat $HOMEROOT/certs/ca_cafalse.cert.pem $HOMEROOT/certs/ca_comemail.cert.pem $HOMEROOT/certs/ca_nocertsign.cert.pem $HOMEROOT/certs/ca.cert.pem $HOMEROOT/certs/ca_digsig.cert.pem $HOMEROOT/certs/ca_unrecognized.cert.pem $HOMEROOT/certs/ca_pathlen.cert.pem $HOMEROOT/certs/ca_sha2.cert.pem > $HOMEROOT/certs/ca_all.cert.pem

chmod 444 $HOMEINTER/certs/ca_all.cert.pem
############################################################################3#####
##############################################################################3#####
###########################################################################3#####
###########################################################################3#####

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
cp $HOME/inter_openssl_noauth.cnf $HOMEINTER/inter_openssl_noauth.cnf
cp $HOME/inter_openssl_comemail.cnf $HOMEINTER/inter_openssl_comemail.cnf
cp $HOME/inter_openssl_digsig.cnf $HOMEINTER/inter_openssl_digsig.cnf
cp $HOME/inter_openssl_cafalse.cnf $HOMEINTER/inter_openssl_cafalse.cnf
cp $HOME/inter_openssl_badident.cnf $HOMEINTER/inter_openssl_badident.cnf
cp $HOME/inter_openssl_nocertsign.cnf $HOMEINTER/inter_openssl_nocertsign.cnf
cp $HOME/inter_openssl_encryption.cnf $HOMEINTER/inter_openssl_encryption.cnf
cp $HOME/inter_openssl_signing.cnf $HOMEINTER/inter_openssl_signing.cnf
cp $HOME/inter_openssl_pathlen.cnf $HOMEINTER/inter_openssl_pathlen.cnf
cp $HOME/inter_openssl_badpathlen.cnf $HOMEINTER/inter_openssl_badpathlen.cnf
cp $HOME/inter_openssl_sha2.cnf $HOMEINTER/inter_openssl_sha2.cnf

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

chmod 444 $HOMEINTER/certs/intermediate_longerchain.cert.pem

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
# Creating Intermediary CA that rejects .com email
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_comemail.key.pem

chmod 400 $HOMEINTER/private/intermediate_comemail.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_comemail' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_comemail.key.pem -out $HOMEINTER/csr/intermediate_comemail.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_comemail.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_comemail.csr.pem -out $HOMEINTER/certs/intermediate_comemail.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_comemail.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_comemail.cert.pem $HOMEROOT/certs/ca_comemail.cert.pem > $HOMEINTER/certs/ca-chain_comemail.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_comemail.cert.pem

#
## Creating intermediate ca without digsig
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_digsig.key.pem

chmod 400 $HOMEINTER/private/intermediate_digsig.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_digsig.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCAdigsig' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_digsig.key.pem -out $HOMEINTER/csr/intermediate_digsig.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_digsig.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_digsig.csr.pem -out $HOMEINTER/certs/intermediate_digsig.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_digsig.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_digsig.cert.pem $HOMEROOT/certs/ca_digsig.cert.pem > $HOMEINTER/certs/ca-chain_digsig.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_digsig.cert.pem

#
# Creating Intermediary CA with cafalse
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_cafalse.key.pem

chmod 400 $HOMEINTER/private/intermediate_cafalse.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_cafalse.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_cafalse' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_cafalse.key.pem -out $HOMEINTER/csr/intermediate_cafalse.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_cafalse.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_cafalse.csr.pem -out $HOMEINTER/certs/intermediate_cafalse.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_cafalse.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_cafalse.cert.pem $HOMEROOT/certs/ca_cafalse.cert.pem > $HOMEINTER/certs/ca-chain_cafalse.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_cafalse.cert.pem

#
# Creating Intermediary CA with cafalse
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_nocertsign.key.pem

chmod 400 $HOMEINTER/private/intermediate_nocertsign.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_nocertsign.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_nocertsign' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_nocertsign.key.pem -out $HOMEINTER/csr/intermediate_nocertsign.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_nocertsign.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_nocertsign.csr.pem -out $HOMEINTER/certs/intermediate_nocertsign.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_nocertsign.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_nocertsign.cert.pem $HOMEROOT/certs/ca_nocertsign.cert.pem > $HOMEINTER/certs/ca-chain_nocertsign.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_nocertsign.cert.pem

#
# Creating Intermediary CA with pathlen 0
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_pathlen.key.pem

chmod 400 $HOMEINTER/private/intermediate_pathlen.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_pathlen.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_pathlen' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_pathlen.key.pem -out $HOMEINTER/csr/intermediate_pathlen.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_pathlen.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_pathlen.csr.pem -out $HOMEINTER/certs/intermediate_pathlen.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_pathlen.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_pathlen.cert.pem $HOMEROOT/certs/ca_pathlen.cert.pem > $HOMEINTER/certs/ca-chain_pathlen.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_pathlen.cert.pem

#
## Creating Intermediary > Intermediary CA with pathlen 0
# 

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_badpathlen.key.pem

chmod 400 $HOMEINTER/private/intermediate_badpathlen.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_badpathlen.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_pathlen1' -new -sha256 -passin pass:pass -key $HOMEINTER/private/intermediate_badpathlen.key.pem -out $HOMEINTER/csr/intermediate_badpathlen.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEINTER/inter_openssl_pathlen.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate_badpathlen.csr.pem -out $HOMEINTER/certs/intermediate_badpathlen.cert.pem -batch

chmod 444 $HOMEINTER/certs/intermediate_badpathlen.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_badpathlen.cert.pem $HOMEINTER/certs/ca-chain_pathlen.cert.pem > $HOMEINTER/certs/ca-chain_badpathlen.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_badpathlen.cert.pem

#
# Creating Intermediary CA with sha2
#

openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/intermediate_sha2.key.pem

chmod 400 $HOMEINTER/private/intermediate_sha2.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl_sha2.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=interCA_sha2' -new -passin pass:pass -key $HOMEINTER/private/intermediate_sha2.key.pem -out $HOMEINTER/csr/intermediate_sha2.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl_sha2.cnf -passin pass:pass -extensions v3_intermediate_ca -days 3650 -notext -in $HOMEINTER/csr/intermediate_sha2.csr.pem -out $HOMEINTER/certs/intermediate_sha2.cert.pem -batch
#md sha256
chmod 444 $HOMEINTER/certs/intermediate_sha2.cert.pem

# Create certificate chain file
cat $HOMEINTER/certs/intermediate_sha2.cert.pem $HOMEROOT/certs/ca_sha2.cert.pem > $HOMEINTER/certs/ca-chain_sha2.cert.pem

chmod 444 $HOMEINTER/certs/ca-chain_sha2.cert.pem

############################################################################3#####
##############################################################################3#####
###########################################################################3#####
###########################################################################3#####

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
# Creating a server certificate that cant auth and rejects .com
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_noauth.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_noauth.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_noauth.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleservernoauth.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_noauth.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_noauth.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_noauth.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_noauth.com.csr.pem -out $HOMEINTER/certs/www.example_server_noauth.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_noauth.com.cert.pem

#
# Creating a server certificate with .com email
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_comemail.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_comemail.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleservercomemail.com/emailAddress=example@example.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_comemail.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_comemail.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_comemail.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_comemail.com.csr.pem -out $HOMEINTER/certs/www.example_server_comemail.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_comemail.com.cert.pem

#
# Creating a server certificate with .edu email
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_eduemail.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_eduemail.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserveredu.com/emailAddress=example@example.edu' -passin pass:pass -key $HOMEINTER/private/www.example_server_eduemail.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_eduemail.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_comemail.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_eduemail.com.csr.pem -out $HOMEINTER/certs/www.example_server_eduemail.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_eduemail.com.cert.pem

#
# Creating a server certificate (no digsig)
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_digsig.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_digsig.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_digsig.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleserverdigsig.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_digsig.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_digsig.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_digsig.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_digsig.com.csr.pem -out $HOMEINTER/certs/www.example_server_digsig.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_digsig.com.cert.pem

#
# Creating a server certificate (ca false)
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_cafalse.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_cafalse.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_cafalse.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleservercafalse.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_cafalse.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_cafalse.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_cafalse.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_cafalse.com.csr.pem -out $HOMEINTER/certs/www.example_server_cafalse.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_cafalse.com.cert.pem

#
# Creating a server certificate (nocertsign)
#

# Create a key pair for the webserver
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_server_nocertsign.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_server_nocertsign.com.key.pem

# Create a CSR for the webserver
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_nocertsign.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleservernocertsign.com' -passin pass:pass -key $HOMEINTER/private/www.example_server_nocertsign.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_server_nocertsign.com.csr.pem

# Intermediate CA signs web server CSR

openssl ca -config $HOMEINTER/inter_openssl_nocertsign.cnf -passin pass:pass -extensions server_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_server_nocertsign.com.csr.pem -out $HOMEINTER/certs/www.example_server_nocertsign.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_server_nocertsign.com.cert.pem

############################################################################3#####
##############################################################################3#####
###########################################################################3#####
###########################################################################3#####

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

#echo " "
#echo "Verifying chain of trust server certificate"
#openssl verify -CAfile $HOMEINTER/certs/ca-chain.cert.pem $HOMEINTER/certs/www.example_server.com.cert.pem

#echo " "
#echo "Verifying chain of trust client certificate"
#openssl verify -CAfile $HOMEINTER/certs/ca-chain.cert.pem $HOMEINTER/certs/www.example_client.com.cert.pem

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

#
# Creating a client certificate that cannot auth and rejects .com
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_noauth.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_noauth.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_noauth.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientnoauth.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_noauth.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_noauth.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_noauth.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_noauth.com.csr.pem -out $HOMEINTER/certs/www.example_client_noauth.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_noauth.com.cert.pem

#
# Creating a client certificate with .com email
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_comemail.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_comemail.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientcomemail.com/emailAddress=example2@example.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_comemail.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_comemail.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_comemail.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_comemail.com.csr.pem -out $HOMEINTER/certs/www.example_client_comemail.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_comemail.com.cert.pem

#
# Creating a client certificate with .edu email for test
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_eduemail.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_eduemail.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_comemail.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclienteduemail.com/emailAddress=example2@example.edu' -passin pass:pass -key $HOMEINTER/private/www.example_client_eduemail.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_eduemail.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_comemail.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_eduemail.com.csr.pem -out $HOMEINTER/certs/www.example_client_eduemail.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_eduemail.com.cert.pem

#
# Creating a client certificate (no digsig)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_digsig.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_digsig.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_digsig.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientdigsig.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_digsig.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_digsig.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_digsig.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_digsig.com.csr.pem -out $HOMEINTER/certs/www.example_client_digsig.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_digsig.com.cert.pem
#
# Creating a client certificate (mismatch subject key identifier)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_badident.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_badident.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_badident.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientbadident.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_badident.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_badident.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_badident.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_badident.com.csr.pem -out $HOMEINTER/certs/www.example_client_badident.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_badident.com.cert.pem

#
# Creating a client certificate (To be created)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_tooearly.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_tooearly.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclienttooearly.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_tooearly.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_tooearly.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -startdate 251010231800Z -enddate 251010231800Z -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client_tooearly.com.csr.pem -out $HOMEINTER/certs/www.example_client_tooearly.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_tooearly.com.cert.pem

#
# Creating a client certificate (Wrong to be created time)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_wrongearly.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_wrongearly.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientwrongearly.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_wrongearly.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_wrongearly.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf -passin pass:pass -startdate 20150214120000Z -enddate 251010231800Z -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client_wrongearly.com.csr.pem -out $HOMEINTER/certs/www.example_client_wrongearly.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_wrongearly.com.cert.pem

#
# Creating a client certificate (ca false)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_cafalse.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_cafalse.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_cafalse.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientcafalse.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_cafalse.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_cafalse.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_cafalse.cnf -passin pass:pass -startdate 151214120000Z -enddate 251023180000Z -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client_cafalse.com.csr.pem -out $HOMEINTER/certs/www.example_client_cafalse.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_cafalse.com.cert.pem

#
# Creating a client certificate (no certsign)
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_nocertsign.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_nocertsign.com.key.pem

# Create a CSR for the client
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_nocertsign.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientnocertsign.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_nocertsign.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_nocertsign.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_nocertsign.cnf -passin pass:pass -startdate 151214120000Z -enddate 251023180000Z -extensions usr_cert -notext -md sha256 -in $HOMEINTER/csr/www.example_client_nocertsign.com.csr.pem -out $HOMEINTER/certs/www.example_client_nocertsign.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_nocertsign.com.cert.pem

#
# Creating a client certificate
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_mismatch.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_mismatch.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=SK/ST=Seoul/O=COMS4182 Hw0/CN=www.exampleclientmismatch.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_mismatch.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_mismatch.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_mismatch.com.csr.pem -out $HOMEINTER/certs/www.example_client_mismatch.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_mismatch.com.cert.pem

#
# Creating a client certificate for encryption only
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_encryption.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_encryption.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_encryption.cnf -subj '/C=SK/ST=Seoul/O=COMS4182 Hw0/CN=www.exampleclientencrytpion.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_encryption.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_encryption.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_encryption.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_encryption.com.csr.pem -out $HOMEINTER/certs/www.example_client_encryption.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_encryption.com.cert.pem

#
# Creating a client certificate for signing only
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_signing.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_signing.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_signing.cnf -subj '/C=SK/ST=Seoul/O=COMS4182 Hw0/CN=www.exampleclientsigning.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_signing.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_signing.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_signing.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_signing.com.csr.pem -out $HOMEINTER/certs/www.example_client_signing.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_signing.com.cert.pem

#
# Creating a client certificate for signing only
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_badpathlen.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_badpathlen.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_badpathlen.cnf -subj '/C=SK/ST=Seoul/O=COMS4182 Hw0/CN=www.exampleclientbadpathlen.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_badpathlen.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_badpathlen.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_badpathlen.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_badpathlen.com.csr.pem -out $HOMEINTER/certs/www.example_client_badpathlen.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_badpathlen.com.cert.pem

#
# Creating a client certificate with sha2
#

# Create a key pair for the client
openssl genrsa -aes256 -passout pass:pass -out $HOMEINTER/private/www.example_client_sha2.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_sha2.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl_sha2.cnf -subj '/C=SK/ST=Seoul/O=COMS4182 Hw0/CN=www.exampleclientsha2.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_sha2.com.key.pem -new -out $HOMEINTER/csr/www.example_client_sha2.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl_sha2.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -in $HOMEINTER/csr/www.example_client_sha2.com.csr.pem -out $HOMEINTER/certs/www.example_client_sha2.com.cert.pem -batch
#md
chmod 444 $HOMEINTER/certs/www.example_client_sha2.com.cert.pem

#
# Creating a client certificate
#

# Create a key pair for the client
openssl genrsa -des3 -passout pass:pass -out $HOMEINTER/private/www.example_client_sha1.com.key.pem 2048

chmod 400 $HOMEINTER/private/www.example_client_sha1.com.key.pem

# Create a CSR for client 1
# Common Name must be a fully qualified domain name

openssl req -config $HOMEINTER/inter_openssl.cnf -subj '/C=US/ST=New York/O=COMS4181 Hw2/CN=www.exampleclientsha1.com' -passin pass:pass -key $HOMEINTER/private/www.example_client_sha1.com.key.pem -new -sha256 -out $HOMEINTER/csr/www.example_client_sha1.com.csr.pem

# Intermediate CA signs client server CSR

openssl ca -config $HOMEINTER/inter_openssl.cnf  -passin pass:pass -extensions usr_cert -days 375 -notext -md sha256 -in $HOMEINTER/csr/www.example_client_sha1.com.csr.pem -out $HOMEINTER/certs/www.example_client_sha1.com.cert.pem -batch

chmod 444 $HOMEINTER/certs/www.example_client_sha1.com.cert.pem


