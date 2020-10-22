/*
1. get email
- read in with fgets. This will get us sender and receiver until you hit DATA
- Verify that sender is in the mailbox
       - if not send error message but continue

2. fork, create a pipe from OS and call mail-out and pipe in the corresponding data

3. printf sender message, then printf all the receiver messages
4. fgets and check until you see just a .
5. kill child process somehow
6. loop back to 3


Q
- how does mail out check who the recpient is? Does it read the email?
- can we assume input files will be properly formatted? like each message will end with a period, no typos etc
*/

int main(int argc, char **argv) {


        //while(fgets?) 
        // read in from standard in, this will give us the first line of MAIL FROM
        // verify that this user exists. If not, print to std err and loop until you hit the "."

        //fork or read in entire file, create a file then just read the whole thing to child in one batch

        //kill child process
        //loop back





}
