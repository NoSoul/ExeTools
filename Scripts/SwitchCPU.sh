# !/bin/sh
if [ $# -ne 1 ];then
    echo "Usage: $0 p/s"
    exit 1;
fi
if [ $1 = 'p' ];then
    for((i=0; i<4; ++i));do
        cpupower -c $i frequency-set -g performance
    done
    exit 0;
elif [ $1 = 's' ];then
    for((i=0; i<4; ++i));do
        cpupower -c $i frequency-set -g ondemand        
    done
    exit 0;
else
    echo "Usage: $0 p/s"
    exit 1;
fi
