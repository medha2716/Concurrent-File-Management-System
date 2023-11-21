# final-project-67
- assumption if user wants to create a file that already exists, its permissions are not changed (can do this later if time)
- else creation of file/dir with all permissions given to everyone
- .c and .h files not counted as part of files/dir in storage server
- copy file/dir into file/dir that doesnt exist(check if it works if already exists not sure)
- sends files/dir every 20 seconds to nm server to update 

- keep a semaphore for each ss, only one client can access an ss whether to delete/create/write/copy, but severa can read and ask for file details
also when delete/create/write/copy, read and file details cannot happen. implememt this in nm server??
(can use a different idea)

- i am writing to file not appending
- max of 50 paths and each path of max 1024 char size can be in a storage server at a time

- assuming a server coming back up doesn't have any change in file structure
- also any server has atleast 1 accessible path

- if this could be changed:
    right now main_dir and main_dir/file.txt needs to be entered as 2 different paths accessible 

- only allowed to read file or read details when server down
- not allowing copy from either for now

- write name of folder that doesnt exist as part of file structure in ss2 and then use copy file dir with that name as destpath

- paths entered: abc.txt file.txt, but file.txt must be passed as accessible path in ss2, may or may not exist in structure

- so for redundancy right now i am copying into previous two servers if not down

- Input: src_file dest_file
1. dest_file must exist in dest server, if it is part of accessible paths(even if it doesn't exist) then a new file is created
2. Its content is overwritten by content of src_file

- Input: src_file dest_folder/dest_file_name
1. dest_folder/dest_file_name must be part of accessible paths of some server (here, dest_folder needs to exist, dest_file may/may not initially exist)
2. Its content is overwritten by content of src_file

- Input: 
src_dir dest_dir
1. All files and folders inside src_dir are copied into dest_dir
2. dest_dir needs to exist in some server
