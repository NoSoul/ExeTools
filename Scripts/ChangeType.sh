#!/bin/sh
if [ $# -ne 2 ]; then
    echo "Usage: $0 TypeA TypeB"
    exit 1;
fi
for elem in *.$1;do
    echo "mv $elem to ${elem%%$1}$2"
    mv $elem ${elem%%$1}$2 
done
exit 0;
