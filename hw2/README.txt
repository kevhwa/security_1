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
(client window), please run client_defaultTest. This is a bash script that
will test a certificates. You can expect to see 
   - 3 successful file transfers
   - 
