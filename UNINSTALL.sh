#!/bin/bash

dest_dir_LIB="/usr/local/lib"

# Check if the file exists
if [ -e "$dest_dir_LIB/libscriber.a" ]; then

    # Copy the file
    sudo rm $dest_dir_LIB/libscriber.a

    # Check if the copy was successful
    if [ $? -eq 0 ]; then
        echo "File libscriber.a removed successfully from $dest_dir_LIB"
    else
        echo "Failed to remove libscriber.a from $dest_dir_LIB"
    fi
else
    echo "File libscriber.a does not exist in $dest_dir_LIB"
fi

#!/bin/bash

# Destination directory
dest_dir_HEAD="/usr/include/scriber"

# Check if the destination directory exists
if [ -d "$dest_dir_HEAD" ]; then
    # Delete all files from the destination directory
    sudo rm -r "$dest_dir_HEAD"/*

    # Remove the destination directory itself
    sudo rmdir "$dest_dir_HEAD"

    echo "Files deleted from $dest_dir_HEAD, and $dest_dir_HEAD removed."
else
    echo "$dest_dir_HEAD does not exist."
fi
