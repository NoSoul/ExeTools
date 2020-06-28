# !/bin/sh
if [ $# -ne 1 ];then
    echo "Usage: $0 p/l"
    exit 1;
fi
ip=207.148.22.201
port=22
if [ $1 = 'p' ];then
    ssh -D7070 -p$port -N root@$ip
    exit 0;
elif [ $1 = 'l' ];then
    ssh -p$port root@$ip
    exit 0;
else
    echo "Usage: $0 p/l"
    exit 1;
fi
