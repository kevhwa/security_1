UNI: jch2169
HW: hw2

Files supplied:
- 

Info:
- I use HTTP/1.0 so the client terminates the connection after each GET
  request is satisfied. This makes it easer/simpler to make a bash script
  that processes multiple clients.

- Below, I focus on the verify error nums that appear. There is also a
  paired ssl alert that also pops up on the other side (ie if verify error
  appears on the serverlog, a corresponding ssl alert error shows up on
  clientlog). Since I'm not sure which error is the focus, I list the verify
  errors below. But all errors can be found by opening the serverlog.txt or
  clientlog.txt

Directions:

0. Since this is a bash script that relies on absolute paths, please unpack
this file inside the following directory

/home/mushu/security_1/hw2

1. There is a script makeExec.sh. If the scripts are not executable when you
unpack them, please run chmod +x on this file and run it. This will then make all the other bash scripts executable so you only have to do the chmod command once.

2. run make.sh (./make.sh). This will make all the certificates along with a clientlog.txt and serverlog.txt that will hold the outputs (including the transferred
file content and error messages).

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

   - cafalse: error num 19 - self signed certificate 
     - client certificate created by intermediate CA whose CA field was set
       to false. Here, since the default server does not trust the root CA,
       it puts this error. In the next test, we will see a different error
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

   - cafalse: error num 24 - invalid CA certificate
     - client certificate created by intermediate CA whose CA field was set
       to false. Since this server trusts all root CAs, it was able to
       verify the certificate but it since the ca field was set to false, it
       gives this error
     - serverlog

   - nocertsign: error num 20 - unable to get local issuer certificate
     - similar to above, I tried to remove the keyCertSign option from the
       key usage to see what error it would give me
     - serverlog


5. On server window, please run server_eduemail.sh. On client window, please
run client_subtreeViolationTest.sh
  - Here, I added a nameconstraint field to the config file so that it
    excludes a particular email address -- in this case email addresses
    ending in .com

  - eduemail: no error - works fine
    - this certificate was created with an email address ending in .edu.
      This should work and be able to get the file

  - comemail: error num 48 - excluded subtree violation
    - since this certificate was created with a .com email, the server
      should reject it
    - clientlog

6. On server window, please run server_shorterchain.sh. On client window,
please run client_longerchain.sh 
  - Here, I removed the pathlen constraing from the config file so that
    intermediate CAs can sign other intermediate CSRs. The server then
    has a verify_depth option that checks that the depth of the certificate
    chain cannot be greater than 1. Since the client certificate was signed
    by a second intermediate CA, its depth is 2 and is rejected

  - longerchain: error num 22 - certificate chain too long
    - serverlog


7. On server window, please run server_acceptall.sh. On client window,
please run client_trialTest.sh.
  - Here, are a group of clients using certificates that aren't necessarily
    working in the way that they are supposed to be. These are tests that I
    will leave here to show what I tried to do. 

  - badident: Currently works (able to connect with client)
    - Here, I removed the subjectKeyIdentifier field from the usr_cert
      config file. I wanted to get a subject mismatch error but it seems
      this works.

  - conflictingexpdate: Currently giving same error as expired date
    - What I did here was create an expired certificate (one that lasts for
      1 minute or so) but also specify that it lasts 100 days so that there
      is a conflict. I was hoping this leads to some bug but it just gives
      the same error as the expired certificate

  - mismatch: currently works
    - What I did here was give differing country, state, organization names
      to a client certificate in hopes that since they are different from
      the root, it might be rejected. 

  - rejectemail: not working properly, asking for a trusted certificate?
    - What I did here was I tried to create a CA that rejects certificates
      that have the emailProtection keyUsage. There is an x509 command
      called -addreject [emailProtection] that is supposed to add this
      functionality to a Root CA but it is currently not working for me. 

  - der: currently works
    - What i did here was I used the x509 command to change the certificate
      format from PEM to DER. And in the server, specified -certform PEM
      in hopes that it would reject the client and produce an error.
      However, the client still is able to connect and retrieve the file


  - Some other things I've tried here that failed in s_client
  - mismatchkey: tried to provide a different key to the cert
  - broken: tried to edit a bit of the cert 


8. You may notice there are some servers that are not being used. It's
because they produce the same errors but on the server side. Since these
won't be counted as different errors, I stopped replicating the errors on
the server side but I still left the few I made in case you would like to
test them.
