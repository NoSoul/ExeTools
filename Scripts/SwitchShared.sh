# !/bin/sh
#if [ $# -ne 1 ]; then
#    echo "Usage: $0 o/c"
#    exit 1;
#fi
#if [ $1 = 'o' ]; then
#    mount -o username=dodo //192.168.1.103/ForLinux /mnt/Desktop/
#    exit 0;
#elif [ $1 = 'c' ]; then
#    umount /mnt/Desktop/
#    exit 0;
#else
#    echo "Usage: $0 o/c"
#    exit 1;
#fi
scp -r wupeng@192.168.1.103:~/public/ForLinux/* .
exit 0;
