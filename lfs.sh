#!/bin/bash

baseDir=./lfs/
searchDir=""

backup()
{
    if [ ! -d "./$searchDir" ];then
        echo "./$searchDir is not exist"
        return
    fi

    echo "Seeking large files with a size greater than 100M..."
    fileList=$(find ./$searchDir -size +100M)
    for file in $fileList
    do
        path=$(dirname $file)
        if [[ $path =~ ".git" ]]
        then
            continue
        fi
        fullpath="$baseDir${path:2}/"
        if [ ! -d "$fullpath" ];then
            mkdir -p $fullpath
        fi
        echo "Moving $file to $fullpath"
        mv $file $fullpath
    done
}

restore()
{
    if [ ! -d "$baseDir" ];then
        echo "$baseDir is not exist"
        return
    fi

    echo "Restoring large files from $baseDir"
    fileList=$(find $baseDir -type f)
    for file in $fileList
    do
        path=$(dirname $file)
        fullpath="./${path:6}/"
        if [ ! -d "$fullpath" ];then
            mkdir -p $fullpath
        fi
        echo "Moving $file to $fullpath"
        mv $file $fullpath
    done
    rm -rf $baseDir
}


if [ "$1" == "backup" ]  
then
    if [ $# -ne 2 ];
    then
        echo "Path must be specified after backup"
    else
        parameter=$2
        searchDir=${parameter%*/}
        backup 
    fi

elif [ "$1" == "restore" ]
then
    restore 
elif [ "$1" == "test" ]
then
    echo $2
else
    echo " backup  dirname   --Backing up large files with a size greater than 50M to ./lfs/"
    echo " restore   --Restoring large files from ./lfs/"
fi
