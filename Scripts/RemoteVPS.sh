# !/bin/sh
if [ $# -ne 1 ];then
    echo "Usage: $0 p/l"
    exit 1;
fi
if [ $1 = 'p' ];then
    ssh -D7070 -p29542 -N root@45.78.57.111
    exit 0;
elif [ $1 = 'l' ];then
    ssh -p29542 root@45.78.57.111
    exit 0;
else
    echo "Usage: $0 p/l"
    exit 1;
fi
