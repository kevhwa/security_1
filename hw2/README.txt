UNI: jch2169
HW: hw2

Files supplied:
- 

Info:
- I use HTTP/1.0 so the client terminates the connection after each GET
  request is satisfied. This makes it easer/simpler to make a bash script
  that processes multiple clients.


Directions:

1. There is 1 script [---]. Please run chmod +x on this file and run it. This
will then make all the other bash scripts executable so you only have to do
the chmod command once.

2. run make. This will make all the certificates along with a clientlog.txt
and serverlog.txt that will hold the outputs (including the transferred
files and error messages)

3. On one window (server window), please run default_server. On the other
(client window), please run client_defaultServerTest. This is a bash script that
will test multiple client certificates. The server is running a plain
vanilla certificate. You can expect to see the below
   - 3 successful file transfers to show that connect, accept and file
     transfers all work on "legitimate" certificates

   - expired client certificate: error num 10 - certificate has expired
     - client is using an expired certificate
     - serverlog

   - nocert: error - peer did not return a certificate
     - cient does not provide its certificate
     - serverlog

   - selfsigned: error num 18 - self signed certificate
     - client provides a certificate that is self signed
     - serverlog

   - tooearly: error num 9 - certificate is not yet valid
     - client provides a certificate that has a start date in the future
     - serverlog

   - badtrust: error num 2 - unable to get issuer certificate
     - server root CA is not one that client trusts
     - clientlog

   - wrongemail: error num 63 - email address mismatch
     - client tests that server has a specific email address as part of
       subjaltname 
     - clientlog

   - wronghostname: error num 62 - hostname mismatch
     - client tests that server has a specific DNS name as part of
       subjaltname
     - clientlog

   - wrongipaddr: error num 64 - ip address mismatch
     - client tests that server has a specific IP address as part of
       subjaltname 
     - clientlog

   - authLevel: error num 66 - EE certificate too weak 
     - client tests that server's encryption are up to a specific standard 
     - clientlog

   - noauth: error num 26 - unsupported certificate purpose
     - removed serverauth option from extended key usage
     - serverlog

   - crlcheck: error num 3 - unable to get certificate crl
     - added a client option to check for crl 
     - clientlog

   - encryption: error num 26 - unsupported certificate purpose
     - client certificate created with only encryption as key usage
     - serverlog



4. On server window, please run server_acceptall. On client window, please
run client_acceptallServerTest.sh
   - Here, I configured the server to trust all the root ca's created thus
     far so I can test certificates created by a variety of CAs. 
   - digsig: error num 26 - unsupported certificate purpose
     - client certificate was created without the digital signature key
       usage
     - serverlog
   
   - badpathlen: error num 25 - path length constraint exceeded
     - client certificate was created with a second intermediate CA when
       pathlen was set to 0 (the first intermediate CA)
     - serverlog

   - onecertchain: error num 20 - unable to get local issuer certificate
     - client sends an intermediate CA's certificate as the certchain 
     - serverlog

   - wrongcertchain: error num 20 - unable to get local issuer certificate
     - client sends another CA's cert chain as its own
     - serverlog


5. On server window, please run server_digsig. On client window, please run
client_digsig. 
  - 
  
  This produces unsupported certificate purpose error. What I
did here was I removed the digital signature field from server and user
keyUsage field. I also tried removing it from the ca and intermediate_ca
keyUsage field as well. They both result in the same error. 


