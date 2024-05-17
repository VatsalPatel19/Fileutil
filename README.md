fileutil: A C Program for File Operations
fileutil is a C program designed to perform various file operations within the home directory subtree of the OS. It utilizes the nftw() function to traverse directory trees, providing functionality to search, copy, move, and archive files based on user-specified arguments and options.

Synopsis
Basic Search
fileutil [root_dir] filename
Searches for a file and displays its absolute path if found; otherwise, prints "Search Unsuccessful".

root_dir: Absolute or relative path (relative to the home directory) of the directory subtree root.
filename: Name of the file to search for.
Example:
$fileutil ~/chapter4 check.txt
Output:
/home/username/chapter4/dir2/check.txt
or
Search Unsuccessful
Search and Copy/Move
fileutil [root_dir] [storage_dir] [options] filename
Searches for filename in root_dir, displays its absolute path if found, and copies or moves it to storage_dir based on the specified option.

root_dir: Path of the directory subtree root.
storage_dir: Path of the directory where the file will be copied or moved.
options: -cp to copy, -mv to move.
filename: Name of the file to search for.
Example:
$fileutil ~/chapter4 ~/backup -cp check.txt
Output:
/home/username/chapter4/dir2/check.txt
or
Search Unsuccessful
Search by Extension and Archive
fileutil [root_dir] [storage_dir] extension
Searches for all files with the given extension in root_dir, lists their absolute paths, and creates a tar file a1.tar in storage_dir.

root_dir: Path of the directory subtree root.
storage_dir: Path where the tar file will be created.
extension: File extension to search for (e.g., .c, .txt, .pdf).
Example:
$fileutil ~/chapter4 ~/backup .txt
Output:
/home/username/chapter4/file1.txt
/home/username/chapter4/dir2/file2.txt
...
Error Handling
The program prints appropriate error messages for invalid inputs or operations.

nftw() Function: The program uses the nftw() function to traverse directory trees recursively.
Comments and Documentation: The code includes adequate comments explaining the functionality and logic.
Refer to the Linux manual on nftw() for detailed usage and the assignment rubrics for submission requirements.






