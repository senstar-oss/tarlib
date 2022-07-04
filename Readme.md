by [Marius Bancila](https://codexpert.ro/blog/author/marius-bancila/)

[original blog post](https://codexpert.ro/blog/2012/09/17/tarlib-windows-tar-library/)


TAR description
===============

If you need tarlib, you must already know something about TAR files. Anyways you can get more info in the following articles:

*   [tar (file format)](http://en.wikipedia.org/wiki/Tar_(file_format))
*   [format of tar archives](http://www.mkssoftware.com/docs/man4/tar.4.asp)
*   [Basic Tar Format](http://www.gnu.org/software/tar/manual/html_node/Standard.html)

Here is a short summary of the TAR format:

*   TAR archives consist of a series of objects, most common being files and folders
*   each such object is preceded by a header (of 512 bytes)
*   the information in the header is encoded in ASCII and numbers are written in the octal base
*   the file data is written unaltered, but it is rounded up to a multiple of 512 bytes
*   the end of the file is marked with at least two consecutive entries filled with zeros
*   there are different version of the TAR archives (UNIX V7, “old GNU” and GNU, STAR and POSIX) and different implementation

Library
=======

tarlib is written in C++ with Visual Studio and requires minimum Windows XP (because of file system APIs that it uses and that were introduced with WinXP). The library is provided as a pack of C++ files (headers and cpps) that you can include in your application.

Note that:

*   the library is distributed under the [Creative Commons Attribution-ShareAlike](http://creativecommons.org/licenses/by-sa/3.0/) license
*   The software is provided “as-is”. No claim of suitability, guarantee, or any warranty whatsoever is provided.

The current version (v1.1)

*   is able to read (and process) existing TAR files
*   does not support creation of TAR files
*   supports parsing tar objects representing files and folders (as these are the most common objects on Windows at least)

Library API
===========

There are a few classes/structures the library provides for handling TAR files.

*   **tarFile**: is the representation of a tar file.
    *   `bool open(std::string const &filename, tarFileMode mode, tarFormatType type)`opens the specified TAR file for reading or writing (not supported in v1.1)
    *   `bool extract(std::string const &folder)`extracts the content of the archive (files and folders) to the specified destination
    *   `tarEntry get_first_entry()`retrieves the first entry in a tar archive
    *   `tarEntry get_next_entry()`retrieves the next entry in a tar archive
    *   `void rewind()`re-positions the file cursor at the beginning of the archive
*   **tarEntry**: represents an object in a TAR file. it contains the header for the entry and methods to process the entry:
    *   `bool is_empty()`indicates whether this is an empty entry (empty entries are used to mark the end of the archive)
    *   `bool is_md5()`indicates whether this is an entry that contains the MD5 hash of the actual TAR file (always found at the end of the archive)
    *   `void rewind()`re-positions the file cursor ar the beginning of the object’s data (so you can read it again)
    *   `bool extract(std::string const &folder)`extracts the current entry (file or folder) to the specified folder
    *   `size_t read(char* buffer, size_t chunksize = tarChunkSize)`reads from the current position in the object’s data to the provided buffer; this function does not read past the end of the object’s data
    *   `static tarEntry makeEmpty()`creates a tarEntry representing an empty object
    *   `static tarEntry makeMD5(char* buffer, size_t size)`creates a tarEntry from a buffer containing the MD5 hash for the TAR object

Examples
========

Example 1: extract a TAR archive to a specified folder using the tarFile
```
void extract1(std::string const &filename, std::string const &destination)
{
   // create tar file with path and read mode
   tarFile tarf(filename, tarModeRead);

   // extract to folder
   tarf.extract(destination);
}
```
Example 2: extract a TAR archive to a specified folder using a loop that iterates through the entries of the TAR archive
```
void extract2(std::string const &filename, std::string const &destination)
{
   // create tar file with path and read mode
   tarFile tarf(filename, tarModeRead);

   // get the first entry
   tarEntry entry = tarf.get_first_entry();
   do 
   {
      // if the entry is a directory create the directory
      if(entry.header.indicator == tarEntryDirectory)
      {
         createfolder(path_combine(destination, entry.header.filename));
      }
      // if the entry is a normal file create the file
      else if(entry.header.indicator == tarEntryNormalFile || 
             entry.header.indicator == tarEntryNormalFileNull)
      {         
         entry.extractfile_to_folder(destination);
      }

      // get the next entry in the TAR archive
      entry = tarf.get_next_entry();
   } while(!entry.is_empty());
}
```
Example 3: a simplified version of the 2nd example
```
void extract3(std::string const &filename, std::string const &destination)
{
   // create tar file with path and read mode
   tarFile tarf(filename, tarModeRead);

   // get the first entry
   tarEntry entry = tarf.get_first_entry();
   do 
   {
      // extract the current entry
      entry.extract(destination);

      // get the next entry in the TAR archive
      entry = tarf.get_next_entry();
   } while(!entry.is_empty());
}
```
Example 4: explicitly process the entries of a TAR file (no auto-extraction to disk, can be in memory processing)
```
void extract4(std::string const& filename)
{
   // create tar file with path and read mode
   tarFile tarf(filename, tarModeRead);

   std::list<tarEntry> entries;

   // get the first entry
   tarEntry entry = tarf.get_first_entry();
   do 
   {
      // add the entry to the list
      entries.push_back(entry);

      // get the next entry in the TAR archive
      entry = tarf.get_next_entry();
   } while(!entry.is_empty());   

   // iterate through the entries
   for(std::list<tarEntry>::iterator it = entries.begin();
      it != entries.end();
      ++it)
   {
      tarEntry& entry = *it;

      // consider the files
      if(entry.header.indicator == tarEntryNormalFile ||
         entry.header.indicator == tarEntryNormalFileNull)
      {
         // position the TAR file cursor at the beginning of the entry
         entry.rewind();

         // read from the TAR file in a chunk
         char chunk[8*1024];
         size_t total = 0;
         do
         {
            size_t readBytes = entry.read(chunk, sizeof(chunk));

            // do something with the read buffer
            // ...

            total += readBytes;
         }while(total < entry.header.filesize);
      }
   }
}
```