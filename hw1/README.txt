

Restrictions
- In order to best control edge cases and possible security flaws, multiple
  restrictions have been built in for user input




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
