#!/bin/sh

#Script to make a new jar executable of sstf, takes output filename as argument

if [ $# -ne 1 ]
 then
         echo "Usage: $0 [Output jar file name]"
         exit 127
 fi

cd bin

jar cfm $1.jar Manifest.txt strezzer/*.class

mv $1.jar ../

cd ..

