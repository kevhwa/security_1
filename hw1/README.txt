uni:jch2169
hw: 1


Uses
- Archive files must be created using cstore program. If you supply an empty
  file to serve as the archive, program will treat it is a valid archive
  file just in case it may have been maliciously deleted or tampered with.


Restrictions
- In order to best control edge cases and possible security flaws, multiple
  restrictions have been built in for user input

Security
- For encryption, I utilize my implementation of AES CBC
- For integrity protection, I utilize my implementation of HMAC. I followed
  the protocol shown in ietf RFC2104. 

- This program offers triple security
  - After every add, extract and delete function, we update the HMAC of that
    file stored in archive along with the entire archive
  - Before every extract and delete, we verify that the archive HMAC is
    valid and that every individual file's HMAC is valid
  - Also stored in every file's metadata is a phrase. This decrypted phrase
    is always checked as another layer of security. Although the phrase is
    constant, the encryption would make it random even if the same phrase is
    encrypted over and over again


Add behavior
- Simply continues to append files to the end of the archive, granted the
  password is correct
- Archive file is able to have multiple instances of the same file


Extract behavior
- Does not delete a file that has been extracted. When you extract files
  from, lets say a zip file, that extracted file isn't deleted within the
  zip file. 
  - Thus, if the same file is added into the archive, only the first
    instance of that file will be extracted
- Extracts to a <filename>-decrypted filename. Will overwrite if a file
  with this name exists. The decrypted is added in case a file with the same
  file name but different content happens to be in the same directory.


Delete behavior
- Does lazy delete. Marks an instance of a stored file with a deleted
  marker. When user goes to try an extract a file, if that file has been
  marked as having been deleted, the search will skip that file and continue
  on. This makes it so if 2 files of the same name have been added, and
  delete on that file has been called once, the 2nd add of that file can
  still be extracted
- Delete will only mark the first instance of a non-deleted file in the
  archive as having been deleted


List behavior
- When a new archive is created, a new list file is created which stores the
  file name in plaintext. This implementation was better suited for my
  current working code. 
  - Following question 29 on piazza, the professor said that the list
    function has no integrity check. So if you corrupted the archive and you
    did the list function, there would be no way integrity check so no way
    of knowing if the archive has been tampered with and fed in wrong file
    names. However, when you call add/extract/delete that have integrity
    checks, these will catch changes.
  - My implementation is similar in that the list file has no integrity
    check and can be tampered with. However, the archive itself has
    integrity checks and so if the archive files were tampered with, it
    would know. What damage is done to the list file is limited to the list
    file. The important data, including the names of the files in the
    archive, are encrypted and integrity protected.
