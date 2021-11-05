#!/usr/bin/env bash

FILE=1
COUNT=1
TEXT="Hello world"

if [ $# -gt 0 -a $# -lt 6 ]
then
    if [ $# -ge 2 ]
    then
        if [[ $1 == "-n" && $2 =~ ^[0-9]+$ ]]
        then
            COUNT=$2
        elif [[ $1 == "--file" && $2 =~ ^[0-9]+$ ]]
        then
            FILE=$2
        else
            echo "Invalid flag: $1 $2"
            exit 1
        fi
        if [ $# -ge 4 ]
        then
            if [[ $3 == "-n" && $4 =~ ^[0-9]+$ && $1 != "-n" ]]
             then
               COUNT=$4
            elif [[ $3 == "--file" && $4 =~ ^[0-9]+$ && $1 != "--file" ]]
            then
                FILE=$4
            else
                echo "Invalid flag: $3 $4"
                exit 1
            fi
            if [ $# -eq 5 ]
            then
                TEXT=$5
            fi
        elif [ $# -eq 3 ]
        then
            TEXT=$3
        fi
    else
        TEXT=$1
    fi
elif [ $# -ge 6 ]
then
    echo "Invalid count of arguments: $#"
    exit 1
fi

for (( i=0; i < $COUNT; i++ ))
do
    printf "%s\n" "$TEXT" >& $FILE
done
