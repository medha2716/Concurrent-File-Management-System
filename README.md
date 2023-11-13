# final-project-67
- assumption if user wants to create a file that already exists, its permissions are not changed (can do this later if time)
- else creation of file/dir with all permissions given to everyone
- .c and .h files not counted as part of files/dir in storage server
- copy file/dir into file/dir that doesnt exist(check if it works if already exists not sure)
- sends files/dir every 20 seconds to nm server to update 

- keep a semaphore for each ss, only one client can access an ss whether to delete/create/write/copy, but severa can read and ask for file details
also when delete/create/write/copy, read and file details cannot happen. implememt this in nm server??
(can use a different idea)