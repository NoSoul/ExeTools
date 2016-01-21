# !/bin/sh
route del default 2>/dev/null
until [ $? -ne 0 ];do
    echo "del default"
    route del default 2>/dev/null
done
route add default gw 192.168.1.1 metric 1024
exit 0;
