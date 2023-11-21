# final-project-67

## Assumptions
- If user wants to create a file that already exists, nothing is done and permissions of file are not changed.
- If file does not exist, file/dir is created with all permissions given to user, owner and group.
- Writing to file command overwrites content of file. (not append)
- Max of 10 paths and each path of max 1024 char size can be in a storage server at a time.
- Max clients can be 50.
- A server coming back up doesn't have any change in file structure
- Any server must have atleast 1 accessible path.
- Only allowed to read file or read details when server is down.

## How to Run:

For NM server in final-project-67:
```
gcc nfs_server.c mytrie.c
./a.out
```

For Running Storage Server code:
(in ss folder or its copies)
```
make
./ss
```

For Client: (in Clients)
```
gcc client.c
./a.out
```

## How functions have been defined:

### For copying file/folders:

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
