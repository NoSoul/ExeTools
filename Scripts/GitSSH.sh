# !/bin/sh
if [ $# -ne 1 ]; then
    echo "Usage: $0 on/off"
    exit 1;
fi
if [ $1 == 'on' ];then
    service sshd start
    service firewalld stop
    echo "ip $(ifconfig | grep inet | tail -n 1 | awk '{print $2}')"
elif [ $1 == 'off' ];then
    service sshd stop
    service firewalld start
else
    exit 1;
fi
