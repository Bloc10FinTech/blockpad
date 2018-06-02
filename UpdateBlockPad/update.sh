#!/bin/sh
pathFiles="$1"
cd $pathFiles
touch "startUpdate" 
rm -r "new_package"
mkdir "new_package"
tar -xf "blockpad.tar.xz" -C "new_package"
debFile=$(find "new_package" -name "*.deb")
dpkg -i $debFile
rm -r "new_package"
rm "startUpdate"
